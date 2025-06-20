#ifndef SYMBOL_TABLE_HEADER
#define SYMBOL_TABLE_HEADER

#include "../../shared/Logger.h"
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include <stdlib.h>
#include <string.h>


#define UNKNOWN_ARRAY_SIZE -1

typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER
} SymbolType;

typedef struct SymbolEntry {
    char* name;
    DataType dataType;
    SymbolType symbolType;
    int offset;              // Offset desde RBP para variables locales
    int paramCount;          // Para funciones: número de parámetros
    int isArray;             // 1 si es array, 0 si no
    int arraySize;           // Tamaño del array (si aplica)
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
    int currentOffset;       // Offset actual para la próxima variable local
} SymbolTable;

/** Initialize module's internal state. */
void initializeSymbolTableModule();

/** Shutdown module's internal state. */
void shutdownSymbolTableModule();

/** Create a new symbol table */
SymbolTable* createSymbolTable();

/** Destroy a symbol table and free its resources */
void destroySymbolTable(SymbolTable* table);

/** Add a variable to the symbol table */
void addVariable(SymbolTable* table, const char* name, DataType type, int isArray, int arraySize);

/** Add a function to the symbol table */
void addFunction(SymbolTable* table, const char* name, DataType returnType, int paramCount);

/** Add a parameter to the symbol table */
void addParameter(SymbolTable* table, const char* name, DataType type, int offset, int isArray, int arraySize);

/** Look up a symbol in the table */
SymbolEntry* lookupSymbol(SymbolTable* table, const char* name);

/** Get the current offset for local variables */
int getCurrentOffset(SymbolTable* table);

/** Reset offset (useful when entering a new function) */
void resetOffset(SymbolTable* table);

#endif