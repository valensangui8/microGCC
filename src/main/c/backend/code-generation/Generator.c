#include "Generator.h"
#include "../semantic-analysis/SymbolTable.h"

/* ──────── ESTADO GLOBAL ──────── */
static Logger      *logger   = NULL;
static SymbolTable *symTable = NULL;
static int          labelCount   = 0;
static char        *fnEndLbl = NULL;
static char  *genFn = NULL;

// Arrays de todos los registros que usamos y debemos preservar en los calls
static const char* PARAM_REGISTERS[] = {"r8", "r9", "r10", "r11"};
static const char* LOCAL_REGISTERS[] = {"r12", "r13", "r14", "r15"};
#define MAX_PARAM_REGISTERS 4
#define MAX_LOCAL_REGISTERS 4

/* ──────── STATIC PROTOTYPES ────────────────────────────────────────────── */

/* utilidades de salida / helpers generales */
static char  *newLbl(void);
static const char *getInstructionSize(SymbolEntry *e);
static inline void load (unsigned n, const char *reg, SymbolEntry *e);
static inline void store(unsigned n, SymbolEntry *e);

/*  */
static void pushUsedRegisters(unsigned n);
static void popUsedRegisters(unsigned n);
static int getUsedRegistersCount(void);

/* secciones y prólogos/epílogos de archivo / función */
static void filePro(void);
static void gGlobalData(void);
static void fileEpi(void);
static void funEpi(unsigned n);

/* acceso y manipulación de arrays */
static void gArrayBase(unsigned n, SymbolEntry *e);
static void gArrayAcc (unsigned n, const char *arr, Expression *idx);

/* llamadas a funciones */
static void gFunctionCallArgs(unsigned n, ListArguments *a, int k);
static void gCall            (unsigned n, const char *name, ListArguments *argList);

/* generación de expresiones */
static void gConstant           (unsigned n, Constant *c);
static void gIdentifier         (unsigned n, const char *name);
static void gBinary             (unsigned n, Expression *e, const char *op);
static void gCompare            (unsigned n, Expression *e, const char *jmp);
static void gNotExpression      (unsigned n, Expression *e);
static void gDivModExpression   (unsigned n, Expression *e);
static void gMultExpression     (unsigned n, Expression *e);
static void gLValueArray        (unsigned n, Expression *lval);
static void gLValue             (unsigned n, Expression *lval);
static void gAssignmentExpression(unsigned n, Expression *e);
static void gExpr               (unsigned n, Expression *e);

/* generación de sentencias / control de flujo */
static void gIf        (unsigned n, StatementIf    *s);
static void gWhile     (unsigned n, StatementWhile *w);
static void gFor       (unsigned n, StatementFor   *f);
static void gReturn    (unsigned n, StatementReturn *r);
static void gDeclaration(unsigned n, Statement *s);
static void gStmt      (unsigned n, Statement *s);
static void gBlock     (unsigned n, Block *b);

/* generación de declaraciones de nivel superior */
static void gFunction  (unsigned n, Declaration *d);
static void gExtern    (Declaration *d);
static void gDecl      (unsigned n, Declaration *d);
static void gDeclList  (unsigned n, DeclarationList *l);



static char *newLbl(void){ 
    char*l=calloc(20,1); sprintf(l,".L%d",labelCount++); 
    return l; 
}



static const char * getInstructionSize(SymbolEntry * e){
    const char * size = e->dataType == TYPE_CHAR ? "byte":"qword";
    return size;
}


// Para contar registros usados en la función actual
static int getUsedRegistersCount(void) {
    int count = 0;
    for (SymbolEntry* e = symTable->head; e; e = e->next) {
        if (e->functionName && strcmp(e->functionName, genFn) == 0 &&
            e->symbolType == SYMBOL_VARIABLE &&
            e->storageLocation == STORAGE_REGISTER) {
            count++;
            }
    }
    return count;
}


