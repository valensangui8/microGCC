#include "SymbolTable.h"

static Logger* _logger = NULL;

void initializeSymbolTableModule(){ _logger = createLogger("SymbolTable"); }
void shutdownSymbolTableModule () { if(_logger) destroyLogger(_logger); }

SymbolTable* createSymbolTable(){
    SymbolTable* t = calloc(1,sizeof(SymbolTable));
    t->head = NULL; t->currentOffset = -8;    /* RBP+8, despues del RET addr */
    return t;
}
void destroySymbolTable(SymbolTable* t){
    if(!t) return;
    SymbolEntry* e = t->head;
    while(e){
        SymbolEntry* nxt = e->next;
        free(e->name);
        if(e->functionName) free(e->functionName);
        free(e);
        e = nxt;
    }
    free(t);
}

static SymbolEntry* newEntry(const char* name,DataType ty,const char* fnName){
    SymbolEntry* e = calloc(1,sizeof(SymbolEntry));
    e->name = strdup(name);
    e->dataType = ty;
    e->functionName = fnName ? strdup(fnName) : NULL;
    return e;
}

void addVariable(SymbolTable* t, const char* name, DataType ty,
                 int isArr, int arrSz, const char* fnName)
{
    logDebugging(_logger,"Add var %s (fn=%s)",name,fnName?fnName:"GLOBAL");

    SymbolEntry* e        = newEntry(name, ty, fnName);
    e->symbolType         = SYMBOL_VARIABLE;
    e->isArray            = isArr;
    e->arraySize          = arrSz;

    int bytesPerElement   = (ty == TYPE_INT) ? 8 : 1;              // ints: 8 bytes, chars: 1 byte
    int totalBytes        = isArr ? bytesPerElement * arrSz        // array
                                  : bytesPerElement;               // escalar

    if (fnName != NULL) {

        t->currentOffset -= totalBytes;

        while (t->currentOffset % 8 != 0) {
            t->currentOffset--;           //alienacion
        }

        e->offset = t->currentOffset;
    }

    e->next   = t->head;
    t->head   = e;
}


/*
void addVariable(SymbolTable* t,const char* name,DataType ty,
                 int isArr,int arrSz,const char* fnName){
    logDebugging(_logger,"Add var %s (fn=%s)",name,fnName?fnName:"GLOBAL");
    SymbolEntry* e = newEntry(name,ty,fnName);
    e->symbolType = SYMBOL_VARIABLE;
    e->isArray    = isArr;
    e->arraySize  = arrSz;
    int bytes = (ty==TYPE_INT) ? 8:1;                          //todo podria cambiar. por ahora siempre 8 para que este alienado
    if(isArr){
       bytes = arrSz * bytes;
    }
    if(fnName != NULL){
        e->offset = t->currentOffset;
        t->currentOffset -= bytes;
    }
    while(t->currentOffset % 8 != 0){
        t->currentOffset++;
    }


    e->next = t->head; t->head = e;
}*/

void addFunction(SymbolTable* t,const char* name,DataType ret,int nPar){
    logDebugging(_logger,"Add func %s",name);
    SymbolEntry* e = newEntry(name,ret,NULL);
    e->symbolType = SYMBOL_FUNCTION;
    e->paramCount = nPar;
    e->next = t->head; t->head = e;
}

void addParameter(SymbolTable* t,const char* name,DataType ty,int off,
                  int isArr,int arrSz,const char* fnName){
    logDebugging(_logger,"Add param %s (fn=%s)",name,fnName);
    SymbolEntry* e = newEntry(name,ty,fnName);
    e->symbolType = SYMBOL_PARAMETER;
    e->offset     = off;
    e->isArray    = isArr;
    e->arraySize  = arrSz;
    e->next = t->head; t->head = e;
}

/* ────── búsqueda ────── */
SymbolEntry* lookupSymbol(SymbolTable* t,const char* name,
                          const char* currentFnName){
    SymbolEntry* cur = t->head;
    SymbolEntry* globalHit = NULL;
    while(cur){
        if(strcmp(cur->name,name)==0){
            if(currentFnName && cur->functionName &&
               strcmp(cur->functionName,currentFnName)==0)
                return cur;                                  /* primero local */
            if(cur->functionName==NULL) globalHit = cur;     /* guarda global */
        }
        cur = cur->next;
    }
    return globalHit;
}

int  getLocalVarsOffset(SymbolTable* t, char * functionName){
    SymbolEntry * entry= lookupSymbol(t, functionName, NULL);
    return t->currentOffset;
}
void resetOffset     (SymbolTable* t){ t->currentOffset = -8; }





/******************** PRINT FUNCTION *********************/



/* ── Helpers de conversión ────────────────────────────────────────── */
static const char * dataTypeStr(DataType t) {
    switch (t) {
        case TYPE_CHAR: return "char";
        case TYPE_INT:  return "int";
        default:        return "??";
    }
}

static const char * symbolTypeStr(SymbolType s) {
    switch (s) {
        case SYMBOL_VARIABLE:  return "var";
        case SYMBOL_FUNCTION:  return "func";
        case SYMBOL_PARAMETER: return "param";
        default:               return "??";
    }
}

/* ── Impresión linda de la TS ─────────────────────────────────────── */
void printSymbolTable(const SymbolTable * table)
{
    if (!table || !table->head) {
        printf(";(symbol table is empty)\n");
        return;
    }

    puts("\n;┌─────────────────────────────────────────── Symbol Table ────────────────────────────────────────────┐");
    puts(";│ Name                │ Type │ Kind  │ Scope/function │ Offset/params │ Array │ Size │");
    puts(";├─────────────────────┼──────┼───────┼────────────────┼───────────────┼───────┼──────┤");

    for (const SymbolEntry * e = table->head; e; e = e->next) {
        const char * scope = (e->functionName == NULL) ? "<global>" : e->functionName;
        const char * arr   = e->isArray ? "yes" : "no";
        int  size          = (e->isArray && e->arraySize != UNKNOWN_ARRAY_SIZE) ? e->arraySize
                                                                                : (e->isArray ? -1 : 0);

        /* Para variables/params imprimimos offset; para funciones imprimimos #params */
        int offOrCnt = (e->symbolType == SYMBOL_FUNCTION) ? e->paramCount
                                                          : e->offset;

        printf(";│ %-19s │ %-4s │ %-5s │ %-14s │ %13d │ %-5s │ %-4d │\n",
               e->name,
               dataTypeStr(e->dataType),
               symbolTypeStr(e->symbolType),
               scope,
               offOrCnt,
               arr,
               size);
    }

    puts(";└───────────────────────────────────────────────────────────────────────────────────────────────────────┘");
}
