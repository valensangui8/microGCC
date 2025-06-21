#include "SemanticAnalyzer.h"
#include <string.h>

static Logger* _logger = NULL;
static SemanticContext* _context = NULL;

static bool _analyzeProgram(Program* program);
static bool _analyzeDeclarationList(DeclarationList* list);
static bool _analyzeDeclaration(Declaration* decl);
static bool _analyzeFunction(Declaration* funcDecl);
static bool _analyzeBlock(Block* block);
static bool _analyzeStatements(Statements* stmts);
static bool _analyzeStatement(Statement* stmt);
static DataType _analyzeExpression(Expression* expr);
static bool _analyzeArguments(ListArguments* args, SymbolEntry* function);
static void _reportError(SemanticError error, const char* details);
static bool _checkTypeCompatibility(DataType expected, DataType actual);

/* PUBLIC FUNCTIONS */

#define CUR_FN (_context->currentFunctionName)

void initializeSemanticAnalyzerModule() {
    _logger = createLogger("SemanticAnalyzer");
    initializeSymbolTableModule();
}

void shutdownSemanticAnalyzerModule() {
    if (_context != NULL && _context->symbolTable != NULL) {
        destroySymbolTable(_context->symbolTable);
    }
    if (_context != NULL) {
        free(_context);
        _context = NULL;
    }
    shutdownSymbolTableModule();
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}

bool analyzeSemantics(CompilerState* compilerState) {
    logDebugging(_logger, "Starting semantic analysis...");

    // Initialize context
    _context = calloc(1, sizeof(SemanticContext));
    _context->symbolTable = createSymbolTable();
    _context->hasErrors = false;
    _context->currentFunctionReturnType = TYPE_INT;
    _context->currentFunctionName = NULL;
    _context->functionHasReturn = false;

    // Analyze the program
    bool success = _analyzeProgram(compilerState->abstractSyntaxtTree);

    // Check that all non-extern declared functions are defined
    if (success) {
        SymbolEntry* current = _context->symbolTable->head;
        while (current != NULL) {
            if (current->symbolType == SYMBOL_FUNCTION && current->offset == -1) {
                // Function was declared but not defined and is not extern
                _reportError(SEMANTIC_ERROR_UNDEFINED_FUNCTION, current->name);
                _context->hasErrors = true;
                success = false;
            }
            current = current->next;
        }
    }

    if (_context->hasErrors) {
        logError(_logger, "Semantic analysis failed with errors");
        destroySymbolTable(_context->symbolTable);
        _context->symbolTable = NULL;
        success = false;
    } else {
        logDebugging(_logger, "Semantic analysis completed successfully");
    }

    return success;
}

SymbolTable* getSymbolTable() {
    if (_context != NULL) {
        return _context->symbolTable;
    }
    return NULL;
}

/* PRIVATE FUNCTIONS */

static bool _analyzeProgram(Program* program) {
    if (program->type == PROGRAM_EMPTY) {
        return true;
    }
    return _analyzeDeclarationList(program->declarationList);
}

static bool _analyzeDeclarationList(DeclarationList* list) {
    while (list != NULL) {
        if (!_analyzeDeclaration(list->declaration)) {
            return false;
        }
        list = list->next;
    }
    return true;
}


static bool _analyzeDeclarationSuffixVariable(Declaration * decl, SymbolEntry * existing){
    // Variable declaration - no redeclaration allowed
    if (existing != NULL) {          //todo cambiar aca (y capaz otro semantic_error_redeclared) para que te deje cambiar identificadores globales en funciones.
        _reportError(SEMANTIC_ERROR_REDECLARED_IDENTIFIER, *decl->identifier);
        return false;
    }

    VariableSuffix* vs = decl->declarationSuffix->variableSuffix;
    int isArray = (vs->type == VARIABLE_SUFFIX_ARRAY);
    int arraySize = isArray ? *vs->arraySize : 0;

    // Check array size
    if (isArray && arraySize <= 0) {
        _reportError(SEMANTIC_ERROR_ARRAY_SIZE, *decl->identifier);
        return false;
    }

    // Add to symbol table
    addVariable(_context->symbolTable, *decl->identifier, decl->dataType, isArray, arraySize, CUR_FN);

    // Check initialization if present
    if (vs->type == VARIABLE_SUFFIX_ASSIGNMENT) {
        DataType exprType = _analyzeExpression(vs->expression);
        if (!_checkTypeCompatibility(decl->dataType, exprType)) {
            _reportError(SEMANTIC_ERROR_TYPE_MISMATCH, *decl->identifier);
            return false;
        }
    }
    return true;
}

