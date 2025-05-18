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

void releaseBloque(Bloque* bloque) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (bloque != NULL) {
        releaseSentencias(bloque->sentencias);
        free(bloque);
    }
}

void releaseConstante(Constante* constante) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (constante != NULL) {
        if (constante->type == 0) {
            releaseConstanteEntera(constante->entera);
        } else {
            releaseConstanteCaracter(constante->caracter);
        }
        free(constante);
    }
}

void releaseConstanteCaracter(ConstanteCaracter* constanteCaracter) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (constanteCaracter != NULL) {
        free(constanteCaracter);
    }
}

void releaseConstanteEntera(ConstanteEntera* constanteEntera) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (constanteEntera != NULL) {
        free(constanteEntera);
    }
}

void releaseDeclaracion(Declaracion* declaracion) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (declaracion != NULL) {
        releaseIdentificador(declaracion->identifier);
        releaseDeclaracionSufijo(declaracion->declaracionSufijo);
        free(declaracion);
    }
}

void releaseDeclaracionLista(DeclaracionLista* declaracionLista) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (declaracionLista != NULL) {
        releaseDeclaracion(declaracionLista->declaracion);
        releaseDeclaracionLista(declaracionLista->next);
        free(declaracionLista);
    }
}

void releaseDeclaracionSufijo(DeclaracionSufijo* declaracionSufijo) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (declaracionSufijo != NULL) {
        if (declaracionSufijo->type == DECLARACION_SUFIJO_VARIABLE) {
            releaseVariableSufijo(declaracionSufijo->variableSufijo);
        } else {
            releaseParametros(declaracionSufijo->parametros);
            releaseFuncionSufijo(declaracionSufijo->funcionSufijo);
        }
        free(declaracionSufijo);
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
                releaseIdentificador(expression->identifier);
                break;
            case EXPRESSION_CONSTANT:
                releaseConstante(expression->constant);
                break;
            case EXPRESSION_ARRAY_ACCESS:
                releaseIdentificador(expression->identifierArray);
                releaseExpression(expression->indexExpression);
                break;
            case EXPRESSION_FUNCTION_CALL:
                releaseIdentificador(expression->identifierFunc);
                releaseListaArgumentos(expression->arguments);
                break;
        }
        free(expression);
    }
}

void releaseFuncionSufijo(FuncionSufijo* funcionSufijo) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (funcionSufijo != NULL) {
        if (funcionSufijo->type == 1) {
            releaseBloque(funcionSufijo->bloque);
        }
        free(funcionSufijo);
    }
}

void releaseIdentificador(Identificador* identificador) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (identificador != NULL) {
        free(identificador->name);
        free(identificador);
    }
}

void releaseIdentificadorSufijo(IdentificadorSufijo* identificadorSufijo) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (identificadorSufijo != NULL) {
        if (identificadorSufijo->type == IDENTIFICADOR_SUFIJO_FUNCTION_CALL) {
            releaseListaArgumentos(identificadorSufijo->arguments);
        } else if (identificadorSufijo->type == IDENTIFICADOR_SUFIJO_ARRAY_ACCESS) {
            releaseExpression(identificadorSufijo->indexExpression);
        }
        free(identificadorSufijo);
    }
}

void releaseListaArgumentos(ListaArgumentos* listaArgumentos) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (listaArgumentos != NULL) {
        releaseExpression(listaArgumentos->expression);
        releaseListaArgumentos(listaArgumentos->next);
        free(listaArgumentos);
    }
}

void releaseParametro(Parametro* parametro) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (parametro != NULL) {
        releaseIdentificador(parametro->identifier);
        releaseParametroArray(parametro->array);
        free(parametro);
    }
}

void releaseParametroArray(ParametroArray* parametroArray) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (parametroArray != NULL) {
        free(parametroArray);
    }
}

void releaseParametroLista(ParametroLista* parametroLista) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (parametroLista != NULL) {
        releaseParametro(parametroLista->parametro);
        releaseParametroLista(parametroLista->next);
        free(parametroLista);
    }
}

