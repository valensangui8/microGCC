#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"

//TODO mover a otra carpeta supongo?????
typedef struct {
    Identifier name;         // key
    DataType type;
    DeclarationType declType;
    int isOccupied;
} Symbol;

#define SYMBOL_TABLE_CAPACITY 128

typedef struct {
    Symbol entries[SYMBOL_TABLE_CAPACITY];
} SymbolTable;



//TODO cambiar a que reciba un puntero
int insertSymbol(SymbolTable* table, Identifier name, DataType type, DeclarationType declType);
int existsSymbol(SymbolTable* table, Identifier name);
void printSymbolTable(SymbolTable* table);


#endif // SYMBOL_TABLE_H