static bool _analyzeDeclarationSuffixFunction(Declaration * decl, SymbolEntry * existing) {
    int paramCount = 0;
    if (decl->declarationSuffix->parameters->type == PARAMS_LIST) {
        ParameterList* p = decl->declarationSuffix->parameters->list;
        while (p != NULL) {
            paramCount++;
            p = p->next;
        }
    }

    bool isDefinition = (decl->declarationSuffix->functionSuffix->type == SUFFIX_BLOCK);
    bool isExtern = (decl->declarationType == DECLARATION_EXTERN);

    if (existing != NULL) {
        // Function already exists - check compatibility
        if (existing->symbolType != SYMBOL_FUNCTION) {
            _reportError(SEMANTIC_ERROR_REDECLARED_IDENTIFIER, *decl->identifier);
            return false;
        }

        // Check return type and parameter count match
        if (existing->dataType != decl->dataType || existing->paramCount != paramCount) {
            _reportError(SEMANTIC_ERROR_TYPE_MISMATCH, *decl->identifier);
            return false;
        }

        // If this is a definition and function was already defined (has non-negative offset)
        if (isDefinition && existing->offset >= 0) {
            _reportError(SEMANTIC_ERROR_REDECLARED_IDENTIFIER, *decl->identifier);
            return false;
        }

        // Mark function as defined if this is a definition
        if (isDefinition) {
            existing->offset = 0;  // Mark as defined
        }
    } else {
        // First time seeing this function - add to symbol table
        addFunction(_context->symbolTable, *decl->identifier, decl->dataType, paramCount);
        SymbolEntry* funcEntry = lookupSymbol(_context->symbolTable, *decl->identifier, CUR_FN);

        if (isDefinition) {
            funcEntry->offset = 0;  // Mark as defined
        } else if (isExtern) {
            funcEntry->offset = -2;  // Mark as extern
        } else {
            funcEntry->offset = -1;  // Mark as declared but not defined
        }
    }

    // Analyze function body if present
    if (isDefinition) {
        return _analyzeFunction(decl);
    }
    return true;
}

