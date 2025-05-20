#include "AbstractSyntaxTree.h"
#include <stdlib.h>

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


void freeProgram(Program* node) {
    if (node == NULL) return;
    if (node->type == PROGRAM_DECLARATIONS) freeDeclarationList(node->declarationList);
    free(node);
}

void freeBlock(Block* node) {
    if (node == NULL) return;
    freeStatements(node->statements);
    free(node);
}

void freeStatements(Statements* node) {
    if (node == NULL) return;
    freeStatement(node->statement);
    freeStatements(node->next);
    free(node);
}

void freeStatement(Statement* node) {
    if (node == NULL) return;
    switch (node->type) {
        case STATEMENT_DECLARATION:
            free(node->identifier);
            freeVariableSuffix(node->variableSuffix);
            break;
        case STATEMENT_IF:
            freeStatementIf(node->statementIf);
            break;
        case STATEMENT_WHILE:
            freeStatementWhile(node->statementWhile);
            break;
        case STATEMENT_FOR:
            freeStatementFor(node->statementFor);
            break;
        case STATEMENT_RETURN:
            freeStatementReturn(node->statementReturn);
            break;
        case STATEMENT_EXPRESSION:
            freeStatementExpression(node->statementExpression);
            break;
        case STATEMENT_BLOCK:
            freeBlock(node->block);
            break;
        case STATEMENT_EMPTY:
            break;
    }
    free(node);
}

void freeStatementReturn(StatementReturn* node) {
    if (node == NULL) return;
    if (node->hasExpression) freeExpression(node->expression);
    free(node);
}

void freeStatementFor(StatementFor* node) {
    if (node == NULL) return;
    if (node->hasInit) freeExpression(node->init);
    if (node->hasCondition) freeExpression(node->condition);
    if (node->hasUpdate) freeExpression(node->update);
    freeBlock(node->block);
    free(node);
}

void freeStatementWhile(StatementWhile* node) {
    if (node == NULL) return;
    freeExpression(node->condition);
    freeBlock(node->block);
    free(node);
}

void freeStatementIf(StatementIf* node) {
    if (node == NULL) return;
    freeExpression(node->condition);
    freeBlock(node->thenBlock);
    if (node->hasElse) freeBlock(node->elseBlock);
    free(node);
}

void freeStatementExpression(StatementExpression* node) {
    if (node == NULL) return;
    freeExpression(node->expression);
    free(node);
}

void freeDeclarationList(DeclarationList* node) {
    if (node == NULL) return;
    freeDeclaration(node->declaration);
    freeDeclarationList(node->next);
    free(node);
}

void freeDeclaration(Declaration* node) {
    if (node == NULL) return;
    free(node->identifier);
    freeDeclarationSuffix(node->declarationSuffix);
    free(node);
}

void freeDeclarationSuffix(DeclarationSuffix* node) {
    if (node == NULL) return;
    switch (node->type) {
        case DECLARATION_SUFFIX_VARIABLE:
            freeVariableSuffix(node->variableSuffix);
            break;
        case DECLARATION_SUFFIX_FUNCTION:
            freeParameters(node->parameters);
            freeFunctionSuffix(node->functionSuffix);
            break;
    }
    free(node);
}

void freeFunctionSuffix(FunctionSuffix* node) {
    if (node == NULL) return;
    freeBlock(node->block);
    free(node);
}

void freeParameters(Parameters* node) {
    if (node == NULL) return;
    freeParameterList(node->list);
    free(node);
}

void freeParameterList(ParameterList* node) {
    if (node == NULL) return;
    freeParameter(node->parameter);
    freeParameterList(node->next);
    free(node);
}

void freeParameter(Parameter* node) {
    if (node == NULL) return;
    free(node->identifier);
    freeParameterArray(node->array);
    free(node);
}

void freeParameterArray(ParameterArray* node) {
    if (node == NULL) return;
    free(node);
}

void freeVariableSuffix(VariableSuffix* node) {
    if (node == NULL) return;
    switch (node->type) {
        case VARIABLE_SUFFIX_ASSIGNMENT:
            freeExpression(node->expression);
            break;
        case VARIABLE_SUFFIX_ARRAY:
            free(node->arraySize);
            break;
        default: break;
    }
    free(node);
}

void freeIdentifierSuffix(IdentifierSuffix* node) {
    if (node == NULL) return;
    switch (node->type) {
        case IDENTIFIER_SUFFIX_FUNCTION_CALL:
            freeListArguments(node->arguments);
            break;
        case IDENTIFIER_SUFFIX_ARRAY_ACCESS:
            freeExpression(node->indexExpression);
            break;
        default: break;
    }
    free(node);
}

void freeListArguments(ListArguments* node) {
    if (node == NULL) return;
    freeExpression(node->expression);
    freeListArguments(node->next);
    free(node);
}

void freeExpression(Expression* node) {
    if (node == NULL) return;
    switch (node->type) {
        case EXPRESSION_ASSIGNMENT: case EXPRESSION_OR: case EXPRESSION_AND:
        case EXPRESSION_EQUAL: case EXPRESSION_NOT_EQUAL: case EXPRESSION_LESS:
        case EXPRESSION_GREATER: case EXPRESSION_LESS_EQUAL: case EXPRESSION_GREATER_EQUAL:
        case EXPRESSION_ADDITION: case EXPRESSION_SUBTRACTION: case EXPRESSION_MULTIPLICATION:
        case EXPRESSION_DIVISION: case EXPRESSION_MODULO:
            freeExpression(node->leftExpression);
            freeExpression(node->rightExpression);
            break;
        case EXPRESSION_NOT: case EXPRESSION_PARENTHESIS:
            freeExpression(node->singleExpression);
            break;
        case EXPRESSION_IDENTIFIER:
            free(node->identifier);
            break;
        case EXPRESSION_CONSTANT:
            freeConstant(node->constant);
            break;
        case EXPRESSION_ARRAY_ACCESS:
            free(node->identifierArray);
            freeExpression(node->indexExpression);
            break;
        case EXPRESSION_FUNCTION_CALL:
            free(node->identifierFunc);
            freeListArguments(node->arguments);
            break;
    }
    free(node);
}

void freeConstant(Constant* node) {
    if (node == NULL) return;
    if (node->type == TYPE_INT) free(node->integer);
    else free(node->character);
    free(node);
}

void freeIdentifier(Identifier* identifier) {
    if (identifier == NULL) return;
    free(*identifier);
    free(identifier);
}