#include "BisonActions.h"

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
Program* DeclaracionListaProgramSemanticAction(CompilerState* compilerState, DeclaracionLista* declaracionLista) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Program* program = calloc(1, sizeof(Program));
    program->declaracionLista = declaracionLista;
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
DeclaracionLista* SingleDeclarationListSemanticAction(Declaracion* declaracion) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DeclaracionLista* lista = calloc(1, sizeof(DeclaracionLista));
    lista->declaracion = declaracion;
    lista->next = NULL;
    return lista;
}

DeclaracionLista* AppendDeclarationListSemanticAction(DeclaracionLista* lista, Declaracion* declaracion) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DeclaracionLista* newLista = calloc(1, sizeof(DeclaracionLista));
    newLista->declaracion = declaracion;
    newLista->next = NULL;
    
    DeclaracionLista* current = lista;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = newLista;
    
    return lista;
}

Declaracion* RegularDeclarationSemanticAction(DataType type, Identificador* identificador, DeclaracionSufijo* declaracionSufijo) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Declaracion* declaracion = calloc(1, sizeof(Declaracion));
    declaracion->dataType = type;  // No conditional check needed
    declaracion->identifier = identificador;
    declaracion->declaracionSufijo = declaracionSufijo;
    declaracion->declarationType = DECLARATION_REGULAR;
    return declaracion;
}

Declaracion* ExternDeclarationSemanticAction(DataType type, Identificador* identificador, DeclaracionSufijo* declaracionSufijo) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Declaracion* declaracion = calloc(1, sizeof(Declaracion));
    declaracion->dataType = type;
    declaracion->identifier = identificador;
    declaracion->declaracionSufijo = declaracionSufijo;
    declaracion->declarationType = DECLARATION_EXTERN;
    return declaracion;
}

DeclaracionSufijo* VariableDeclaracionSufijoSemanticAction(VariableSufijo* variableSufijo) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DeclaracionSufijo* sufijo = calloc(1, sizeof(DeclaracionSufijo));
    sufijo->variableSufijo = variableSufijo;
    sufijo->type = DECLARACION_SUFIJO_VARIABLE;
    return sufijo;
}

DeclaracionSufijo* FunctionDeclaracionSufijoSemanticAction(Parametros* parametros, FuncionSufijo* funcionSufijo) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    DeclaracionSufijo* sufijo = calloc(1, sizeof(DeclaracionSufijo));
    sufijo->parametros = parametros;
    sufijo->funcionSufijo = funcionSufijo;
    sufijo->type = DECLARACION_SUFIJO_FUNCTION;
    return sufijo;
}

FuncionSufijo* EmptyFuncionSufijoSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    FuncionSufijo* sufijo = calloc(1, sizeof(FuncionSufijo));
    sufijo->type = 0;
    return sufijo;
}

FuncionSufijo* BlockFuncionSufijoSemanticAction(Bloque* bloque) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    FuncionSufijo* sufijo = calloc(1, sizeof(FuncionSufijo));
    sufijo->bloque = bloque;
    sufijo->type = 1;
    return sufijo;
}

VariableSufijo* EmptyVariableSufijoSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    VariableSufijo* sufijo = calloc(1, sizeof(VariableSufijo));
    sufijo->type = VARIABLE_SUFIJO_NONE;
    return sufijo;
}

VariableSufijo* AssignmentVariableSufijoSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    VariableSufijo* sufijo = calloc(1, sizeof(VariableSufijo));
    sufijo->expression = expression;
    sufijo->type = VARIABLE_SUFIJO_ASSIGNMENT;
    return sufijo;
}

VariableSufijo* ArrayVariableSufijoSemanticAction(int size) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    VariableSufijo* sufijo = calloc(1, sizeof(VariableSufijo));
    ConstanteEntera* arraySize = calloc(1, sizeof(ConstanteEntera));
    arraySize->value = size;
    sufijo->arraySize = arraySize;
    sufijo->type = VARIABLE_SUFIJO_ARRAY;
    return sufijo;
}

// Parameter actions
Parametros* VoidParametrosSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Parametros* parametros = calloc(1, sizeof(Parametros));
    parametros->type = 0;
    return parametros;
}

Parametros* ListParametrosSemanticAction(ParametroLista* lista) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Parametros* parametros = calloc(1, sizeof(Parametros));
    parametros->lista = lista;
    parametros->type = 1;
    return parametros;
}

Parametros* EmptyParametrosSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Parametros* parametros = calloc(1, sizeof(Parametros));
    parametros->type = 2;
    return parametros;
}

ParametroLista* SingleParametroListaSemanticAction(Parametro* parametro) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ParametroLista* lista = calloc(1, sizeof(ParametroLista));
    lista->parametro = parametro;
    lista->next = NULL;
    return lista;
}

ParametroLista* AppendParametroListaSemanticAction(Parametro* parametro, ParametroLista* lista) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ParametroLista* newLista = calloc(1, sizeof(ParametroLista));
    newLista->parametro = parametro;
    newLista->next = lista;
    return newLista;
}


