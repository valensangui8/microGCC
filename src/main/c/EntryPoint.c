#include "backend/code-generation/Generator.h"
#include "backend/semantic-analysis/SemanticAnalyzer.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
#include "shared/CompilerState.h"
#include "shared/Environment.h"
#include "shared/Logger.h"
#include "shared/String.h"

const int main(const int count, const char ** arguments) {
    Logger * logger = createLogger("EntryPoint");
    initializeFlexActionsModule();
    initializeBisonActionsModule();
    initializeSyntacticAnalyzerModule();
    initializeAbstractSyntaxTreeModule();
    initializeSemanticAnalyzerModule();
    initializeGeneratorModule();

    // Logs the arguments of the application.
    for (int k = 0; k < count; ++k) {
        logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
    }

    // Begin compilation process.
    CompilerState compilerState = {
            .abstractSyntaxtTree = NULL,
            .succeed = false,
            .value = 0
    };

    CompilationStatus compilationStatus = SUCCEED;

    // Phase 1: Syntactic Analysis
    const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);

    if (syntacticAnalysisStatus == ACCEPT) {
        logDebugging(logger, "Syntactic analysis succeeded. Starting semantic analysis...");

        // Phase 2: Semantic Analysis
        if (analyzeSemantics(&compilerState)) {
            logDebugging(logger, "Semantic analysis succeeded. Generating assembly code...");

            // Phase 3: Code Generation
            SymbolTable* symbolTable = getSymbolTable();
            if (symbolTable != NULL) {
                generate(&compilerState, symbolTable);
                logDebugging(logger, "Code generation completed successfully.");
            } else {
                logError(logger, "Failed to retrieve symbol table.");
                compilationStatus = FAILED;
            }
        } else {
            logError(logger, "Semantic analysis failed. Code generation aborted.");
            compilationStatus = FAILED;
        }
    } else {
        logError(logger, "The syntactic-analysis phase rejects the input program.");
        compilationStatus = FAILED;
    }

    logDebugging(logger, "Releasing modules resources...");
    shutdownGeneratorModule();
    shutdownSemanticAnalyzerModule();
    shutdownAbstractSyntaxTreeModule();
    shutdownSyntacticAnalyzerModule();
    shutdownBisonActionsModule();
    shutdownFlexActionsModule();
    logDebugging(logger, "Compilation is done.");
    destroyLogger(logger);
    return compilationStatus;
}