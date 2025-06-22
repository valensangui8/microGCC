#include "SymbolTable.h"

static Logger* _logger = NULL;

// Arrays de registros disponibles
static const char* PARAM_REGISTERS[] = {"r8", "r9", "r10", "r11"};
static const char* LOCAL_REGISTERS[] = {"r12", "r13", "r14", "r15"};
#define MAX_PARAM_REGISTERS 4
#define MAX_LOCAL_REGISTERS 4

static SymbolEntry *newEntry(const char *name, DataType ty, const char *fnName);
static const char *dataTypeStr(DataType t);
static const char *symbolTypeStr(SymbolType s);

void initializeSymbolTableModule(){ _logger = createLogger("SymbolTable"); }
void shutdownSymbolTableModule () { if(_logger) destroyLogger(_logger); }

SymbolTable* createSymbolTable(){
    SymbolTable* t = calloc(1,sizeof(SymbolTable));
    t->head = NULL; t->currentOffset = -8;    /* RBP+8, despues del RET addr */
    t->currentParamRegisterCount = 0;
    t->currentLocalRegisterCount = 0;
    return t;
}
void destroySymbolTable(SymbolTable* t){
    if(!t) return;
    SymbolEntry* e = t->head;
    while(e){
        SymbolEntry* nxt = e->next;
        free(e->name);
        if(e->functionName) free(e->functionName);
        if(e->symbolType != SYMBOL_FUNCTION && e->storageLocation == STORAGE_REGISTER && e->registerName) {
            free(e->registerName);
        }
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

    if (fnName == NULL) {
        // A las variables globales le asignamos STORAGE_STACK todo: raro -> no tiene mucho sentido
        e->storageLocation = STORAGE_STACK;
        e->offset = 0;
        e->next   = t->head;
        t->head   = e;
        return;
    }

    // Intentamos asignar un registro si hay disponibles
    if (!isArr && t->currentLocalRegisterCount < MAX_LOCAL_REGISTERS) {
        e->storageLocation = STORAGE_REGISTER;
        e->registerName = strdup(LOCAL_REGISTERS[t->currentLocalRegisterCount]);
        t->currentLocalRegisterCount++;
        logDebugging(_logger, "Assigned register %s to variable %s", e->registerName, name);
    } else {
        // Usamos stack si no qudan registros o es un array
        e->storageLocation = STORAGE_STACK;
        t->currentOffset -= totalBytes;
        while (t->currentOffset % 8 != 0) { // alineación
            t->currentOffset--;
        }
        e->offset = t->currentOffset;
    }

    e->next   = t->head;
    t->head   = e;
}


void addFunction(SymbolTable* t,const char* name,DataType ret,int nPar, FunctionStatus functionStatus){
    logDebugging(_logger,"Add func %s",name);
    SymbolEntry* e = newEntry(name,ret,NULL);
    e->symbolType = SYMBOL_FUNCTION;
    e->paramCount = nPar;
    e->functionStatus = functionStatus;
    e->next = t->head; t->head = e;
}


void addParameter(SymbolTable* t, const char* name, DataType ty, int* off, int isArr, int arrSz, const char* fnName){
    logDebugging(_logger,"Add param %s (fn=%s)",name,fnName);
    SymbolEntry* e = newEntry(name,ty,fnName);
    e->symbolType = SYMBOL_PARAMETER;
    // e->offset     = off;
    e->isArray    = isArr;
    e->arraySize  = arrSz;

    // Intentar asignar registro para parámetros no-array
    if (!isArr && t->currentParamRegisterCount < MAX_PARAM_REGISTERS) {
        e->storageLocation = STORAGE_REGISTER;
        e->registerName = strdup(PARAM_REGISTERS[t->currentParamRegisterCount]);
        t->currentParamRegisterCount++;
        logDebugging(_logger, "Assigned register %s to parameter %s", e->registerName, name);
    } else {
        // Usar stack si no hay registros o es array todo: en realidad acá el array podría pasarse también por un registro, puesto que estamos mandando la dir. de memoria del array
        e->storageLocation = STORAGE_STACK;
        e->offset = *off;
        *off += 8;
        // todo: offset += 8;
    }

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


void resetRegisterCounters(SymbolTable* t) {
    t->currentParamRegisterCount = 0;
    t->currentLocalRegisterCount = 0;
}


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

    puts("\n;┌─────────────────────────────────────────────────── Symbol Table ─────────────────────────────────────────┐");
    puts(";│ Name                │ Type │ Kind  │ Scope/function │ Storage │ Location      │ Array │ Size │ Status  │");
    puts(";├─────────────────────┼──────┼───────┼────────────────┼─────────┼───────────────┼───────┼──────┼─────────┤");

    for (const SymbolEntry * e = table->head; e; e = e->next) {
        const char *scope  = (e->functionName == NULL) ? "<global>" : e->functionName;
        const char *arr    = e->isArray ? "yes" : "no";
        int  size          = (e->isArray && e->arraySize != UNKNOWN_ARRAY_SIZE) ? e->arraySize
                                                                                : (e->isArray ? -1 : 0);

        const char *statusStr = "-";
        if (e->symbolType == SYMBOL_FUNCTION) {
            switch (e->functionStatus) {
                case EXTERN_FUN:                       statusStr = "EXTERN";      break;
                case DEFINED_FUN:                      statusStr = "DEFINED";     break;
                case DECLARED_BUT_NOT_DEFINED_FUN:     statusStr = "DECL-ND";     break;
                default:                               statusStr = "?";           break;
            }
        }

        if (e->symbolType == SYMBOL_FUNCTION) {
            printf(";│ %-19s │ %-4s │ %-5s │ %-14s │ %-7s │ %13d │ %-5s │ %-4d │ %-7s │\n",
                   e->name,
                   dataTypeStr(e->dataType),
                   symbolTypeStr(e->symbolType),
                   scope,
                   "-",
                   e->paramCount,
                   arr,
                   size,
                   statusStr);
        } else {
            char location[16];
            if (e->storageLocation == STORAGE_REGISTER) {
                snprintf(location, sizeof(location), "%s", e->registerName);
            } else {
                snprintf(location, sizeof(location), "%d", e->offset);
            }

            printf(";│ %-19s │ %-4s │ %-5s │ %-14s │ %-7s │ %-13s │ %-5s │ %-4d │ %-7s │\n",
                   e->name,
                   dataTypeStr(e->dataType),
                   symbolTypeStr(e->symbolType),
                   scope,
                   e->storageLocation == STORAGE_REGISTER ? "REG" : "STACK",
                   location,
                   arr,
                   size,
                   statusStr);
        }
    }

    puts(";└──────────────────────────────────────────────────────────────────────────────────────────────────────────┘");
}
// void printSymbolTable(const SymbolTable * table)
// {
//     if (!table || !table->head) {
//         printf(";(symbol table is empty)\n");
//         return;
//     }
//
//     puts("\n;┌───────────────────────────────────────────── Symbol Table ───────────────────────────────────┐");
//     puts(";│ Name                │ Type │ Kind  │ Scope/function │ Offset/params │ Array │ Size │ Status  │");
//     puts(";├─────────────────────┼──────┼───────┼────────────────┼───────────────┼───────┼──────┼─────────┤");
//
//     for (const SymbolEntry * e = table->head; e; e = e->next) {
//
//         const char *scope  = (e->functionName == NULL) ? "<global>" : e->functionName;
//         const char *arr    = e->isArray ? "yes" : "no";
//         int  size          = (e->isArray && e->arraySize != UNKNOWN_ARRAY_SIZE) ? e->arraySize
//                                                                                 : (e->isArray ? -1 : 0);
//
//         /* Para variables/params imprimimos offset; para funciones imprimimos #params */
//         int offOrCnt = (e->symbolType == SYMBOL_FUNCTION) ? e->paramCount
//                                                           : e->offset;
//
//         /* Determinar el texto de la columna Status */
//         const char *statusStr = "-";                  // valor por defecto
//         if (e->symbolType == SYMBOL_FUNCTION) {
//             switch (e->functionStatus) {                   // nuevo campo en SymbolEntry
//                 case EXTERN_FUN:                       statusStr = "EXTERN";      break;
//                 case DEFINED_FUN:                      statusStr = "DEFINED";     break;
//                 case DECLARED_BUT_NOT_DEFINED_FUN:     statusStr = "DECL-ND";     break;
//                 default:                               statusStr = "?";           break;
//             }
//         }
//
//         printf(";│ %-19s │ %-4s │ %-5s │ %-14s │ %13d │ %-5s │ %-4d │ %-7s │\n",
//                e->name,
//                dataTypeStr(e->dataType),
//                symbolTypeStr(e->symbolType),
//                scope,
//                offOrCnt,
//                arr,
//                size,
//                statusStr);
//     }
//
//     puts(";└──────────────────────────────────────────────────────────────────────────────────────────────┘");
// }
