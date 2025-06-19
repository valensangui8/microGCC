//
// Created by lulos on 6/19/2025.
//

#include "AstNavigator.h"
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

static Logger * _logger = NULL;

/* Private Functions */



static int evaluateDeclarationSuffix(DeclarationSuffix * declarationSuffix, SymbolTable * symbolTable){
    if(declarationSuffix->type == DECLARATION_SUFFIX_VARIABLE){
        logError(_logger, "esto 1");
        return TRUE;
    }else if (declarationSuffix->type == DECLARATION_SUFFIX_FUNCTION){
        logError(_logger, "esto 2");
    }

    logError(_logger, "esto es raro %d ", declarationSuffix->type);


    return FALSE;
}

static int evaluateDeclaration(Declaration * dec, SymbolTable * symbolTable){
    if(existsSymbol(symbolTable, *dec->identifier)){
        logError(_logger, "Symbol already exists %d", dec->identifier);
        return FALSE;
    }
    insertSymbol(symbolTable,*dec->identifier, dec->dataType, dec->declarationType);  //Todo por ahora lo checkeo el valor de lo que retorna esto

    return evaluateDeclarationSuffix(dec->declarationSuffix, symbolTable);

}

static int evaluateDeclarationList(DeclarationList * declarationList, SymbolTable * symbolTable){

    if(declarationList == NULL){
        logError(_logger, "Declaration list is null");
        return FALSE;
    }

    if(declarationList->next != NULL){
        return evaluateDeclaration(declarationList->declaration,symbolTable) && evaluateDeclarationList(declarationList->next, symbolTable);
    }

    return evaluateDeclaration(declarationList->declaration, symbolTable);
}


static int evaluateProgram(Program * program , SymbolTable * table){
    if(program->type == PROGRAM_EMPTY){
        return TRUE;
    }
    return evaluateDeclarationList(program->declarationList, table);
}


//*  PUBLIC FUNCTIONS *//
void initializeAstNavigatorModule() {
    _logger = createLogger("AstNavigator");
}

void shutdownAstNavigatorModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}




int navigateAst(Program * program, SymbolTable * table){
    initializeAstNavigatorModule(); //todo mover
    if(program == NULL || table == NULL){
        logError(_logger, "Program or symbol table null");
        return false;
    }
    return evaluateProgram(program, table);
}

