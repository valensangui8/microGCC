#include "SymbolTable.h"

static Logger* _logger = NULL;

void initializeSymbolTableModule(){ _logger = createLogger("SymbolTable"); }
void shutdownSymbolTableModule () { if(_logger) destroyLogger(_logger); }

SymbolTable* createSymbolTable(){
    SymbolTable* t = calloc(1,sizeof(SymbolTable));
    t->head = NULL; t->currentOffset = 8;    /* RBP+8, despues del RET addr */
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


void addVariable(SymbolTable* t,const char* name,DataType ty,
                 int isArr,int arrSz,const char* fnName){
    logDebugging(_logger,"Add var %s (fn=%s)",name,fnName?fnName:"GLOBAL");
    SymbolEntry* e = newEntry(name,ty,fnName);
    e->symbolType = SYMBOL_VARIABLE;
    e->isArray    = isArr;
    e->arraySize  = arrSz;
    int words = (ty==TYPE_INT)?2:1;                          //todo podria cambiar
    if(isArr) words*=arrSz;
    e->offset = t->currentOffset;
    t->currentOffset += words;
    if(t->currentOffset & 1) ++t->currentOffset;             /* alinear 2 B */


    /*
        while(table->currentOffset % 8 != 0){  todo ?
            table->currentOffset += 4;
        }
    */


    e->next = t->head; t->head = e;
}

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

int  getCurrentOffset(SymbolTable* t){ return t->currentOffset; }
void resetOffset     (SymbolTable* t){ t->currentOffset = 8; }
