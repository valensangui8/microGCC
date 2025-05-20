#include "BisonActions.h"
#include <string.h>
/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
    _logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
    logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

// Program actions
Program* DeclarationListProgramSemanticAction(CompilerState* compilerState, DeclarationList* declarationList) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program* program = calloc(1, sizeof(Program));
    program->declarationList = declarationList;
    program->type = PROGRAM_DECLARATIONS;
    compilerState->abstractSyntaxtTree = program;
    if (0 < flexCurrentContext()) {
        logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
        compilerState->succeed = false;
    }
    else {
        compilerState->succeed = true;
    }
    return program;
}

Program* EmptyProgramSemanticAction(CompilerState* compilerState) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program* program = calloc(1, sizeof(Program));
    program->type = PROGRAM_EMPTY;
    compilerState->abstractSyntaxtTree = program;
    if (0 < flexCurrentContext()) {
        logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
        compilerState->succeed = false;
    }
    else {
        compilerState->succeed = true;
    }
    return program;
}

// Declaration actions
DeclarationList* SingleDeclarationListSemanticAction(Declaration* declaration) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DeclarationList* list = calloc(1, sizeof(DeclarationList));
    list->declaration = declaration;
    list->next = NULL;
    return list;
}

DeclarationList* AppendDeclarationListSemanticAction(DeclarationList* list, Declaration* declaration) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DeclarationList* newList = calloc(1, sizeof(DeclarationList));
    newList->declaration = declaration;
    newList->next = NULL;
    
    DeclarationList* current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = newList;
    
    return list;
}

Declaration* RegularDeclarationSemanticAction(DataType type, Identifier * identifier, DeclarationSuffix* declarationSuffix) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Declaration* declaration = calloc(1, sizeof(Declaration));
    declaration->dataType = type;  // No conditional check needed
    declaration->identifier = identifier;
    declaration->declarationSuffix = declarationSuffix;
    declaration->declarationType = DECLARATION_REGULAR;
    return declaration;
}

Declaration* ExternDeclarationSemanticAction(DataType type, Identifier * identifier, DeclarationSuffix* declarationSuffix) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Declaration* declaration = calloc(1, sizeof(Declaration));
    declaration->dataType = type;
    declaration->identifier = identifier;
    declaration->declarationSuffix = declarationSuffix;
    declaration->declarationType = DECLARATION_EXTERN;
    return declaration;
}

DeclarationSuffix* VariableDeclarationSuffixSemanticAction(VariableSuffix* variableSuffix) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DeclarationSuffix* suffix = calloc(1, sizeof(DeclarationSuffix));
    suffix->variableSuffix = variableSuffix;
    suffix->type = VARIABLE_SUFFIX_ARRAY;
    return suffix;
}

DeclarationSuffix* FunctionDeclarationSuffixSemanticAction(Parameters* parameters, FunctionSuffix* functionSuffix) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DeclarationSuffix* suffix = calloc(1, sizeof(DeclarationSuffix));
    suffix->parameters = parameters;
    suffix->functionSuffix = functionSuffix;
    suffix->type = DECLARATION_SUFFIX_FUNCTION;
    return suffix;
}

FunctionSuffix* EmptyFuncionSuffixSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    FunctionSuffix* suffix = calloc(1, sizeof(FunctionSuffix));
    suffix->type = 0;
    return suffix;
}

FunctionSuffix* BlockFuncionSuffixSemanticAction(Block* block) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    FunctionSuffix* suffix = calloc(1, sizeof(FunctionSuffix));
    suffix->block = block;
    suffix->type = 1;
    return suffix;
}

VariableSuffix* EmptyVariableSuffixSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    VariableSuffix* suffix = calloc(1, sizeof(VariableSuffix));
    suffix->type = VARIABLE_SUFFIX_NONE;
    return suffix;
}

VariableSuffix* AssignmentVariableSuffixSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    VariableSuffix* suffix = calloc(1, sizeof(VariableSuffix));
    suffix->expression = expression;
    suffix->type = VARIABLE_SUFFIX_ASSIGNMENT;
    return suffix;
}

VariableSuffix* ArrayVariableSuffixSemanticAction(int size) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    VariableSuffix* suffix = calloc(1, sizeof(VariableSuffix));
    ConstantInteger* arraySize = calloc(1, sizeof(ConstantInteger));
    *arraySize = size;
    suffix->arraySize = arraySize;
    suffix->type = VARIABLE_SUFFIX_ARRAY;
    return suffix;
}

// Parameter actions
Parameters* VoidParametrosSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Parameters* parameters = calloc(1, sizeof(Parameters));
    parameters->type = 0;
    return parameters;
}

Parameters* ListParametrosSemanticAction(ParameterList* list) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Parameters* parameters = calloc(1, sizeof(Parameters));
    parameters->list = list;
    parameters->type = 1;
    return parameters;
}

Parameters* EmptyParametrosSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Parameters* parameters = calloc(1, sizeof(Parameters));
    parameters->type = 2;
    return parameters;
}

ParameterList* SingleParametroListSemanticAction(Parameter* parameter) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ParameterList* list = calloc(1, sizeof(ParameterList));
    list->parameter = parameter;
    list->next = NULL;
    return list;
}

ParameterList* AppendParametroListSemanticAction(Parameter* parameter, ParameterList* list) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ParameterList* newList = calloc(1, sizeof(ParameterList));
    newList->parameter = parameter;
    newList->next = list;
    return newList;
}


Parameter* ParameterSemanticAction(DataType type, Identifier* identifier, ParameterArray * array) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Parameter* parameter = calloc(1, sizeof(Parameter));
    parameter->type = type;
    parameter->identifier = identifier;
    parameter->array = array;
    return parameter;
}

ParameterArray * ArrayParametroArraySemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ParameterArray * array = calloc(1, sizeof(ParameterArray));
    array->type = PARAMETER_ARRAY_BRACKETS;
    return array;
}

ParameterArray * EmptyParametroArraySemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ParameterArray * array = calloc(1, sizeof(ParameterArray));
    array->type = PARAMETER_ARRAY_NONE;
    return array;
}

// Block and statement actions
Block* BlockSemanticAction(Statements* statements) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Block* block = calloc(1, sizeof(Block));
    block->statements = statements;
    return block;
}

Statements* AppendStatementsSemanticAction(Statement* statement, Statements* statements) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statements* newStatements = calloc(1, sizeof(Statements));
    newStatements->statement = statement;
    newStatements->next = statements;
    return newStatements;
}

Statements* EmptyStatementsSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    return NULL;
}


Statement* DeclarationStatementSemanticAction(DataType type, Identifier* identifier, VariableSuffix* variableSuffix) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement* statement = calloc(1, sizeof(Statement));
    statement->dataType = type;
    statement->identifier = identifier;
    statement->variableSuffix = variableSuffix;
    statement->type = STATEMENT_DECLARATION;
    return statement;
}

Statement* IfStatementSemanticAction(StatementIf* statementIf) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement* statement = calloc(1, sizeof(Statement));
    statement->statementIf = statementIf;
    statement->type = STATEMENT_IF;
    return statement;
}

Statement* WhileStatementSemanticAction(StatementWhile* statementWhile) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement* statement = calloc(1, sizeof(Statement));
    statement->statementWhile = statementWhile;
    statement->type = STATEMENT_WHILE;
    return statement;
}

Statement* ForStatementSemanticAction(StatementFor* statementFor) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement* statement = calloc(1, sizeof(Statement));
    statement->statementFor = statementFor;
    statement->type = STATEMENT_FOR;
    return statement;
}