// CAMBIO: Nueva función para obtener TODOS los registros usados
static void pushUsedRegisters(unsigned n) {
    // Push variables y parámetros en registros
    for (SymbolEntry* e = symTable->head; e; e = e->next) {
        if (e->functionName && strcmp(e->functionName, genFn) == 0 && e->symbolType == SYMBOL_PARAMETER && e->storageLocation == STORAGE_REGISTER) {
            out(n, "push %s\n", e->registerName);
        }
    }

    // todo: podríamos hacer (e->symbolType == SYMBOL_PARAMETER || e->symbolType == SYMBOL_VARIABLE) arriba y evitar la doble pasada

    for (SymbolEntry* e = symTable->head; e; e = e->next) {
        if (e->functionName && strcmp(e->functionName, genFn) == 0 && e->symbolType == SYMBOL_VARIABLE && e->storageLocation == STORAGE_REGISTER) {
            out(n, "push %s\n", e->registerName);
        }
    }
}

// CAMBIO: Pop en orden inverso
static void popUsedRegisters(unsigned n) {
    // Pop variables locales primero (orden inverso)
    for (int i = MAX_LOCAL_REGISTERS - 1; i >= 0; i--) {
        int found = 0;
        for (SymbolEntry* e = symTable->head; e; e = e->next) {
            if (e->functionName && strcmp(e->functionName, genFn) == 0 &&
                e->symbolType == SYMBOL_VARIABLE &&
                e->storageLocation == STORAGE_REGISTER &&
                strcmp(e->registerName, LOCAL_REGISTERS[i]) == 0) {
                out(n, "pop %s\n", e->registerName);
                found = 1;
                break;
                }
        }
    }

    // Pop parámetros después (orden inverso)
    for (int i = MAX_PARAM_REGISTERS - 1; i >= 0; i--) {
        for (SymbolEntry* e = symTable->head; e; e = e->next) {
            if (e->functionName && strcmp(e->functionName, genFn) == 0 &&
                e->symbolType == SYMBOL_PARAMETER &&
                e->storageLocation == STORAGE_REGISTER &&
                strcmp(e->registerName, PARAM_REGISTERS[i]) == 0) {
                out(n, "pop %s\n", e->registerName);
                break;
                }
        }
    }
}


static inline void load(unsigned n, const char* reg, SymbolEntry*e){
    // Si está en un registro // todo: ver si estamos manejando bien los chars
    if (e->storageLocation == STORAGE_REGISTER) {
        if (strcmp(reg, e->registerName) != 0) {
            out(n, "mov %s, %s\n", reg, e->registerName);
        }
        return;
    }

    int o = e->offset;
    char * asmInstruction = e->isArray ? "lea":(e->dataType == TYPE_CHAR ? "movzx":"mov");
    const char * size = getInstructionSize(e);

    if(e->functionName == NULL){
        out(n,"%s %s,%s[%s]\n",asmInstruction , reg, size,e->name);
        return;
    }

    out(n,o>=0? "%s %s, %s [rbp+%d]  ; %s\n":"%s %s, %s [rbp-%d]  ; %s\n",asmInstruction,reg,size,o>=0?o:-o,e->name);
}

static inline void store(unsigned n, SymbolEntry*e){
    //
    if (e->storageLocation == STORAGE_REGISTER) {
        if (strcmp("rax", e->registerName) != 0) {
            out(n, "mov %s, rax\n", e->registerName);
        }
        return;
    }

    int o = e->offset;
    const char * size = getInstructionSize(e);

    if(e->functionName == NULL){
        out(n,"mov [%s], rax\n", e->name);
        return;
    }

    out(n,o>=0? "mov [rbp+%d],rax  ; %s\n":"mov [rbp-%d],rax  ; %s\n",o>=0?o:-o,e->name);
}

static void filePro(void){
    out(0,"section .text\nglobal _start\n\n_start:\n");
    out(1,"call main\n");
    out(1,"mov rdi, rax\n");
    out(1,"mov rax, 60\n");
    out(1,"syscall\n\n");



}