Parametro* ParametroSemanticAction(DataType type, Identificador* identificador, ParametroArray* array) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Parametro* parametro = calloc(1, sizeof(Parametro));
    parametro->type = type;
    parametro->identifier = identificador;
    parametro->array = array;
    return parametro;
}

ParametroArray* ArrayParametroArraySemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ParametroArray* array = calloc(1, sizeof(ParametroArray));
    array->type = PARAMETRO_ARRAY_BRACKETS;
    return array;
}

ParametroArray* EmptyParametroArraySemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ParametroArray* array = calloc(1, sizeof(ParametroArray));
    array->type = PARAMETRO_ARRAY_NONE;
    return array;
}

// Block and statement actions
Bloque* BloqueSemanticAction(Sentencias* sentencias) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Bloque* bloque = calloc(1, sizeof(Bloque));
    bloque->sentencias = sentencias;
    return bloque;
}

Sentencias* AppendSentenciasSemanticAction(Sentencia* sentencia, Sentencias* sentencias) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sentencias* newSentencias = calloc(1, sizeof(Sentencias));
    newSentencias->sentencia = sentencia;
    newSentencias->next = sentencias;
    return newSentencias;
}

Sentencias* EmptySentenciasSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    return NULL;
}


Sentencia* DeclarationSentenciaSemanticAction(DataType type, Identificador* identificador, VariableSufijo* variableSufijo) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sentencia* sentencia = calloc(1, sizeof(Sentencia));
    sentencia->dataType = type;
    sentencia->identifier = identificador;
    sentencia->variableSufijo = variableSufijo;
    sentencia->type = SENTENCIA_DECLARATION;
    return sentencia;
}

Sentencia* IfSentenciaSemanticAction(SentenciaIf* sentenciaIf) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sentencia* sentencia = calloc(1, sizeof(Sentencia));
    sentencia->sentenciaIf = sentenciaIf;
    sentencia->type = SENTENCIA_IF;
    return sentencia;
}

Sentencia* WhileSentenciaSemanticAction(SentenciaWhile* sentenciaWhile) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sentencia* sentencia = calloc(1, sizeof(Sentencia));
    sentencia->sentenciaWhile = sentenciaWhile;
    sentencia->type = SENTENCIA_WHILE;
    return sentencia;
}

Sentencia* ForSentenciaSemanticAction(SentenciaFor* sentenciaFor) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sentencia* sentencia = calloc(1, sizeof(Sentencia));
    sentencia->sentenciaFor = sentenciaFor;
    sentencia->type = SENTENCIA_FOR;
    return sentencia;
}

Sentencia* ReturnSentenciaSemanticAction(SentenciaReturn* sentenciaReturn) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sentencia* sentencia = calloc(1, sizeof(Sentencia));
    sentencia->sentenciaReturn = sentenciaReturn;
    sentencia->type = SENTENCIA_RETURN;
    return sentencia;
}

Sentencia* ExpressionSentenciaSemanticAction(SentenciaExpresion* sentenciaExpresion) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sentencia* sentencia = calloc(1, sizeof(Sentencia));
    sentencia->sentenciaExpresion = sentenciaExpresion;
    sentencia->type = SENTENCIA_EXPRESSION;
    return sentencia;
}

Sentencia* BlockSentenciaSemanticAction(Bloque* bloque) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sentencia* sentencia = calloc(1, sizeof(Sentencia));
    sentencia->bloque = bloque;
    sentencia->type = SENTENCIA_BLOQUE;
    return sentencia;
}

Sentencia* EmptySentenciaSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Sentencia* sentencia = calloc(1, sizeof(Sentencia));
    sentencia->type = SENTENCIA_EMPTY;
    return sentencia;
}

SentenciaExpresion* SentenciaExpresionSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    SentenciaExpresion* sentencia = calloc(1, sizeof(SentenciaExpresion));
    sentencia->expression = expression;
    return sentencia;
}

SentenciaIf* SimpleSentenciaIfSemanticAction(Expression* condition, Bloque* thenBloque) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    SentenciaIf* sentencia = calloc(1, sizeof(SentenciaIf));
    sentencia->condition = condition;
    sentencia->thenBloque = thenBloque;
    sentencia->hasElse = 0;
    return sentencia;
}

SentenciaIf* WithElseSentenciaIfSemanticAction(Expression* condition, Bloque* thenBloque, Bloque* elseBloque) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    SentenciaIf* sentencia = calloc(1, sizeof(SentenciaIf));
    sentencia->condition = condition;
    sentencia->thenBloque = thenBloque;
    sentencia->elseBloque = elseBloque;
    sentencia->hasElse = 1;
    return sentencia;
}

SentenciaWhile* SentenciaWhileSemanticAction(Expression* condition, Bloque* bloque) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    SentenciaWhile* sentencia = calloc(1, sizeof(SentenciaWhile));
    sentencia->condition = condition;
    sentencia->bloque = bloque;
    return sentencia;
}

