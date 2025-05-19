#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
    _logger = createLogger("AbstractSyntaxTree");
}

void shutdownAbstractSyntaxTreeModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

/** PUBLIC FUNCTIONS */

void releaseBlock(Block* block) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (block != NULL) {
        releaseStatements(block->statements);
        free(block);
    }
}

void releaseConstant(Constant* constant) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (constant != NULL) {
        if (constant->type == 0) {
            releaseConstantInteger(constant->integer);
        } else {
            releaseConstantCharacter(constant->character);
        }
        free(constant);
    }
}

void releaseConstantCharacter(ConstantCharacter* constantCharacter) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (constantCharacter != NULL) {
        free(constantCharacter);
    }
}

void releaseConstantInteger(ConstantInteger* constantInteger) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (constantInteger != NULL) {
        free(constantInteger);
    }
}

void releaseDeclaration(Declaration* declaration) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (declaration != NULL) {
        releaseIdentifier(declaration->identifier);
        releaseDeclarationSuffix(declaration->declarationSuffix);
        free(declaration);
    }
}

void releaseDeclarationList(DeclarationList* declarationList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (declarationList != NULL) {
        releaseDeclaration(declarationList->declaration);
        releaseDeclarationList(declarationList->next);
        free(declarationList);
    }
}

void releaseDeclarationSuffix(DeclarationSuffix* declarationSuffix) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (declarationSuffix != NULL) {
        if (declarationSuffix->type == VARIABLE_SUFFIX_DECLARATION) {
            releaseVariableSuffix(declarationSuffix->variableSuffix);
        } else {
            releaseParameters(declarationSuffix->parameters);
            releaseFunctionSuffix(declarationSuffix->functionSuffix);
        }
        free(declarationSuffix);
    }
}

void releaseExpression(Expression* expression) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (expression != NULL) {
        switch (expression->type) {
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
                releaseExpression(expression->leftExpression);
                releaseExpression(expression->rightExpression);
                break;
            case EXPRESSION_NOT:
            case EXPRESSION_PARENTHESIS:
                releaseExpression(expression->singleExpression);
                break;
            case EXPRESSION_IDENTIFIER:
                releaseIdentifier(expression->identifier);
                break;
            case EXPRESSION_CONSTANT:
                releaseConstant(expression->constant);
                break;
            case EXPRESSION_ARRAY_ACCESS:
                releaseIdentifier(expression->identifierArray);
                releaseExpression(expression->indexExpression);
                break;
            case EXPRESSION_FUNCTION_CALL:
                releaseIdentifier(expression->identifierFunc);
                releaseListArguments(expression->arguments);
                break;
        }
        free(expression);
    }
}

void releaseFunctionSuffix(FunctionSuffix* functionSuffix) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (functionSuffix != NULL) {
        if (functionSuffix->type == 1) {
            releaseBlock(functionSuffix->block);
        }
        free(functionSuffix);
    }
}

void releaseIdentifier(Identifier* identifier) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (identifier != NULL) {
        free(identifier->name);
        free(identifier);
    }
}

void releaseIdentifierSuffix(IdentifierSuffix* identifierSuffix) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (identifierSuffix != NULL) {
        if (identifierSuffix->type == IDENTIFIER_SUFFIX_FUNCTION_CALL) {
            releaseListArguments(identifierSuffix->arguments);
        } else if (identifierSuffix->type == IDENTIFIER_SUFFIX_ARRAY_ACCESS) {
            releaseExpression(identifierSuffix->indexExpression);
        }
        free(identifierSuffix);
    }
}

void releaseListArguments(ListArguments* listArguments) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (listArguments != NULL) {
        releaseExpression(listArguments->expression);
        releaseListArguments(listArguments->next);
        free(listArguments);
    }
}

void releaseParameter(Parameter* parameter) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (parameter != NULL) {
        releaseIdentifier(parameter->identifier);
        releaseParameterArray(parameter->array);
        free(parameter);
    }
}

void releaseParameterArray(ParameterArray* parameterArray) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (parameterArray != NULL) {
        free(parameterArray);
    }
}

void releaseParameterList(ParameterList* parameterList) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (parameterList != NULL) {
        releaseParameter(parameterList->parameter);
        releaseParameterList(parameterList->next);
        free(parameterList);
    }
}

void releaseParameters(Parameters* parameters) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (parameters != NULL) {
        if (parameters->type == 1) {
            releaseParameterList(parameters->list);
        }
        free(parameters);
    }
}

void releaseProgram(Program* program) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (program != NULL) {
        if (program->type == PROGRAM_DECLARATIONS) {
            releaseDeclarationList(program->declarationList);
        }
        free(program);
    }
}

void releaseStatement(Statement* statement) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statement != NULL) {
        switch (statement->type) {
            case STATEMENT_DECLARATION:
                releaseIdentifier(statement->identifier);
                releaseVariableSuffix(statement->variableSuffix);
                break;
            case STATEMENT_IF:
                releaseStatementIf(statement->statementIf);
                break;
            case STATEMENT_WHILE:
                releaseStatementWhile(statement->statementWhile);
                break;
            case STATEMENT_FOR:
                releaseStatementFor(statement->statementFor);
                break;
            case STATEMENT_RETURN:
                releaseStatementReturn(statement->statementReturn);
                break;
            case STATEMENT_EXPRESSION:
                releaseStatementExpression(statement->statementExpression);
                break;
            case STATEMENT_BLOCK:
                releaseBlock(statement->block);
                break;
            case STATEMENT_EMPTY:
                break;
        }
        free(statement);
    }
}

void releaseStatements(Statements* statements) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statements != NULL) {
        releaseStatement(statements->statement);
        releaseStatements(statements->next);
        free(statements);
    }
}

void releaseStatementExpression(StatementExpression* statementExpression) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statementExpression != NULL) {
        releaseExpression(statementExpression->expression);
        free(statementExpression);
    }
}

void releaseStatementFor(StatementFor* statementFor) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statementFor != NULL) {
        if (statementFor->hasInit) {
            releaseExpression(statementFor->init);
        }
        if (statementFor->hasCondition) {
            releaseExpression(statementFor->condition);
        }
        if (statementFor->hasUpdate) {
            releaseExpression(statementFor->update);
        }
        releaseBlock(statementFor->block);
        free(statementFor);
    }
}

void releaseStatementIf(StatementIf* statementIf) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statementIf != NULL) {
        releaseExpression(statementIf->condition);
        releaseBlock(statementIf->thenBlock);
        if (statementIf->hasElse) {
            releaseBlock(statementIf->elseBlock);
        }
        free(statementIf);
    }
}

void releaseStatementReturn(StatementReturn* statementReturn) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statementReturn != NULL) {
        if (statementReturn->hasExpression) {
            releaseExpression(statementReturn->expression);
        }
        free(statementReturn);
    }
}

void releaseStatementWhile(StatementWhile* statementWhile) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (statementWhile != NULL) {
        releaseExpression(statementWhile->condition);
        releaseBlock(statementWhile->block);
        free(statementWhile);
    }
}

void releaseVariableSuffix(VariableSuffix* variableSuffix) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (variableSuffix != NULL) {
        if (variableSuffix->type == VARIABLE_SUFFIX_ASSIGNMENT) {
            releaseExpression(variableSuffix->expression);
        } else if (variableSuffix->type == VARIABLE_SUFFIX_ARRAY) {
            releaseConstantInteger(variableSuffix->arraySize);
        }
        free(variableSuffix);
    }
}