static bool _analyzeDeclaration(Declaration* decl) {
    SymbolEntry* existing = lookupSymbol(_context->symbolTable, *decl->identifier, CUR_FN);

    if (decl->declarationSuffix->type == DECLARATION_SUFFIX_VARIABLE) {
        // Variable declaration - no redeclaration allowed
        return _analyzeDeclarationSuffixVariable(decl, existing);
        /*if (existing != NULL) {
            _reportError(SEMANTIC_ERROR_REDECLARED_IDENTIFIER, *decl->identifier);
            return false;
        }

        VariableSuffix* vs = decl->declarationSuffix->variableSuffix;
        int isArray = (vs->type == VARIABLE_SUFFIX_ARRAY);
        int arraySize = isArray ? *vs->arraySize : 0;

        // Check array size
        if (isArray && arraySize <= 0) {
            _reportError(SEMANTIC_ERROR_ARRAY_SIZE, *decl->identifier);
            return false;
        }

        // Add to symbol table
        addVariable(_context->symbolTable, *decl->identifier, decl->dataType, isArray, arraySize);

        // Check initialization if present
        if (vs->type == VARIABLE_SUFFIX_ASSIGNMENT) {
            DataType exprType = _analyzeExpression(vs->expression);
            if (!_checkTypeCompatibility(decl->dataType, exprType)) {
                _reportError(SEMANTIC_ERROR_TYPE_MISMATCH, *decl->identifier);
                return false;
            }
        }*/
    } //else {
        // Function declaration/definition
        /*int paramCount = 0;
        if (decl->declarationSuffix->parameters->type == PARAMS_LIST) {
            ParameterList* p = decl->declarationSuffix->parameters->list;
            while (p != NULL) {
                paramCount++;
                p = p->next;
            }
        }

        bool isDefinition = (decl->declarationSuffix->functionSuffix->type == SUFFIX_BLOCK);
        bool isExtern = (decl->declarationType == DECLARATION_EXTERN);

        if (existing != NULL) {
            // Function already exists - check compatibility
            if (existing->symbolType != SYMBOL_FUNCTION) {
                _reportError(SEMANTIC_ERROR_REDECLARED_IDENTIFIER, *decl->identifier);
                return false;
            }

            // Check return type and parameter count match
            if (existing->dataType != decl->dataType || existing->paramCount != paramCount) {
                _reportError(SEMANTIC_ERROR_TYPE_MISMATCH, *decl->identifier);
                return false;
            }

            // If this is a definition and function was already defined (has non-negative offset)
            if (isDefinition && existing->offset >= 0) {
                _reportError(SEMANTIC_ERROR_REDECLARED_IDENTIFIER, *decl->identifier);
                return false;
            }

            // Mark function as defined if this is a definition
            if (isDefinition) {
                existing->offset = 0;  // Mark as defined
            }
        } else {
            // First time seeing this function - add to symbol table
            addFunction(_context->symbolTable, *decl->identifier, decl->dataType, paramCount);
            SymbolEntry* funcEntry = lookupSymbol(_context->symbolTable, *decl->identifier);

            if (isDefinition) {
                funcEntry->offset = 0;  // Mark as defined
            } else if (isExtern) {
                funcEntry->offset = -2;  // Mark as extern
            } else {
                funcEntry->offset = -1;  // Mark as declared but not defined
            }
        }

        // Analyze function body if present
        if (isDefinition) {
            return _analyzeFunction(decl);
        }*/
        return _analyzeDeclarationSuffixFunction(decl, existing);

  //  }

   // return true;
}

static bool _analyzeFunction(Declaration* funcDecl) {
    logDebugging(_logger, "Analyzing function: %s", *funcDecl->identifier);

    // Save current context
    DataType previousReturnType = _context->currentFunctionReturnType;
    char* previousFunctionName = _context->currentFunctionName;
    bool previousHasReturn = _context->functionHasReturn;

    // Set up new function context
    _context->currentFunctionReturnType = funcDecl->dataType;
    _context->currentFunctionName = *funcDecl->identifier;
    _context->functionHasReturn = false;

    // Reset offset for new function
    resetOffset(_context->symbolTable);

    // Add parameters to symbol table
    if (funcDecl->declarationSuffix->parameters->type == PARAMS_LIST) {
        ParameterList* p = funcDecl->declarationSuffix->parameters->list;
        int paramOffset = 16; // Skip old BP and return address

        while (p != NULL) {
            Parameter* param = p->parameter;

            // Check for duplicate parameter names
            if (lookupSymbol(_context->symbolTable, *param->identifier, CUR_FN) != NULL) {
                _reportError(SEMANTIC_ERROR_REDECLARED_IDENTIFIER, *param->identifier);
                return false;
            }

            if(param->array->type == PARAMETER_ARRAY_BRACKETS){
                addParameter(_context->symbolTable, *param->identifier, param->type, paramOffset, 1 , UNKNOWN_ARRAY_SIZE, CUR_FN); /*TODO no sabemos el array size*/
            }else{
                addParameter(_context->symbolTable, *param->identifier, param->type, paramOffset, 0 ,0, CUR_FN);
            }
            //addParameter(_context->symbolTable, *param->identifier, param->type, paramOffset); // todo estaba esto antes. (esta raro porque le pongo arraySize = -1, podria ser un unknown o cambiar la gramatica. )
            //parece estar bien solo que en la generacion de codigo hace un mov en vez de un lea
            paramOffset += 8; // All parameters are word-sized in 8086
            p = p->next;
        }
    }

    // Analyze function body
    bool success = _analyzeBlock(funcDecl->declarationSuffix->functionSuffix->block);

    // Check if non-void function has return
    if (funcDecl->dataType != TYPE_CHAR && funcDecl->dataType != TYPE_INT) {
        // We don't have void in our language, this shouldn't happen
        logError(_logger, "Invalid return type for function %s", *funcDecl->identifier);
        success = false;
    } else if (!_context->functionHasReturn) {
        // All functions must have explicit return in our subset of C
        _reportError(SEMANTIC_ERROR_MISSING_RETURN, *funcDecl->identifier);
        success = false;
    }

    // Restore previous context
    _context->currentFunctionReturnType = previousReturnType;
    _context->currentFunctionName = previousFunctionName;
    _context->functionHasReturn = previousHasReturn;

    return success;
}