static void gGlobalData(){
    SymbolEntry * entry = symTable->head;
    while (entry != NULL) {
            // Solo procesamos símbolos que no pertenecen a una función (i.e., variables globales)
        if (entry->functionName == NULL) {
            const char * varName = entry->name; // nombre original del símbolo
            if(entry->symbolType == SYMBOL_VARIABLE){
                switch (entry->dataType) {
                    case TYPE_CHAR:
                    out(1, "%s resb %d\n", varName, entry->isArray ? entry->arraySize : 1 ); // resb = 1 byte

                    break;
                    case TYPE_INT:
                    out(1, "%s resq %d\n", varName, entry->isArray ? entry->arraySize : 1 ); // resq = 8 bytes
                    break;
                    
                    default:
                    logError(logger, "; Error: tipo no reconocido para %s\n", varName);
                    break;
                }
            }
        }
        entry = entry->next;
}
}


static void fileEpi(void){

    out(0, "section .bss\n");
    gGlobalData();
}

static void funEpi(unsigned n){
    out(n,"mov rsp, rbp\n");
    out(n,"pop rbp\n");
    out(n,"ret\n");

}



/* --- acceso a arrays --- */
static void gArrayBase(unsigned n, SymbolEntry *e) {
    if (e->functionName == NULL) {
        // Es global: accedé directamente al label
        out(n, "lea rdi, [%s]\n", e->name);
        return;
    }

    const char *operation = (e->symbolType == SYMBOL_PARAMETER) ? "mov" : "lea";
    out(n, "%s rdi, [rbp%+d]\n", operation, e->offset);                               // El %+d hace que le ponga signo
}

// static void gArrayBase(unsigned n, SymbolEntry *e) {
//     if (e->functionName == NULL) {
//         out(n, "lea rdi, [%s]\n", e->name);
//         return;
//     }
//
//     // CAMBIO: Arrays siempre están en stack (por diseño)
//     const char *operation = (e->symbolType == SYMBOL_PARAMETER) ? "mov" : "lea";
//     out(n, "%s rdi, [rbp%+d]\n", operation, e->offset);
// }

static void gArrayAcc(unsigned n,const char*arr,Expression*idx){
    gExpr(n,idx); out(n,"mov rbx, rax\n");
    SymbolEntry*e=lookupSymbol(symTable,arr, genFn);
    if(e->dataType==TYPE_INT) out(n,"shl rbx,3\n");
    gArrayBase(n,e);
    out(n,"add rdi, rbx\n");
    if (e->dataType == TYPE_INT) {
        out(n, "mov rax, [rdi]\n");
    } else if (e->dataType == TYPE_CHAR) {
        out(n, "movzx rax, byte [rdi]\n");
    }
}




/* --- llamadas a funciones--- */
//
// static void gFunctionCallArgs(unsigned n,ListArguments*a , int k){
//     if(k>1) gFunctionCallArgs(n,a->next,k-1);      // Llamado recursivo para pushear en orden inverso
//     gExpr(n,a->expression);                           // El resultado de la expresion esta en RAX
//     out(n,"push rax\n");                            //  En Rax esta el parametro computado
// }

