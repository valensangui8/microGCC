//
// Created by lulos on 6/19/2025.
//

#ifndef SEMANTIC_ANALYZER_HEADER
#define SEMANTIC_ANALYZER_HEADER

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "SymbolTable.h"
#include <stdbool.h>

typedef enum {
    SEMANTIC_OK,
    SEMANTIC_ERROR_UNDECLARED_VARIABLE,
    SEMANTIC_ERROR_UNDEFINED_FUNCTION,
    SEMANTIC_ERROR_UNDECLARED_FUNCTION,
    SEMANTIC_ERROR_REDECLARED_IDENTIFIER,
    SEMANTIC_ERROR_TYPE_MISMATCH,
    SEMANTIC_ERROR_WRONG_ARGUMENT_COUNT,
    SEMANTIC_ERROR_ARRAY_INDEX_TYPE,
    SEMANTIC_ERROR_NON_ARRAY_INDEXED,
    SEMANTIC_ERROR_VOID_VARIABLE,
    SEMANTIC_ERROR_RETURN_TYPE_MISMATCH,
    SEMANTIC_ERROR_MISSING_RETURN,
    SEMANTIC_ERROR_ARRAY_SIZE
} SemanticError;

typedef struct {
    bool hasErrors;
    SymbolTable* symbolTable;
    DataType currentFunctionReturnType;
    char* currentFunctionName;
    bool functionHasReturn;
} SemanticContext;

/** Initialize module's internal state. */
void initializeSemanticAnalyzerModule();

/** Shutdown module's internal state. */
void shutdownSemanticAnalyzerModule();

/**
 * Performs semantic analysis on the AST and populates the symbol table.
 * Returns true if the analysis succeeded, false if there were errors.
 */
bool analyzeSemantics(CompilerState* compilerState);

/**
 * Get the populated symbol table after successful semantic analysis.
 * Returns NULL if analysis hasn't been performed or failed.
 */
SymbolTable* getSymbolTable();

#endif