static bool _analyzeBlock(Block* block) {
    if (block->statements != NULL) {
        return _analyzeStatements(block->statements);
    }
    return true;
}

static bool _analyzeStatements(Statements* stmts) {
    while (stmts != NULL) {
        if (!_analyzeStatement(stmts->statement)) {
            return false;
        }
        stmts = stmts->next;
    }
    return true;
}

static bool _analyzeStatement(Statement* stmt) {
    switch (stmt->type) {
        case STATEMENT_DECLARATION: {
            // Check if identifier already exists
            if (lookupSymbol(_context->symbolTable, *stmt->identifier, CUR_FN) != NULL) {
                _reportError(SEMANTIC_ERROR_REDECLARED_IDENTIFIER, *stmt->identifier);
                return false;
            }

            VariableSuffix* vs = stmt->variableSuffix;
            int isArray = (vs->type == VARIABLE_SUFFIX_ARRAY);
            int arraySize = isArray ? *vs->arraySize : 0;

            if (isArray && arraySize <= 0) {
                _reportError(SEMANTIC_ERROR_ARRAY_SIZE, *stmt->identifier);
                return false;
            }

            addVariable(_context->symbolTable, *stmt->identifier, stmt->dataType, isArray, arraySize, CUR_FN);

            if (vs->type == VARIABLE_SUFFIX_ASSIGNMENT) {
                DataType exprType = _analyzeExpression(vs->expression);
                if (!_checkTypeCompatibility(stmt->dataType, exprType)) {
                    _reportError(SEMANTIC_ERROR_TYPE_MISMATCH, *stmt->identifier);
                    return false;
                }
            }
            break;
        }

        case STATEMENT_IF: {
            DataType condType = _analyzeExpression(stmt->statementIf->condition);
            if (condType == -1) return false;

            if (!_analyzeBlock(stmt->statementIf->thenBlock)) return false;

            if (stmt->statementIf->hasElse) {
                if (!_analyzeBlock(stmt->statementIf->elseBlock)) return false;
            }
            break;
        }

        case STATEMENT_WHILE: {
            DataType condType = _analyzeExpression(stmt->statementWhile->condition);
            if (condType == -1) return false;

            if (!_analyzeBlock(stmt->statementWhile->block)) return false;
            break;
        }

        case STATEMENT_FOR: {
            if (stmt->statementFor->hasInit) {
                DataType initType = _analyzeExpression(stmt->statementFor->init);
                if (initType == -1) return false;
            }

            if (stmt->statementFor->hasCondition) {
                DataType condType = _analyzeExpression(stmt->statementFor->condition);
                if (condType == -1) return false;
            }

            if (stmt->statementFor->hasUpdate) {
                DataType updateType = _analyzeExpression(stmt->statementFor->update);
                if (updateType == -1) return false;
            }

            if (!_analyzeBlock(stmt->statementFor->block)) return false;
            break;
        }

        case STATEMENT_RETURN: {
            _context->functionHasReturn = true;

            if (stmt->statementReturn->hasExpression) {
                DataType exprType = _analyzeExpression(stmt->statementReturn->expression);
                if (exprType == -1) return false;

                if (!_checkTypeCompatibility(_context->currentFunctionReturnType, exprType)) {
                    _reportError(SEMANTIC_ERROR_RETURN_TYPE_MISMATCH, _context->currentFunctionName);
                    return false;
                }
            }
            break;
        }

        case STATEMENT_EXPRESSION: {
            DataType type = _analyzeExpression(stmt->statementExpression->expression);
            if (type == -1) return false;
            break;
        }

        case STATEMENT_BLOCK:
            return _analyzeBlock(stmt->block);

        case STATEMENT_EMPTY:
            break;
    }

    return true;
}

