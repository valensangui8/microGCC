//
// Created by lulos on 6/19/2025.
//

#include "SymbolTable.h"

static unsigned int hash(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash % SYMBOL_TABLE_CAPACITY;
}

int insertSymbol(SymbolTable* table, Identifier name, DataType type, DeclarationType declType) {
    unsigned int index = hash(name);
    for (int i = 0; i < SYMBOL_TABLE_CAPACITY; i++) {
        unsigned int try = (index + i) % SYMBOL_TABLE_CAPACITY;
        if (!table->entries[try].isOccupied) {
            table->entries[try].name = strdup(name);
            table->entries[try].type = type;
            table->entries[try].declType = declType;
            table->entries[try].isOccupied = 1;
            return 1; // success
        } else if (strcmp(table->entries[try].name, name) == 0) {
            return 0; // already exists
        }
    }
    return 0; // table full
}

int existsSymbol(SymbolTable* table, Identifier name) {
    unsigned int index = hash(name);
    for (int i = 0; i < SYMBOL_TABLE_CAPACITY; i++) {
        unsigned int try = (index + i) % SYMBOL_TABLE_CAPACITY;
        if (table->entries[try].isOccupied && strcmp(table->entries[try].name, name) == 0) {
            return 1; // exists
        }
        if (!table->entries[try].isOccupied) {
            return 0; // not found
        }
    }
    return 0;
}

void printSymbolTable(SymbolTable* table) {
    printf("Symbol Table:\n");
    for (int i = 0; i < SYMBOL_TABLE_CAPACITY; ++i) {
        if (table->entries[i].isOccupied) {
            printf("  %s => Type: %s, DeclType: %s\n",
                   table->entries[i].name,
                   table->entries[i].type == TYPE_INT ? "int" : "char",
                   table->entries[i].declType == DECLARATION_REGULAR ? "regular" : "extern");
        }
    }
}