// static void gFunctionCallArgs(unsigned n, ListArguments* a, int k) {
//     // Primero evaluar todos los argumentos y guardarlos
//     int argCount = k;
//     int i = 0;
//
//     // Arrays temporales para guardar valores
//     for (ListArguments* arg = a; arg && i < k; arg = arg->next, i++) {
//         gExpr(n, arg->expression);
//         out(n, "push rax  ; temp save arg %d\n", i);
//     }
//
//     // Ahora asignar a registros o dejar en stack
//     for (i = k - 1; i >= 0; i--) {
//         if (i < MAX_PARAM_REGISTERS) {
//             out(n, "pop %s  ; arg %d to register\n", PARAM_REGISTERS[i], i);
//         } else {
//             out(n, "; arg %d stays on stack\n", i);
//         }
//     }
// }
static void gFunctionCallArgs(unsigned n, ListArguments* a, int k) {
    // Primero evaluar todos los argumentos y guardarlos
    int argCount = k;
    int i = 0;

    // Arrays temporales para guardar valores
    ListArguments * arg = a;
    for (; arg && i < k && i < MAX_PARAM_REGISTERS; arg = arg->next, i++) {
        gExpr(n, arg->expression);
        out(n, "mov %s, rax ; arg %d to register\n", PARAM_REGISTERS[i], i);
    }

    // for (int q = k; arg && q > MAX_PARAM_REGISTERS; arg = arg->next, q-- ) {
    //     gExpr(n, arg->expression);
    //     out(n, "push rax  ; save arg %d\n", i);
    // }

    // Guardamos los argumentos restantes para hacer los push en orden inverso
    int stackArgCount = k - MAX_PARAM_REGISTERS;
    if (stackArgCount < 0 || arg == NULL) {
        return;
    }

    ListArguments** stackArgs = malloc(sizeof(ListArguments*) * stackArgCount);
    int j = 0;
    for (; arg && j < stackArgCount; arg = arg->next, j++) { //CAMBIO
        stackArgs[j] = arg;
    }

    for (int l = stackArgCount - 1; l >= 0; l--) { //CAMBIO
        gExpr(n, stackArgs[l]->expression); //CAMBIO
        out(n, "push rax  ; save arg %d (reversed)\n", MAX_PARAM_REGISTERS + l); //CAMBIO
    }

    free(stackArgs);

}


static void gCall(unsigned n,const char*name,ListArguments * argList){
    pushUsedRegisters(n); // pusheamos los registros con las variables locales (todo: y parametros?)

    int c=0;
    for(ListArguments * t = argList ; t ; t=t->next) ++c;  // Cuenta argumentos
    if(c) gFunctionCallArgs(n,argList,c);             // Apila los argumentos    // llamado de funcion --> Resultado en RAX

    out(n,"call %s\n",name);

    // llamado de funcion --> Resultado en RAX
    int stackParams = (c > MAX_PARAM_REGISTERS) ? c - MAX_PARAM_REGISTERS : 0;
    if(stackParams > 0) out(n, "add rsp, %d\n", stackParams * 8);                  // limpia la pila

    popUsedRegisters(n); // popeamos los registros con las variables locales (todo: y parametros?)
}


/* Los resultados de las expresiones se ponen en rax*/

static void gConstant(unsigned n,Constant*c){
    out(n,"mov rax,%d\n",*c->integer);
}

static void gIdentifier(unsigned n,const char*name){
    load(n,"rax",lookupSymbol(symTable,name, genFn));  // cargo el identificador en RAX 
}

static void gBinary(unsigned n,Expression*e,const char*op){
    gExpr(n,e->leftExpression);
    out(n,"push rax\n");                   //*  Resultado de haber llamado a gExpr(n,e->leftExpression);
    gExpr(n,e->rightExpression);            //*  Resultado en RAX
    out(n,"pop rbx\n");                    //*  Resultado de la e->leftExpression en RBX
    out(n,"%s rbx, rax\n",op);            //*   operacion leftExpression, rightExpresion
    out(n, "mov rax, rbx\n");             //*   Dejo resultado en RAX
}

static void gCompare(unsigned n,Expression*e,const char*jmp){
    char *T=newLbl(),*End=newLbl();
    gExpr(n,e->leftExpression);
    out(n,"push rax\n");
    gExpr(n,e->rightExpression);
    out(n,"pop rbx\n");
    out(n,"cmp rbx, rax\n");                // En RBX esta leftExpression, en rax rightExpression
    out(n, "%s %s\n",jmp,T);
    out(n,"mov rax,0\n");                    // No se cumplio la condicion de salto. RAX <- 0.
    out(n, "jmp %s\n",End);
    out(0,"%s:\n",T);
    out(n,"mov rax,1\n");                   // Se cumplio la condicion de salto. RAX <- 1.
    out(0,"%s:\n",End);                   
    free(T);
    free(End);
}

// Si la comparación fue verdadera (la expresión fue igual a cero en C),
// se asigna 1 (TRUE) a RAX como resultado booleano. De lo contrario se le asigna 0.
static void gNotExpression(unsigned n, Expression * e){
    gExpr(n,e->singleExpression);
    out(n,"cmp rax,0\n");    // Compara rax con 0 (esto setea los flags)
    out(n,"mov rax,0\n");    // No setea flags.
    out(n,"sete al\n");     //  al = (Zero Flag == 1) ? 1 : 0
}

