#ifndef GENERATOR_HEADER
#define GENERATOR_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../semantic-analysis/SymbolTable.h"
#include <stdarg.h>
#include <stdio.h>

/** Initialize module's internal state. */
void initializeGeneratorModule();

/** Shutdown module's internal state. */
void shutdownGeneratorModule();

/**
 * Generates the final output using the current compiler state and symbol table.
 * The symbol table must be already populated and validated.
 */
void generate(CompilerState * compilerState, SymbolTable* symbolTable);

/* Private function declarations - for internal use only */
static void _generateProgram(Program * program);
static void _generateDeclarationList(DeclarationList* list);
static void _generateDeclaration(Declaration* decl);
static void _generateFunction(Declaration* funcDecl);
static void _generateBlock(const unsigned int indentationLevel, Block* block);
static void _generateStatements(const unsigned int indentationLevel, Statements* stmts);
static void _generateStatement(const unsigned int indentationLevel, Statement* stmt);
static void _generateIfStatement(const unsigned int indentationLevel, StatementIf* stmt);
static void _generateWhileStatement(const unsigned int indentationLevel, StatementWhile* stmt);
static void _generateForStatement(const unsigned int indentationLevel, StatementFor* stmt);
static void _generateReturnStatement(const unsigned int indentationLevel, StatementReturn* stmt);
static void _generateExpression(const unsigned int indentationLevel, Expression* expr);
static void _generateBinaryOperation(const unsigned int indentationLevel, Expression* expr, const char* op);
static void _generateComparisonExpression(const unsigned int indentationLevel, Expression* expr, const char* jumpInstr);
static void _generateLValue(const unsigned int indentationLevel, Expression* expr);
static void _generateIdentifier(const unsigned int indentationLevel, const char* name);
static void _generateConstant(const unsigned int indentationLevel, Constant* constant);
static void _generateArrayAccess(const unsigned int indentationLevel, const char* arrayName, Expression* index);
static void _generateFunctionCall(const unsigned int indentationLevel, const char* funcName, ListArguments* args);
static void _generateArgumentsReverse(const unsigned int indentationLevel, ListArguments* args, int count);

#endif