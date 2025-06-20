#include "SymbolTable.h"

static Logger* _logger = NULL;

void initializeSymbolTableModule() {
    _logger = createLogger("SymbolTable");
}

void shutdownSymbolTableModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

SymbolTable* createSymbolTable() {
    SymbolTable* table = calloc(1, sizeof(SymbolTable));
    table->head = NULL;
    table->currentOffset = 8;  // Empezamos en RBP+8 (después del return address)
    return table;
}

void destroySymbolTable(SymbolTable* table) {
    if (table == NULL) return;

    SymbolEntry* current = table->head;
    while (current != NULL) {
        SymbolEntry* next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
    free(table);
}

void addVariable(SymbolTable* table, const char* name, DataType type, int isArray, int arraySize) {
    logDebugging(_logger, "Adding variable: %s", name);

    SymbolEntry* entry = calloc(1, sizeof(SymbolEntry));
    entry->name = strdup(name);
    entry->dataType = type;
    entry->symbolType = SYMBOL_VARIABLE;
    entry->isArray = isArray;
    entry->arraySize = arraySize;

    // Calcular el offset basado en el tipo y si es array
    int size = (type == TYPE_INT) ? 2 : 1;  // int = 2 bytes, char = 1 byte en 8086
    if (isArray) {
        size *= arraySize;
    }

    entry->offset = table->currentOffset;
    table->currentOffset += size;

    // Alinear a palabra si es necesario
    if (table->currentOffset % 2 != 0) {
        table->currentOffset++;
    }
    /*
    while(table->currentOffset % 8 != 0){
        table->currentOffset += 4;
    }
    */

    // Agregar al inicio de la lista
    entry->next = table->head;
    table->head = entry;
}

void addFunction(SymbolTable* table, const char* name, DataType returnType, int paramCount) {
    logDebugging(_logger, "Adding function: %s", name);

    SymbolEntry* entry = calloc(1, sizeof(SymbolEntry));
    entry->name = strdup(name);
    entry->dataType = returnType;
    entry->symbolType = SYMBOL_FUNCTION;
    entry->paramCount = paramCount;
    entry->offset = 0;  // Las funciones no tienen offset

    entry->next = table->head;
    table->head = entry;
}

void addParameter(SymbolTable* table, const char* name, DataType type, int offset, int isArray, int arraySize) {
    logDebugging(_logger, "Adding parameter: %s at offset %d", name, offset);

    SymbolEntry* entry = calloc(1, sizeof(SymbolEntry));
    entry->name = strdup(name);
    entry->dataType = type;
    entry->symbolType = SYMBOL_PARAMETER;
    entry->offset = offset;

    entry->isArray = isArray;
    entry->arraySize=arraySize;

    entry->next = table->head;
    table->head = entry;
}

SymbolEntry* lookupSymbol(SymbolTable* table, const char* name) {
    SymbolEntry* current = table->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int getCurrentOffset(SymbolTable* table) {
    return table->currentOffset;
}

void resetOffset(SymbolTable* table) {
    table->currentOffset = 8;  // Reset to RBP+8
}