static void gDivModExpression(unsigned n, Expression * e){
    gExpr(n,e->leftExpression);
    out(n,"push rax\n");
    gExpr(n,e->rightExpression);
    out(n,"mov rbx, rax\n");        // RBX --> rightExpression
    out(n,"pop rax\n");             // RAX --> leftExpression
    out(n,"cqo\n");                 //  Deja a RDX en 0 o -1 dependiendo del signo de RAX (prepara la extension RDX:RAX para idiv)
    out(n,"idiv rbx\n");            //  RDX:RAX / RBX --> Rta: RDX: resto. RAX: cociente

    if(e->type==EXPRESSION_MODULO) out(n,"mov rax, rdx\n");     //Dejo el resto en RAX.
}

static void gMultExpression(unsigned n, Expression * e){
    gExpr(n,e->leftExpression);
    out(n,"push rax\n");
    gExpr(n,e->rightExpression);
    out(n,"pop rbx\n");
    out(n,"imul rax, rbx\n");
}



static void gLValueArray(unsigned n, Expression * lval){
    out(n,"push rax\n");                     // guardar la expresion computada (lo que esta despues del = en la asignacion).
    gExpr(n,lval->indexExpression);           //  evalua la expresion del indice y lo deja en RAX.
    out(n,"mov rbx, rax\n");                //  RBX --> indice del ARRAY
    SymbolEntry*e=lookupSymbol(symTable,*lval->identifierArray, genFn);
    if(e->dataType==TYPE_INT) out(n,"shl rbx,3\n"); // Como los INTS ocupan 8Bytes, hace RBX = 8*RBX
    gArrayBase(n,e);                            // deja en RDI la dir. del comienzo del array
    out(n,"add rdi, rbx\n");                // Le sumo el offset del indice a RDI
    out(n, "pop rax\n");                    // recupero la expresion de la derecha

    if (e->dataType == TYPE_INT) {
        out(n, "mov [rdi], rax\n");         // Caso 1, es INT por lo que muevo todo el registro al array (8 bytes)
    } else if (e->dataType == TYPE_CHAR) {
        out(n, "mov byte [rdi], al\n");     // Caso 2,  es CHAR, por lo que muevo solo un BYTE
    }
}

/*
 *  A esta funcion le llega con la expresion de la derecha ya computada.
 *  Es decir, RAX tiene el resultado que se debe asignar.
 */
static void gLValue(unsigned n,Expression*lval){
    if(lval->type==EXPRESSION_IDENTIFIER){
        store(n,lookupSymbol(symTable,*lval->identifier, genFn));
        return;
    }
    gLValueArray(n, lval);
}

static void gAssignmentExpression(unsigned n, Expression * e){
    gExpr(n,e->rightExpression);
    gLValue(n,e->leftExpression);
}


/* --- dispatcher --- */
static void gExpr(unsigned n,Expression*e){
    switch(e->type){
        case EXPRESSION_CONSTANT:       gConstant(n,e->constant); break;
        case EXPRESSION_IDENTIFIER:     gIdentifier(n,*e->identifier); break;
        case EXPRESSION_ARRAY_ACCESS:   gArrayAcc(n,*e->identifierArray,e->indexExpression); break;
        case EXPRESSION_ASSIGNMENT:     gAssignmentExpression(n,e); break;
        case EXPRESSION_ADDITION:       gBinary(n,e,"add"); break;
        case EXPRESSION_SUBTRACTION:    gBinary(n,e,"sub"); break;
        case EXPRESSION_MULTIPLICATION: gMultExpression(n,e); break;
        case EXPRESSION_DIVISION:
        case EXPRESSION_MODULO:         {gDivModExpression(n, e);break;}
        case EXPRESSION_OR:             gBinary(n,e,"or");  break;
        case EXPRESSION_AND:            gBinary(n,e,"and"); break;
        case EXPRESSION_EQUAL:          gCompare(n,e,"je");  break;
        case EXPRESSION_NOT_EQUAL:      gCompare(n,e,"jne"); break;
        case EXPRESSION_LESS:           gCompare(n,e,"jl");  break;
        case EXPRESSION_GREATER:        gCompare(n,e,"jg");  break;
        case EXPRESSION_LESS_EQUAL:     gCompare(n,e,"jle"); break;
        case EXPRESSION_GREATER_EQUAL:  gCompare(n,e,"jge"); break;
        case EXPRESSION_NOT:            gNotExpression(n, e); break;
        case EXPRESSION_FUNCTION_CALL:  gCall(n,*e->identifierFunc,e->arguments); break;
        case EXPRESSION_PARENTHESIS:    gExpr(n,e->singleExpression); break;
    }
}