void releaseParametros(Parametros* parametros) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (parametros != NULL) {
        if (parametros->type == 1) {
            releaseParametroLista(parametros->lista);
        }
        free(parametros);
    }
}

void releaseProgram(Program* program) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (program != NULL) {
        if (program->type == PROGRAM_DECLARATIONS) {
            releaseDeclaracionLista(program->declaracionLista);
        }
        free(program);
    }
}

void releaseSentencia(Sentencia* sentencia) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (sentencia != NULL) {
        switch (sentencia->type) {
            case SENTENCIA_DECLARATION:
                releaseIdentificador(sentencia->identifier);
                releaseVariableSufijo(sentencia->variableSufijo);
                break;
            case SENTENCIA_IF:
                releaseSentenciaIf(sentencia->sentenciaIf);
                break;
            case SENTENCIA_WHILE:
                releaseSentenciaWhile(sentencia->sentenciaWhile);
                break;
            case SENTENCIA_FOR:
                releaseSentenciaFor(sentencia->sentenciaFor);
                break;
            case SENTENCIA_RETURN:
                releaseSentenciaReturn(sentencia->sentenciaReturn);
                break;
            case SENTENCIA_EXPRESSION:
                releaseSentenciaExpresion(sentencia->sentenciaExpresion);
                break;
            case SENTENCIA_BLOQUE:
                releaseBloque(sentencia->bloque);
                break;
            case SENTENCIA_EMPTY:
                break;
        }
        free(sentencia);
    }
}

void releaseSentencias(Sentencias* sentencias) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (sentencias != NULL) {
        releaseSentencia(sentencias->sentencia);
        releaseSentencias(sentencias->next);
        free(sentencias);
    }
}

void releaseSentenciaExpresion(SentenciaExpresion* sentenciaExpresion) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (sentenciaExpresion != NULL) {
        releaseExpression(sentenciaExpresion->expression);
        free(sentenciaExpresion);
    }
}

void releaseSentenciaFor(SentenciaFor* sentenciaFor) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (sentenciaFor != NULL) {
        if (sentenciaFor->hasInit) {
            releaseExpression(sentenciaFor->init);
        }
        if (sentenciaFor->hasCondition) {
            releaseExpression(sentenciaFor->condition);
        }
        if (sentenciaFor->hasUpdate) {
            releaseExpression(sentenciaFor->update);
        }
        releaseBloque(sentenciaFor->bloque);
        free(sentenciaFor);
    }
}

void releaseSentenciaIf(SentenciaIf* sentenciaIf) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (sentenciaIf != NULL) {
        releaseExpression(sentenciaIf->condition);
        releaseBloque(sentenciaIf->thenBloque);
        if (sentenciaIf->hasElse) {
            releaseBloque(sentenciaIf->elseBloque);
        }
        free(sentenciaIf);
    }
}

void releaseSentenciaReturn(SentenciaReturn* sentenciaReturn) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (sentenciaReturn != NULL) {
        if (sentenciaReturn->hasExpression) {
            releaseExpression(sentenciaReturn->expression);
        }
        free(sentenciaReturn);
    }
}

void releaseSentenciaWhile(SentenciaWhile* sentenciaWhile) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (sentenciaWhile != NULL) {
        releaseExpression(sentenciaWhile->condition);
        releaseBloque(sentenciaWhile->bloque);
        free(sentenciaWhile);
    }
}

void releaseVariableSufijo(VariableSufijo* variableSufijo) {
    logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    if (variableSufijo != NULL) {
        if (variableSufijo->type == VARIABLE_SUFIJO_ASSIGNMENT) {
            releaseExpression(variableSufijo->expression);
        } else if (variableSufijo->type == VARIABLE_SUFIJO_ARRAY) {
            releaseConstanteEntera(variableSufijo->arraySize);
        }
        free(variableSufijo);
    }
}