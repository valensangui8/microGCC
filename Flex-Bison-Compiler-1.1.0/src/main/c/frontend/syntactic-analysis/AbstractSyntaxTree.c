#include "AbstractSyntaxTree.h"

static Logger* _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
    _logger = createLogger("AbstractSyntaxTree");
}

void shutdownAbstractSyntaxTreeModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

/* --- Release Functions --- */

void releaseConstantInteger(ConstantInteger* ci) {
    logDebugging(_logger, "Freeing: ConstantInteger");
    free(ci);
}

void releaseConstantCharacter(ConstantCharacter* cc) {
    logDebugging(_logger, "Freeing: ConstantCharacter");
    free(cc);
}

void releaseConstant(Constant* c) {
    if (!c) return;
    logDebugging(_logger, "Freeing: Constant");

    if (c->type == 0)
        releaseConstantInteger(c->integer);
    else
        releaseConstantCharacter(c->character);

    free(c);
}

void releaseIdentifier(Identifier* id) {
    if (!id) return;
    logDebugging(_logger, "Freeing: Identifier");
    
    free(id);
}

void releaseExpression(Expression* expr) {
    if (!expr) return;
    logDebugging(_logger, "Freeing: Expression");

    switch (expr->type) {
        case EXPRESSION_ASSIGNMENT:
        case EXPRESSION_OR:
        case EXPRESSION_AND:
        case EXPRESSION_EQUAL:
        case EXPRESSION_NOT_EQUAL:
        case EXPRESSION_LESS:
        case EXPRESSION_GREATER:
        case EXPRESSION_LESS_EQUAL:
        case EXPRESSION_GREATER_EQUAL:
        case EXPRESSION_ADDITION:
        case EXPRESSION_SUBTRACTION:
        case EXPRESSION_MULTIPLICATION:
        case EXPRESSION_DIVISION:
        case EXPRESSION_MODULO:
            releaseExpression(expr->leftExpression);
            releaseExpression(expr->rightExpression);
            break;
        case EXPRESSION_NOT:
        case EXPRESSION_PARENTHESIS:
            releaseExpression(expr->singleExpression);
            break;
        case EXPRESSION_IDENTIFIER:
            releaseIdentifier(expr->identifier);
            break;
        case EXPRESSION_CONSTANT:
            releaseConstant(expr->constant);
            break;
        case EXPRESSION_ARRAY_ACCESS:
            releaseIdentifier(expr->identifierArray);
            releaseExpression(expr->indexExpression);
            break;
        case EXPRESSION_FUNCTION_CALL:
            releaseIdentifier(expr->identifierFunc);
            releaseListArguments(expr->arguments);
            break;
    }

    free(expr);
}

void releaseListArguments(ListArguments* list) {
    while (list) {
        ListArguments* next = list->next;
        releaseExpression(list->expression);
        free(list);
        list = next;
    }
}

void releaseIdentifierSuffix(IdentifierSuffix* suffix) {
    if (!suffix) return;
    logDebugging(_logger, "Freeing: IdentifierSuffix");

    if (suffix->type == IDENTIFIER_SUFFIX_FUNCTION_CALL) {
        releaseListArguments(suffix->arguments);
    } else if (suffix->type == IDENTIFIER_SUFFIX_ARRAY_ACCESS) {
        releaseExpression(suffix->indexExpression);
    }

    free(suffix);
}

void releaseVariableSuffix(VariableSuffix* suffix) {
    if (!suffix) return;
    logDebugging(_logger, "Freeing: VariableSuffix");

    if (suffix->type == VARIABLE_SUFFIX_ASSIGNMENT)
        releaseExpression(suffix->expression);
    else if (suffix->type == VARIABLE_SUFFIX_ARRAY)
        releaseConstantInteger(suffix->arraySize);

    free(suffix);
}

void releaseParameterArray(ParameterArray* array) {
    logDebugging(_logger, "Freeing: ParameterArray");
    free(array);
}

void releaseParameter(Parameter* p) {
    if (!p) return;
    logDebugging(_logger, "Freeing: Parameter");
    releaseIdentifier(p->identifier);
    releaseParameterArray(p->array);
    free(p);
}