/* ──────── CONTROL FLUJO / BLOQUES ──────── */


static void gIf(unsigned n, StatementIf *s){
    char *lblElse = newLbl();                                      // etiqueta para la rama else
    char *lblEnd  = newLbl();                                      // etiqueta de salida del if completo
    gExpr(n, s->condition);                                     // evalúa la condición -> Resulatdo en RAX
    out(n, "cmp rax, 0\n");                                   // ¿condición == 0?
    out(n, "je %s\n", s->hasElse ? lblElse : lblEnd);         // si es 0 (falso) salta a else o end
    gBlock(n, s->thenBlock);                                    // ejecuta el bloque dentro del IF

    if (s->hasElse) {
        out(n, "jmp %s\n", lblEnd);                            // salta al final para no caer al else
        out(0, "%s:\n", lblElse);                           // etiqueta ELSE
        gBlock(n, s->elseBlock);                                // ejecuta el bloque ELSE
    }

    out(0, "%s:\n", lblEnd);                                // etiqueta END
    free(lblElse);
    free(lblEnd);
}

static void gWhile(unsigned n, StatementWhile *w){
    char *lblTop = newLbl();         // inicio del bucle
    char *lblEnd = newLbl();         // salida del bucle

    out(0, "%s:\n", lblTop);         //etiqueta de inicio del bucle
    gExpr(n, w->condition);          // evalúa condición -> RAX tiene el resultado
    out(n, "cmp rax, 0\n");          // comprueba si es 0
    out(n, "je %s\n", lblEnd);       // si es 0, terminar

    gBlock(n, w->block);             // cuerpo del WHILE
    out(n, "jmp %s\n", lblTop);      // vuelve al inicio

    out(0, "%s:\n", lblEnd);         // etiqueta END
    free(lblTop);
    free(lblEnd);
}

static void gFor(unsigned n, StatementFor *f){
    char *lblTop = newLbl();         // etiqueta de inicio del bucle
    char *lblEnd = newLbl();         // etiqueta de salida del bucle

    if (f->hasInit) gExpr(n, f->init);            // inicialización (si existe)
    out(0, "%s:\n", lblTop);         // etiqueta TOP

    if (f->hasCondition) {           // condición (si existe)
        gExpr(n, f->condition);      // cond -> RAX
        out(n, "cmp rax, 0\n");      // comprueba si es 0
        out(n, "je %s\n", lblEnd);   // si es 0, salir
    }

    gBlock(n, f->block);             // cuerpo del FOR
    if (f->hasUpdate) gExpr(n, f->update);  // actualización (si existe)

    out(n, "jmp %s\n", lblTop);      // vuelve al inicio
    out(0, "%s:\n", lblEnd);         // etiqueta END

    free(lblTop);
    free(lblEnd);
}



/* ──────── STMTS/BLOCKS ──────── */
static void gReturn(unsigned n,StatementReturn*r){
    if(r->hasExpression) gExpr(n,r->expression);
    out(n,"jmp %s\n",fnEndLbl);
}

static void gDeclaration(unsigned n, Statement * s){
    if(s->variableSuffix->type==VARIABLE_SUFFIX_ASSIGNMENT){
        gExpr(n,s->variableSuffix->expression);
        store(n,lookupSymbol(symTable,*s->identifier, genFn)); }
}