static boolean canAssignToLValue(Expression * leftExpression){
    if (leftExpression->type == EXPRESSION_IDENTIFIER) {
        SymbolEntry *lhsSym =
                lookupSymbol(_context->symbolTable,
                             *leftExpression->identifier,
                             CUR_FN);
        if (lhsSym != NULL && lhsSym->isArray) {
            return false;
        }
    }
    return true;
}

static DataType _analyzeExpression(Expression* expr) {
    switch (expr->type) {
        case EXPRESSION_ASSIGNMENT: {

            if(!canAssignToLValue(expr->leftExpression)){
                _reportError(SEMANTIC_ERROR_TYPE_MISMATCH, "assignment of array");
                return -1;
            }

            DataType leftType = _analyzeExpression(expr->leftExpression);
            DataType rightType = _analyzeExpression(expr->rightExpression);

            if (leftType == -1 || rightType == -1) return -1;

            if (!_checkTypeCompatibility(leftType, rightType)) {
                _reportError(SEMANTIC_ERROR_TYPE_MISMATCH, "assignment");
                return -1;
            }

            return leftType;
        }

        case EXPRESSION_OR:
        case EXPRESSION_AND:/* {
            DataType leftType = _analyzeExpression(expr->leftExpression);
            DataType rightType = _analyzeExpression(expr->rightExpression);

            if (leftType == -1 || rightType == -1) return -1;

            return TYPE_INT; // Boolean operations return int
        }*/
        case EXPRESSION_EQUAL:
        case EXPRESSION_NOT_EQUAL:
        case EXPRESSION_LESS:
        case EXPRESSION_GREATER:
        case EXPRESSION_LESS_EQUAL:
        case EXPRESSION_GREATER_EQUAL: /*{
            DataType leftType = _analyzeExpression(expr->leftExpression);
            DataType rightType = _analyzeExpression(expr->rightExpression);

            if (leftType == -1 || rightType == -1) return -1;

            return TYPE_INT; // Comparison operations return int
        }*/

        case EXPRESSION_ADDITION:
        case EXPRESSION_SUBTRACTION:
        case EXPRESSION_MULTIPLICATION:
        case EXPRESSION_DIVISION:
        case EXPRESSION_MODULO: {
            DataType leftType = _analyzeExpression(expr->leftExpression);
            DataType rightType = _analyzeExpression(expr->rightExpression);

            if (leftType == -1 || rightType == -1) return -1;

            // Both operands should be numeric
            return TYPE_INT;
        }

        case EXPRESSION_NOT: {
            DataType type = _analyzeExpression(expr->singleExpression);
            if (type == -1) return -1;

            return TYPE_INT; // NOT operation returns int
        }

        case EXPRESSION_IDENTIFIER: {
            SymbolEntry* symbol = lookupSymbol(_context->symbolTable, *expr->identifier, CUR_FN);
            if (symbol == NULL) {
                _reportError(SEMANTIC_ERROR_UNDECLARED_VARIABLE, *expr->identifier);
                return -1;
            }

            return symbol->dataType;
        }

        case EXPRESSION_CONSTANT:
            return expr->constant->type;

        case EXPRESSION_PARENTHESIS:
            return _analyzeExpression(expr->singleExpression);

        case EXPRESSION_ARRAY_ACCESS: {
            SymbolEntry* symbol = lookupSymbol(_context->symbolTable, *expr->identifierArray, CUR_FN);
            if (symbol == NULL) {
                _reportError(SEMANTIC_ERROR_UNDECLARED_VARIABLE, *expr->identifierArray);
                return -1;
            }

            if (!symbol->isArray) {
                _reportError(SEMANTIC_ERROR_NON_ARRAY_INDEXED, *expr->identifierArray);
                return -1;
            }

            DataType indexType = _analyzeExpression(expr->indexExpression);
            if (indexType == -1) return -1;

            if (indexType != TYPE_INT) {
                _reportError(SEMANTIC_ERROR_ARRAY_INDEX_TYPE, *expr->identifierArray);
                return -1;
            }

            return symbol->dataType;
        }

        case EXPRESSION_FUNCTION_CALL: {
            SymbolEntry* function = lookupSymbol(_context->symbolTable, *expr->identifierFunc, CUR_FN);
            if (function == NULL) {
                _reportError(SEMANTIC_ERROR_UNDECLARED_FUNCTION, *expr->identifierFunc);
                return -1;
            }

            if (function->symbolType != SYMBOL_FUNCTION) {
                _reportError(SEMANTIC_ERROR_UNDECLARED_FUNCTION, *expr->identifierFunc);
                return -1;
            }

            // Count arguments
            int argCount = 0;
            ListArguments* args = expr->arguments;
            while (args != NULL) {
                argCount++;
                DataType argType = _analyzeExpression(args->expression);
                if (argType == -1) return -1;
                args = args->next;
            }

            if (argCount != function->paramCount) {
                _reportError(SEMANTIC_ERROR_WRONG_ARGUMENT_COUNT, *expr->identifierFunc);
                return -1;
            }

            return function->dataType;
        }
    }

    return -1; // Should not reach here
}

