#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>
#include <string.h>

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */

// Program actions
Program* DeclaracionListaProgramSemanticAction(CompilerState* compilerState, DeclaracionLista* declaracionLista);
Program* EmptyProgramSemanticAction(CompilerState* compilerState);

// Declaration actions
DeclaracionLista* SingleDeclarationListSemanticAction(Declaracion* declaracion);
DeclaracionLista* AppendDeclarationListSemanticAction(DeclaracionLista* lista, Declaracion* declaracion);
DeclaracionSufijo* VariableDeclaracionSufijoSemanticAction(VariableSufijo* variableSufijo);
DeclaracionSufijo* FunctionDeclaracionSufijoSemanticAction(Parametros* parametros, FuncionSufijo* funcionSufijo);
FuncionSufijo* EmptyFuncionSufijoSemanticAction();
FuncionSufijo* BlockFuncionSufijoSemanticAction(Bloque* bloque);
VariableSufijo* EmptyVariableSufijoSemanticAction();
VariableSufijo* AssignmentVariableSufijoSemanticAction(Expression* expression);
VariableSufijo* ArrayVariableSufijoSemanticAction(int size);

// Parameter actions
Parametros* VoidParametrosSemanticAction();
Parametros* ListParametrosSemanticAction(ParametroLista* lista);
Parametros* EmptyParametrosSemanticAction();
ParametroLista* SingleParametroListaSemanticAction(Parametro* parametro);
ParametroLista* AppendParametroListaSemanticAction(Parametro* parametro, ParametroLista* lista);
ParametroArray* ArrayParametroArraySemanticAction();
ParametroArray* EmptyParametroArraySemanticAction();

// Block and statement actions
Bloque* BloqueSemanticAction(Sentencias* sentencias);
Sentencias* AppendSentenciasSemanticAction(Sentencia* sentencia, Sentencias* sentencias);
Sentencias* EmptySentenciasSemanticAction();
Sentencia* IfSentenciaSemanticAction(SentenciaIf* sentenciaIf);
Sentencia* WhileSentenciaSemanticAction(SentenciaWhile* sentenciaWhile);
Sentencia* ForSentenciaSemanticAction(SentenciaFor* sentenciaFor);
Sentencia* ReturnSentenciaSemanticAction(SentenciaReturn* sentenciaReturn);
Sentencia* ExpressionSentenciaSemanticAction(SentenciaExpresion* sentenciaExpresion);
Sentencia* BlockSentenciaSemanticAction(Bloque* bloque);
Sentencia* EmptySentenciaSemanticAction();
SentenciaExpresion* SentenciaExpresionSemanticAction(Expression* expression);
SentenciaIf* SimpleSentenciaIfSemanticAction(Expression* condition, Bloque* thenBloque);
SentenciaIf* WithElseSentenciaIfSemanticAction(Expression* condition, Bloque* thenBloque, Bloque* elseBloque);
SentenciaWhile* SentenciaWhileSemanticAction(Expression* condition, Bloque* bloque);
SentenciaFor* SentenciaForSemanticAction(Expression* init, Expression* condition, Expression* update, Bloque* bloque);
SentenciaReturn* SentenciaReturnSemanticAction(Expression* expression);

// Expression actions
Expression* AssignmentExpressionSemanticAction(Expression* lvalue, Expression* expression);
Expression* OrExpressionSemanticAction(Expression* left, Expression* right);
Expression* AndExpressionSemanticAction(Expression* left, Expression* right);
Expression* EqualExpressionSemanticAction(Expression* left, Expression* right);
Expression* NotEqualExpressionSemanticAction(Expression* left, Expression* right);
Expression* LessExpressionSemanticAction(Expression* left, Expression* right);
Expression* GreaterExpressionSemanticAction(Expression* left, Expression* right);
Expression* LessEqualExpressionSemanticAction(Expression* left, Expression* right);
Expression* GreaterEqualExpressionSemanticAction(Expression* left, Expression* right);
Expression* AdditionExpressionSemanticAction(Expression* left, Expression* right);
Expression* SubtractionExpressionSemanticAction(Expression* left, Expression* right);
Expression* MultiplicationExpressionSemanticAction(Expression* left, Expression* right);
Expression* DivisionExpressionSemanticAction(Expression* left, Expression* right);
Expression* ModuloExpressionSemanticAction(Expression* left, Expression* right);
Expression* NotExpressionSemanticAction(Expression* expression);
Expression* IdentifierExpressionSemanticAction(Identificador* identificador, IdentificadorSufijo* sufijo);
Expression* ConstantExpressionSemanticAction(Constante* constante);
Expression* ParenthesisExpressionSemanticAction(Expression* expression);
Expression* SimpleExpresionLvalueSemanticAction(Identificador* identificador);
Expression* ArrayExpresionLvalueSemanticAction(Identificador* identificador, Expression* index);

// Identifier suffix actions
IdentificadorSufijo* FunctionCallIdentificadorSufijoSemanticAction(ListaArgumentos* arguments);
IdentificadorSufijo* ArrayAccessIdentificadorSufijoSemanticAction(Expression* index);
IdentificadorSufijo* EmptyIdentificadorSufijoSemanticAction();

// Argument list actions
ListaArgumentos* SingleListaArgumentosSemanticAction(Expression* expression);
ListaArgumentos* AppendListaArgumentosSemanticAction(ListaArgumentos* lista, Expression* expression);

// Constant actions
Constante* IntegerConstanteSemanticAction(ConstanteEntera* constante);
Constante* CharacterConstanteSemanticAction(ConstanteCaracter* constante);
ConstanteEntera* ConstanteEnteraSemanticAction(int value);
ConstanteCaracter* ConstanteCaracterSemanticAction(char value);

// Identifier action
Identificador* IdentificadorSemanticAction(char* name);

#endif