static void gStmt(unsigned n,Statement*s){
    switch(s->type){
        case STATEMENT_DECLARATION: gDeclaration(n,s);          break;
        case STATEMENT_IF:      gIf(n,s->statementIf);       break;
        case STATEMENT_WHILE:   gWhile(n,s->statementWhile); break;
        case STATEMENT_FOR:     gFor(n,s->statementFor);     break;
        case STATEMENT_RETURN:  gReturn(n,s->statementReturn); break;
        case STATEMENT_EXPRESSION: gExpr(n,s->statementExpression->expression); break;
        case STATEMENT_BLOCK:   gBlock(n,s->block);          break;
        default: break;
    }
}
static void gBlock(unsigned n,Block*b){
    for(Statements*t=b->statements;t;t=t->next){
        gStmt(n,t->statement);
        if(t->statement->type==STATEMENT_RETURN) break;
    }
}

///* ──────── FUNCIÓN ──────── */

static void gFunction(unsigned n, Declaration* d) {
    char* callersFunctionName = genFn;
    genFn = *d->identifier;
    fnEndLbl = newLbl();

    out(0, "%s:\n", *d->identifier);
    out(n, "push rbp\n");
    out(n, "mov rbp, rsp\n");

    // Mover parámetros del stack a registros
    // if (d->declarationSuffix->parameters->type == PARAMS_LIST) {
    //     ParameterList* p = d->declarationSuffix->parameters->list;
    //     int paramOffset = 16;
    //     int paramRegIndex = 0;
    //
    //     while (p != NULL) {
    //         SymbolEntry* paramEntry = lookupSymbol(symTable, *p->parameter->identifier, genFn);
    //         if (paramEntry && paramEntry->storageLocation == STORAGE_REGISTER) {
    //             // Cargar parámetro del stack al registro asignado
    //             out(n, "mov %s, [rbp+%d]\n", paramEntry->registerName, paramOffset);
    //         }
    //         paramOffset += 8;
    //         p = p->next;
    //     }
    // }

    int minOffset = 0;

    for (SymbolEntry* e = symTable->head; e; e = e->next) {
        if (e->functionName && strcmp(e->functionName, genFn) == 0 &&
            e->symbolType == SYMBOL_VARIABLE && e->storageLocation == STORAGE_STACK &&  e->offset < minOffset) {
            minOffset = e->offset;
        }
    }

    int localBytes = -minOffset;
    if (localBytes > 0) {
        int aligned = (localBytes + 15) & ~15;
        out(n, "sub rsp, %d\n", aligned);
    }

    gBlock(n, d->declarationSuffix->functionSuffix->block);

    out(0, "%s:\n", fnEndLbl);
    funEpi(n);
    free(fnEndLbl); fnEndLbl = NULL;
    genFn = callersFunctionName;
}


static void gExtern(Declaration *d){
    out(0,"extern %s\n",*d->identifier);
}

/* ──────── DECLARATIONS / FILE ──────── */
static void gDecl(unsigned n, Declaration*d){
    if(d->declarationType==DECLARATION_EXTERN) {
        gExtern(d);
        return;
    }
    if(d->declarationSuffix->type==DECLARATION_SUFFIX_FUNCTION &&
       d->declarationSuffix->functionSuffix->type==SUFFIX_BLOCK) gFunction(n,d);
}
static void gDeclList(unsigned n, DeclarationList*l){ for(;l;l=l->next) gDecl(n,l->declaration); }

/* ──────── API ──────── */
void initializeGeneratorModule(){ logger=createLogger("Generator"); }
void shutdownGeneratorModule () { if(logger) destroyLogger(logger); }

void generate(CompilerState *cs){
    symTable=cs->st; labelCount=0;

    printSymbolTable(symTable);

    filePro();
    Program *root=(Program*)cs->abstractSyntaxtTree;

    if(root && root->type==PROGRAM_DECLARATIONS) gDeclList(1,root->declarationList);
    fileEpi();
    shutdownGeneratorUtils();
}