static void _reportError(SemanticError error, const char* details) {
    _context->hasErrors = true;

    switch (error) {
        case SEMANTIC_ERROR_UNDECLARED_VARIABLE:
            logError(_logger, "Undeclared variable: %s", details);
            break;
        case SEMANTIC_ERROR_UNDECLARED_FUNCTION:
            logError(_logger, "Undeclared function: %s", details);
            break;
        case SEMANTIC_ERROR_UNDEFINED_FUNCTION:
            logError(_logger, "Function declared but not defined: %s", details);
            break;
        case SEMANTIC_ERROR_REDECLARED_IDENTIFIER:
            logError(_logger, "Identifier already declared: %s", details);
            break;
        case SEMANTIC_ERROR_TYPE_MISMATCH:
            logError(_logger, "Type mismatch in %s", details);
            break;
        case SEMANTIC_ERROR_WRONG_ARGUMENT_COUNT:
            logError(_logger, "Wrong number of arguments for function: %s", details);
            break;
        case SEMANTIC_ERROR_ARRAY_INDEX_TYPE:
            logError(_logger, "Array index must be integer: %s", details);
            break;
        case SEMANTIC_ERROR_NON_ARRAY_INDEXED:
            logError(_logger, "Cannot index non-array variable: %s", details);
            break;
        case SEMANTIC_ERROR_VOID_VARIABLE:
            logError(_logger, "Cannot declare void variable: %s", details);
            break;
        case SEMANTIC_ERROR_RETURN_TYPE_MISMATCH:
            logError(_logger, "Return type mismatch in function: %s", details);
            break;
        case SEMANTIC_ERROR_MISSING_RETURN:
            logError(_logger, "Missing return statement in function: %s", details);
            break;
        case SEMANTIC_ERROR_ARRAY_SIZE:
            logError(_logger, "Invalid array size for: %s", details);
            break;
    }
}

static bool _checkTypeCompatibility(DataType expected, DataType actual) {
    // For now, we're strict about types
    // In the future, we might allow char to int promotion
    return expected == actual;
}