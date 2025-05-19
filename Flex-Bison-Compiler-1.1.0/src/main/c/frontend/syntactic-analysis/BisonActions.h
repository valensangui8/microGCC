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
Program* DeclarationListProgramSemanticAction(CompilerState* compilerState, DeclarationList* declarationList);
Program* EmptyProgramSemanticAction(CompilerState* compilerState);

// Declaration actions
DeclarationList* SingleDeclarationListSemanticAction(Declaration* declaration);
DeclarationList* AppendDeclarationListSemanticAction(DeclarationList* list, Declaration* declaration);
DeclarationSuffix* VariableDeclarationSuffixSemanticAction(VariableSuffix* variableSuffix);
DeclarationSuffix* FunctionDeclarationSuffixSemanticAction(Parameters* parameters, FunctionSuffix* functionSuffix);
FunctionSuffix* EmptyFuncionSuffixSemanticAction();
FunctionSuffix* BlockFuncionSuffixSemanticAction(Block* block);
VariableSuffix* EmptyVariableSuffixSemanticAction();
VariableSuffix* AssignmentVariableSuffixSemanticAction(Expression* expression);
VariableSuffix* ArrayVariableSuffixSemanticAction(int size);

// Parameter actions
Parameters* VoidParametrosSemanticAction();
Parameters* ListParametrosSemanticAction(ParameterList* list);
Parameters* EmptyParametrosSemanticAction();
ParameterList* SingleParametroListSemanticAction(Parameter* parameter);
ParameterList* AppendParametroListSemanticAction(Parameter* parameter, ParameterList* list);
ParameterArray* ArrayParametroArraySemanticAction();
ParameterArray* EmptyParametroArraySemanticAction();

// Block and statement actions
Block* BlockSemanticAction(Statements* statements);
Statements* AppendStatementsSemanticAction(Statement* statement, Statements* statements);
Statements* EmptyStatementsSemanticAction();
Statement* IfStatementSemanticAction(StatementIf* statementIf);
Statement* WhileStatementSemanticAction(StatementWhile* statementWhile);
Statement* ForStatementSemanticAction(StatementFor* statementFor);
Statement* ReturnStatementSemanticAction(StatementReturn* statementReturn);
Statement* ExpressionStatementSemanticAction(StatementExpression* statementExpression);
Statement* BlockStatementSemanticAction(Block* block);
Statement* EmptyStatementSemanticAction();
StatementExpression* StatementExpressionSemanticAction(Expression* expression);
StatementIf* SimpleStatementIfSemanticAction(Expression* condition, Block* thenBlock);
StatementIf* WithElseStatementIfSemanticAction(Expression* condition, Block* thenBlock, Block* elseBlock);
StatementWhile* StatementWhileSemanticAction(Expression* condition, Block* block);
StatementFor* StatementForSemanticAction(Expression* init, Expression* condition, Expression* update, Block* block);
StatementReturn* StatementReturnSemanticAction(Expression* expression);

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
Expression* IdentifierExpressionSemanticAction(Identifier* identifier, IdentifierSuffix* suffix);
Expression* ConstantExpressionSemanticAction(Constant* constant);
Expression* ParenthesisExpressionSemanticAction(Expression* expression);
Expression* SimpleExpressionLvalueSemanticAction(Identifier* identifier);
Expression* ArrayExpressionLvalueSemanticAction(Identifier* identifier, Expression* index);

// Identifier suffix actions
IdentifierSuffix* FunctionCallIdentifierSuffixSemanticAction(ListArguments* arguments);
IdentifierSuffix* ArrayAccessIdentifierSuffixSemanticAction(Expression* index);
IdentifierSuffix* EmptyIdentifierSuffixSemanticAction();

// Argument list actions
ListArguments* SingleListArgumentsSemanticAction(Expression* expression);
ListArguments* AppendListArgumentsSemanticAction(ListArguments* list, Expression* expression);

// Constant actions
Constant* IntegerConstantSemanticAction(ConstantInteger* constant);
Constant* CharacterConstantSemanticAction(ConstantCharacter* constant);
ConstantInteger* ConstantIntegerSemanticAction(int value);
ConstantCharacter* ConstantCharacterSemanticAction(char value);

// Identifier action
Identifier* IdentifierSemanticAction(char* name);

#endif