Statement* ReturnStatementSemanticAction(StatementReturn* statementReturn) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement* statement = calloc(1, sizeof(Statement));
    statement->statementReturn = statementReturn;
    statement->type = STATEMENT_RETURN;
    return statement;
}

Statement* ExpressionStatementSemanticAction(StatementExpression* statementExpression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement* statement = calloc(1, sizeof(Statement));
    statement->statementExpression = statementExpression;
    statement->type = STATEMENT_EXPRESSION;
    return statement;
}

Statement* BlockStatementSemanticAction(Block* block) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement* statement = calloc(1, sizeof(Statement));
    statement->block = block;
    statement->type = STATEMENT_BLOCK;
    return statement;
}

Statement* EmptyStatementSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Statement* statement = calloc(1, sizeof(Statement));
    statement->type = STATEMENT_EMPTY;
    return statement;
}

StatementExpression* StatementExpressionSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StatementExpression* statement = calloc(1, sizeof(StatementExpression));
    statement->expression = expression;
    return statement;
}

StatementIf* SimpleStatementIfSemanticAction(Expression* condition, Block* thenBlock) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StatementIf* statement = calloc(1, sizeof(StatementIf));
    statement->condition = condition;
    statement->thenBlock = thenBlock;
    statement->hasElse = 0;
    return statement;
}

StatementIf* WithElseStatementIfSemanticAction(Expression* condition, Block* thenBlock, Block* elseBlock) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StatementIf* statement = calloc(1, sizeof(StatementIf));
    statement->condition = condition;
    statement->thenBlock = thenBlock;
    statement->elseBlock = elseBlock;
    statement->hasElse = 1;
    return statement;
}

StatementWhile* StatementWhileSemanticAction(Expression* condition, Block* block) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StatementWhile* statement = calloc(1, sizeof(StatementWhile));
    statement->condition = condition;
    statement->block = block;
    return statement;
}

StatementFor* StatementForSemanticAction(Expression* init, Expression* condition, Expression* update, Block* block) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StatementFor* statement = calloc(1, sizeof(StatementFor));
    statement->init = init;
    statement->hasInit = (init != NULL) ? 1 : 0;
    statement->condition = condition;
    statement->hasCondition = (condition != NULL) ? 1 : 0;
    statement->update = update;
    statement->hasUpdate = (update != NULL) ? 1 : 0;
    statement->block = block;
    return statement;
}

StatementReturn* StatementReturnSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    StatementReturn* statement = calloc(1, sizeof(StatementReturn));
    statement->expression = expression;
    statement->hasExpression = (expression != NULL) ? 1 : 0;
    return statement;
}

// Expression actions
Expression* AssignmentExpressionSemanticAction(Expression* lvalue, Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = lvalue;
    result->rightExpression = expression;
    result->type = EXPRESSION_ASSIGNMENT;
    return result;
}

Expression* OrExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_OR;
    return result;
}

Expression* AndExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_AND;
    return result;
}

Expression* EqualExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_EQUAL;
    return result;
}

Expression* NotEqualExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_NOT_EQUAL;
    return result;
}

Expression* LessExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_LESS;
    return result;
}

Expression* GreaterExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_GREATER;
    return result;
}

Expression* LessEqualExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_LESS_EQUAL;
    return result;
}

Expression* GreaterEqualExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_GREATER_EQUAL;
    return result;
}

Expression* AdditionExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_ADDITION;
    return result;
}

Expression* SubtractionExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_SUBTRACTION;
    return result;
}

Expression* MultiplicationExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_MULTIPLICATION;
    return result;
}

Expression* DivisionExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_DIVISION;
    return result;
}

Expression* ModuloExpressionSemanticAction(Expression* left, Expression* right) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->leftExpression = left;
    result->rightExpression = right;
    result->type = EXPRESSION_MODULO;
    return result;
}

Expression* NotExpressionSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->singleExpression = expression;
    result->type = EXPRESSION_NOT;
    return result;
}

