#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"

#define FALSE 0
#define TRUE 1

/* Tipos adicionales para distinguir variables y funciones */
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_FUNCTION
} SymbolKind;

typedef struct _Symbol {
    Identifier      name;       /* clave */
    DataType        type;       /* int, char, ... */
    DeclarationType declType;   /* regular | extern */
    SymbolKind      kind;       /* variable | function */
    int             isDefined;  /* solo p/funciones: 0 = prototipo */
    int             isOccupied; /* casilla usada */
} Symbol;

#define SYMBOL_TABLE_CAPACITY 128

    typedef struct _SymbolTable {
        Symbol                 entries[SYMBOL_TABLE_CAPACITY];
        struct _SymbolTable   *parent;   /* NULL = scope global */
    } SymbolTable;

/* Construcción / destrucción */
SymbolTable *createSymbolTable(SymbolTable *parent);
void         destroySymbolTable(SymbolTable *table);

/* Inserción (FALSE si ya existe en scope actual) */
int insertSymbol(SymbolTable *table,
                 Identifier      name,
                 DataType        type,
                 DeclarationType declType,
                 SymbolKind      kind,
                 int             isDefined);

/* Búsquedas */
Symbol *lookupSymbolCurrent(SymbolTable *table, Identifier name);
Symbol *lookupSymbol       (SymbolTable *table, Identifier name);

int existsSymbolCurrent(SymbolTable *table, Identifier name);
int existsSymbol        (SymbolTable *table, Identifier name);

void printSymbolTable(SymbolTable *table);

#endif