void releaseParameterList(ParameterList* list) {
    while (list) {
        ParameterList* next = list->next;
        releaseParameter(list->parameter);
        free(list);
        list = next;
    }
}

void releaseParameters(Parameters* params) {
    if (!params) return;
    logDebugging(_logger, "Freeing: Parameters");

    if (params->type == 1)
        releaseParameterList(params->list);

    free(params);
}

void releaseFunctionSuffix(FunctionSuffix* suffix) {
    if (!suffix) return;
    logDebugging(_logger, "Freeing: FunctionSuffix");

    if (suffix->type == 1)
        releaseBlock(suffix->block);

    free(suffix);
}

void releaseDeclarationSuffix(DeclarationSuffix* suffix) {
    if (!suffix) return;
    logDebugging(_logger, "Freeing: DeclarationSuffix");

    if (suffix->type == DECLARATION_SUFFIX_VARIABLE)
        releaseVariableSuffix(suffix->variableSuffix);
    else {
        releaseParameters(suffix->parameters);
        releaseFunctionSuffix(suffix->functionSuffix);
    }

    free(suffix);
}

void releaseDeclaration(Declaration* d) {
    if (!d) return;
    logDebugging(_logger, "Freeing: Declaration");
    releaseIdentifier(d->identifier);
    releaseDeclarationSuffix(d->declarationSuffix);
    free(d);
}

void releaseDeclarationList(DeclarationList* list) {
    while (list) {
        DeclarationList* next = list->next;
        releaseDeclaration(list->declaration);
        free(list);
        list = next;
    }
}

void releaseStatementExpression(StatementExpression* stmt) {
    if (!stmt) return;
    releaseExpression(stmt->expression);
    free(stmt);
}

void releaseStatementIf(StatementIf* stmt) {
    if (!stmt) return;
    releaseExpression(stmt->condition);
    releaseBlock(stmt->thenBlock);
    if (stmt->hasElse)
        releaseBlock(stmt->elseBlock);
    free(stmt);
}

void releaseStatementWhile(StatementWhile* stmt) {
    if (!stmt) return;
    releaseExpression(stmt->condition);
    releaseBlock(stmt->block);
    free(stmt);
}

void releaseStatementFor(StatementFor* stmt) {
    if (!stmt) return;
    if (stmt->hasInit) releaseExpression(stmt->init);
    if (stmt->hasCondition) releaseExpression(stmt->condition);
    if (stmt->hasUpdate) releaseExpression(stmt->update);
    releaseBlock(stmt->block);
    free(stmt);
}

void releaseStatementReturn(StatementReturn* stmt) {
    if (!stmt) return;
    if (stmt->hasExpression)
        releaseExpression(stmt->expression);
    free(stmt);
}

void releaseStatement(Statement* stmt) {
    if (!stmt) return;
    logDebugging(_logger, "Freeing: Statement");

    switch (stmt->type) {
        case STATEMENT_DECLARATION:
            (void)stmt->dataType;
            releaseIdentifier(stmt->identifier);
            releaseVariableSuffix(stmt->variableSuffix);
    
            break;
        case STATEMENT_IF:
            releaseStatementIf(stmt->statementIf);
            break;
        case STATEMENT_WHILE:
            releaseStatementWhile(stmt->statementWhile);
            break;
        case STATEMENT_FOR:
            releaseStatementFor(stmt->statementFor);
            break;
        case STATEMENT_RETURN:
            releaseStatementReturn(stmt->statementReturn);
            break;
        case STATEMENT_EXPRESSION:
            releaseStatementExpression(stmt->statementExpression);
            break;
        case STATEMENT_BLOCK:
            releaseBlock(stmt->block);
            break;
        case STATEMENT_EMPTY:
            break;
    }

    free(stmt);
}

void releaseStatements(Statements* list) {
    while (list) {
        Statements* next = list->next;
        releaseStatement(list->statement);
        free(list);
        list = next;
    }
}

void releaseBlock(Block* block) {
    if (!block) return;
    logDebugging(_logger, "Freeing: Block");
    releaseStatements(block->statements);
    free(block);
}

void releaseProgram(Program* program) {
    if (!program) return;
    logDebugging(_logger, "Freeing: Program");

    if (program->type == PROGRAM_DECLARATIONS)
        releaseDeclarationList(program->declarationList);

    free(program);
}