Expression* IdentifierExpressionSemanticAction(Identifier* identifier, IdentifierSuffix* suffix) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    
    if (suffix->type == IDENTIFIER_SUFFIX_NONE) {
        result->identifier = identifier;
        result->type = EXPRESSION_IDENTIFIER;
    } else if (suffix->type == IDENTIFIER_SUFFIX_FUNCTION_CALL) {
        result->identifierFunc = identifier;
        result->arguments = suffix->arguments;
        result->type = EXPRESSION_FUNCTION_CALL;
    } else if (suffix->type == IDENTIFIER_SUFFIX_ARRAY_ACCESS) {
        result->identifierArray = identifier;
        result->indexExpression = suffix->indexExpression;
        result->type = EXPRESSION_ARRAY_ACCESS;
    }
    
    return result;
}

Expression* ConstantExpressionSemanticAction(Constant* constant) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->constant = constant;
    result->type = EXPRESSION_CONSTANT;
    return result;
}

Expression* ParenthesisExpressionSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->singleExpression = expression;
    result->type = EXPRESSION_PARENTHESIS;
    return result;
}

Expression* SimpleExpressionLvalueSemanticAction(Identifier* identifier) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->identifier = identifier;
    result->type = EXPRESSION_IDENTIFIER;
    return result;
}

Expression* ArrayExpressionLvalueSemanticAction(Identifier* identifier, Expression* index) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->identifierArray = identifier;
    result->indexExpression = index;
    result->type = EXPRESSION_ARRAY_ACCESS;
    return result;
}

IdentifierSuffix* FunctionCallIdentifierSuffixSemanticAction(ListArguments* arguments) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    IdentifierSuffix* suffix = calloc(1, sizeof(IdentifierSuffix));
    suffix->arguments = arguments;
    suffix->type = IDENTIFIER_SUFFIX_FUNCTION_CALL;
    return suffix;
}

IdentifierSuffix* ArrayAccessIdentifierSuffixSemanticAction(Expression* index) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    IdentifierSuffix* suffix = calloc(1, sizeof(IdentifierSuffix));
    suffix->indexExpression = index;
    suffix->type = IDENTIFIER_SUFFIX_ARRAY_ACCESS;
    return suffix;
}

IdentifierSuffix* EmptyIdentifierSuffixSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    IdentifierSuffix* suffix = calloc(1, sizeof(IdentifierSuffix));
    suffix->type = IDENTIFIER_SUFFIX_NONE;
    return suffix;
}

ListArguments* SingleListArgumentsSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ListArguments* list = calloc(1, sizeof(ListArguments));
    list->expression = expression;
    list->next = NULL;
    return list;
}

ListArguments* AppendListArgumentsSemanticAction(ListArguments* list, Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ListArguments* current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    
    ListArguments* newArg = calloc(1, sizeof(ListArguments));
    newArg->expression = expression;
    newArg->next = NULL;
    
    current->next = newArg;
    
    return list;
}

Constant* IntegerConstantSemanticAction(ConstantInteger* constant) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Constant* result = calloc(1, sizeof(Constant));
    result->integer = constant;
    result->type = 0;
    return result;
}

Constant* CharacterConstantSemanticAction(ConstantCharacter* constant) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Constant* result = calloc(1, sizeof(Constant));
    result->character = constant;
    result->type = 1;
    return result;
}

ConstantInteger* ConstantIntegerSemanticAction(int value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ConstantInteger* constant = calloc(1, sizeof(ConstantInteger));
    *constant = value;
    return constant;
}

ConstantCharacter* ConstantCharacterSemanticAction(char value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ConstantCharacter* constant = calloc(1, sizeof(ConstantCharacter));
    *constant = value;
    return constant;
}

// Identifier action
Identifier* IdentifierSemanticAction(char* name) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Identifier* identifier = calloc(1, sizeof(Identifier));
    *identifier = strdup(name);
    return identifier;
}