SentenciaFor* SentenciaForSemanticAction(Expression* init, Expression* condition, Expression* update, Bloque* bloque) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    SentenciaFor* sentencia = calloc(1, sizeof(SentenciaFor));
    sentencia->init = init;
    sentencia->hasInit = (init != NULL) ? 1 : 0;
    sentencia->condition = condition;
    sentencia->hasCondition = (condition != NULL) ? 1 : 0;
    sentencia->update = update;
    sentencia->hasUpdate = (update != NULL) ? 1 : 0;
    sentencia->bloque = bloque;
    return sentencia;
}

SentenciaReturn* SentenciaReturnSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    SentenciaReturn* sentencia = calloc(1, sizeof(SentenciaReturn));
    sentencia->expression = expression;
    sentencia->hasExpression = (expression != NULL) ? 1 : 0;
    return sentencia;
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

Expression* IdentifierExpressionSemanticAction(Identificador* identificador, IdentificadorSufijo* sufijo) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    
    if (sufijo->type == IDENTIFICADOR_SUFIJO_NONE) {
        result->identifier = identificador;
        result->type = EXPRESSION_IDENTIFIER;
    } else if (sufijo->type == IDENTIFICADOR_SUFIJO_FUNCTION_CALL) {
        result->identifierFunc = identificador;
        result->arguments = sufijo->arguments;
        result->type = EXPRESSION_FUNCTION_CALL;
    } else if (sufijo->type == IDENTIFICADOR_SUFIJO_ARRAY_ACCESS) {
        result->identifierArray = identificador;
        result->indexExpression = sufijo->indexExpression;
        result->type = EXPRESSION_ARRAY_ACCESS;
    }
    
    return result;
}

Expression* ConstantExpressionSemanticAction(Constante* constante) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->constant = constante;
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

Expression* SimpleExpresionLvalueSemanticAction(Identificador* identificador) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->identifier = identificador;
    result->type = EXPRESSION_IDENTIFIER;
    return result;
}

Expression* ArrayExpresionLvalueSemanticAction(Identificador* identificador, Expression* index) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Expression* result = calloc(1, sizeof(Expression));
    result->identifierArray = identificador;
    result->indexExpression = index;
    result->type = EXPRESSION_ARRAY_ACCESS;
    return result;
}

IdentificadorSufijo* FunctionCallIdentificadorSufijoSemanticAction(ListaArgumentos* arguments) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    IdentificadorSufijo* sufijo = calloc(1, sizeof(IdentificadorSufijo));
    sufijo->arguments = arguments;
    sufijo->type = IDENTIFICADOR_SUFIJO_FUNCTION_CALL;
    return sufijo;
}

IdentificadorSufijo* ArrayAccessIdentificadorSufijoSemanticAction(Expression* index) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    IdentificadorSufijo* sufijo = calloc(1, sizeof(IdentificadorSufijo));
    sufijo->indexExpression = index;
    sufijo->type = IDENTIFICADOR_SUFIJO_ARRAY_ACCESS;
    return sufijo;
}

IdentificadorSufijo* EmptyIdentificadorSufijoSemanticAction() {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    IdentificadorSufijo* sufijo = calloc(1, sizeof(IdentificadorSufijo));
    sufijo->type = IDENTIFICADOR_SUFIJO_NONE;
    return sufijo;
}

ListaArgumentos* SingleListaArgumentosSemanticAction(Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ListaArgumentos* lista = calloc(1, sizeof(ListaArgumentos));
    lista->expression = expression;
    lista->next = NULL;
    return lista;
}

ListaArgumentos* AppendListaArgumentosSemanticAction(ListaArgumentos* lista, Expression* expression) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ListaArgumentos* current = lista;
    while (current->next != NULL) {
        current = current->next;
    }
    
    ListaArgumentos* newArg = calloc(1, sizeof(ListaArgumentos));
    newArg->expression = expression;
    newArg->next = NULL;
    
    current->next = newArg;
    
    return lista;
}

Constante* IntegerConstanteSemanticAction(ConstanteEntera* constante) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Constante* result = calloc(1, sizeof(Constante));
    result->entera = constante;
    result->type = 0;
    return result;
}

Constante* CharacterConstanteSemanticAction(ConstanteCaracter* constante) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Constante* result = calloc(1, sizeof(Constante));
    result->caracter = constante;
    result->type = 1;
    return result;
}

ConstanteEntera* ConstanteEnteraSemanticAction(int value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ConstanteEntera* constante = calloc(1, sizeof(ConstanteEntera));
    constante->value = value;
    return constante;
}

ConstanteCaracter* ConstanteCaracterSemanticAction(char value) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    ConstanteCaracter* constante = calloc(1, sizeof(ConstanteCaracter));
    constante->value = value;
    return constante;
}

// Identifier action
Identificador* IdentificadorSemanticAction(char* name) {
    _logSyntacticAnalyzerAction(__FUNCTION__);
    Identificador* identifier = calloc(1, sizeof(Identificador));
    identifier->name = name;
    return identifier;
}