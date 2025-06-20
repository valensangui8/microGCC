#ifndef SYMBOL_TABLE_HEADER
#define SYMBOL_TABLE_HEADER

#include "../../shared/Logger.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdlib.h>
#include <string.h>

#define UNKNOWN_ARRAY_SIZE -1

/** Tipo de símbolo */
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER
} SymbolType;

/* ─────────── Entrada de la TS ─────────── */
typedef struct SymbolEntry {
    char*      name;
    DataType   dataType;
    SymbolType symbolType;
    char*      functionName;        //NULL => global; otro => nombre de la función
    int offset;                     /* variables / parámetros               */
    int paramCount;                 /* funciones                            */
    int isArray;
    int arraySize;                  /* UNKNOWN_ARRAY_SIZE si no se conoce   */

    struct SymbolEntry* next;
} SymbolEntry;

/*
typedef struct SymbolEntry {
    char* name;
    DataType dataType;
    SymbolType symbolType;
    union{
        int paramCount;          // Para funciones: número de parámetros
        struct {
            int offset;              // Offset desde RBP para variables locales
            int isArray;             // 1 si es array, 0 si no
            int arraySize;           // Tamaño del array (si aplica)
        };
    };

    struct SymbolEntry* next;
} SymbolEntry;
*/ //@TODO


typedef struct {
    SymbolEntry* head;
    int          currentOffset;     /* Offset actual para locales (palabras)*/
} SymbolTable;

/* Init / shutdown */
void initializeSymbolTableModule();
void shutdownSymbolTableModule();

/* Creación / destrucción de tabla */
SymbolTable* createSymbolTable();
void         destroySymbolTable(SymbolTable* table);

/* Inserciones */
void addVariable (SymbolTable* t,const char* name,DataType ty,
                  int isArr,int arrSz,const char* fnName);
void addFunction (SymbolTable* t,const char* name,DataType ret,int nPar);
void addParameter(SymbolTable* t,const char* name,DataType ty,int off,
                  int isArr,int arrSz,const char* fnName);

SymbolEntry* lookupSymbol(SymbolTable* t,const char* name,
                          const char* currentFnName);

/* Offset helpers */
int  getCurrentOffset(SymbolTable* t);
void resetOffset     (SymbolTable* t);

#endif
