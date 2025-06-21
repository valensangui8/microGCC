#include "Generator.h"
#include "../semantic-analysis/SymbolTable.h"

/* ──────── ESTADO GLOBAL ──────── */
static Logger      *logger   = NULL;
static SymbolTable *symTable = NULL;
static int          labelCount   = 0;
static char        *fnEndLbl = NULL;
static char  *genFn = NULL;



static void gExpr(unsigned,Expression*);
static void gStmt(unsigned,Statement*);
static void gBlock(unsigned,Block*);
static void gDeclList(DeclarationList*);
static void gFunctionCallArgs(unsigned,ListArguments*,int);

/* ──────── UTILIDADES DE SALIDA ──────── */
static char *pad(unsigned n){ return 
    indentation(' ',n,4); 
}

static void out(unsigned n,const char *fmt,...){
    va_list ap; va_start(ap,fmt);
    char *p=pad(n),*ef=concatenate(2,p,fmt);
    vfprintf(stdout,ef,ap); fflush(stdout);
    free(ef); free(p); va_end(ap);
}
static char *newLbl(void){ 
    char*l=calloc(20,1); sprintf(l,".L%d",labelCount++); 
    return l; 
}



static const char * getInstructionSize(SymbolEntry * e){
    const char * size = e->dataType == TYPE_CHAR ? "byte":"qword";
    return size;
}


static inline void load(unsigned n,const char*reg,SymbolEntry*e){
    int o = e->offset;
    char * asmInstruction = e->isArray ? "lea":(e->dataType == TYPE_CHAR ? "movzx":"mov");
    const char * size = getInstructionSize(e);

    if(e->functionName == NULL){
        out(n,"%s %s,%s[%s]\n",asmInstruction , reg, size,e->name);
        return;
    }

    out(n,o>=0? "%s %s,[rbp+%d]  ; %s\n":"%s %s,[rbp-%d]  ; %s\n",
        asmInstruction,reg,o>=0?o:-o,e->name);  //@todo rompi algo???
}

static inline void store(unsigned n,SymbolEntry*e){
    int o = e->offset;
    const char * size = getInstructionSize(e);

    if(e->functionName == NULL){
        out(n,"mov %s [%s], rax\n",size, e->name);
        return;
    }

    out(n,o>=0? "mov %s [rbp+%d],rax  ; %s\n":"mov %s [rbp-%d],rax  ; %s\n",size,
        o>=0?o:-o,e->name);
}

static void filePro(void){
    out(0,"section .text\nglobal _start\n\n_start:\n");
    out(1,"call main\nmov rdi, rax\nmov rax, 60\nsyscall\n\n");
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
    out(0, "; end of file\n");
}

static void funEpi(unsigned n){
    out(n,"mov rsp, rbp\npop rbp\nret\n");
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


static void gLValue(unsigned n,Expression*lval){
    if(lval->type==EXPRESSION_IDENTIFIER){
        store(n,lookupSymbol(symTable,*lval->identifier, genFn));
    }else{
        out(n,"push rax\n");
        gExpr(n,lval->indexExpression);
        out(n,"mov rbx, rax\n");
        SymbolEntry*e=lookupSymbol(symTable,*lval->identifierArray, genFn);
        if(e->dataType==TYPE_INT) out(n,"shl rbx,3\n");
        gArrayBase(n,e);
        out(n,"add rdi, rbx\npop rax\n");

        if (e->dataType == TYPE_INT) {
            out(n, "mov [rdi], rax\n");
        } else if (e->dataType == TYPE_CHAR) {
            out(n, "mov byte [rdi], al\n");
        }
    }
}

/* --- llamadas a funciones--- */
static void gFunctionCallArgs(unsigned n,ListArguments*a , int k){
    if(k>1) gFunctionCallArgs(n,a->next,k-1);      // Llamado recursivo para pushear en orden inverso
    gExpr(n,a->expression);                           // El resultado de la expresion esta en RAX
    out(n,"push rax\n");
}
static void gCall(unsigned n,const char*name,ListArguments * argList){
    int c=0; 
    for(ListArguments * t = argList ; t ; t=t->next) ++c;
    if(c) gFunctionCallArgs(n,argList,c);
    out(n,"call %s\n",name);
    if(c) out(n,"add rsp,%d\n",c*8);
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
    out(n,"mov rax,0\njmp %s\n",End);       // No se cumplio la condicion de salto. RAX <- 0.
    out(0,"%s:\n",T);           
    out(n,"mov rax,1\n");                   // Se cumplio la condicion de salto. RAX <- 1.
    out(0,"%s:\n",End);                   
    free(T);
    free(End);
}

/* --- dispatcher --- */
static void gExpr(unsigned n,Expression*e){
    switch(e->type){
        case EXPRESSION_CONSTANT:       gConstant(n,e->constant); break;
        case EXPRESSION_IDENTIFIER:     gIdentifier(n,*e->identifier); break;
        case EXPRESSION_ARRAY_ACCESS:   gArrayAcc(n,*e->identifierArray,e->indexExpression); break;
        case EXPRESSION_ASSIGNMENT:
            gExpr(n,e->rightExpression); gLValue(n,e->leftExpression); break;
        case EXPRESSION_ADDITION:       gBinary(n,e,"add"); break;
        case EXPRESSION_SUBTRACTION:    gBinary(n,e,"sub"); break;
        case EXPRESSION_MULTIPLICATION:{
            gExpr(n,e->leftExpression); out(n,"push rax\n");
            gExpr(n,e->rightExpression);out(n,"pop rbx\nimul rax, rbx\n"); break; }
        case EXPRESSION_DIVISION:
        case EXPRESSION_MODULO:{
            gExpr(n,e->leftExpression); out(n,"push rax\n");
            gExpr(n,e->rightExpression);out(n,"mov rbx, rax\npop rax\ncqo\nidiv rbx\n");
            if(e->type==EXPRESSION_MODULO) out(n,"mov rax, rdx\n"); break; }
        case EXPRESSION_OR:             gBinary(n,e,"or");  break;
        case EXPRESSION_AND:            gBinary(n,e,"and"); break;
        case EXPRESSION_EQUAL:          gCompare(n,e,"je");  break;
        case EXPRESSION_NOT_EQUAL:      gCompare(n,e,"jne"); break;
        case EXPRESSION_LESS:           gCompare(n,e,"jl");  break;
        case EXPRESSION_GREATER:        gCompare(n,e,"jg");  break;
        case EXPRESSION_LESS_EQUAL:     gCompare(n,e,"jle"); break;
        case EXPRESSION_GREATER_EQUAL:  gCompare(n,e,"jge"); break;
        case EXPRESSION_NOT:
            gExpr(n,e->singleExpression); out(n,"cmp rax,0\nmov rax,0\nsete al\n"); break;
        case EXPRESSION_FUNCTION_CALL:  gCall(n,*e->identifierFunc,e->arguments); break;
        case EXPRESSION_PARENTHESIS:    gExpr(n,e->singleExpression); break;
    }
}

/* ──────── CONTROL FLUJO / BLOQUES ──────── */
static void gIf(unsigned n,StatementIf*s){
    char *Else=newLbl(),*End=newLbl();
    gExpr(n,s->condition); out(n,"cmp rax,0\nje %s\n",s->hasElse?Else:End);
    gBlock(n,s->thenBlock);
    if(s->hasElse){
        out(n,"jmp %s\n",End);
        out(0,"%s:\n",Else); gBlock(n,s->elseBlock);
    }
    out(0,"%s:\n",End); free(Else); free(End);
}
static void gWhile(unsigned n,StatementWhile*w){
    char*Top=newLbl(),*End=newLbl();
    out(0,"%s:\n",Top);
    gExpr(n,w->condition); out(n,"cmp rax,0\nje %s\n",End);
    gBlock(n,w->block);    out(n,"jmp %s\n",Top);
    out(0,"%s:\n",End); free(Top); free(End);
}
static void gFor(unsigned n,StatementFor*f){
    char*Top=newLbl(),*End=newLbl();
    if(f->hasInit) gExpr(n,f->init);
    out(0,"%s:\n",Top);
    if(f->hasCondition){ gExpr(n,f->condition);
        out(n,"cmp rax,0\nje %s\n",End);}
    gBlock(n,f->block);
    if(f->hasUpdate) gExpr(n,f->update);
    out(n,"jmp %s\n",Top);
    out(0,"%s:\n",End); free(Top); free(End);
}

/* ──────── STMTS/BLOCKS ──────── */
static void gReturn(unsigned n,StatementReturn*r){
    if(r->hasExpression) gExpr(n,r->expression);
    out(n,"jmp %s\n",fnEndLbl);
}
static void gStmt(unsigned n,Statement*s){
    switch(s->type){
        case STATEMENT_DECLARATION:
            if(s->variableSuffix->type==VARIABLE_SUFFIX_ASSIGNMENT){
                gExpr(n,s->variableSuffix->expression);
                store(n,lookupSymbol(symTable,*s->identifier, genFn)); }
            break;
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

static void gFunction(Declaration* d) {
    char* callersFunctionName = genFn;
    genFn = *d->identifier;
    fnEndLbl = newLbl();

    out(0, "%s:\n", *d->identifier);
    out(1, "push rbp\nmov rbp, rsp\n");

    int minOffset = 0;

    for (SymbolEntry* e = symTable->head; e; e = e->next) {     //@TODO .modularizar, o agregar un campo en la tabla mejor dicho.
        if (e->functionName && strcmp(e->functionName, genFn) == 0 &&
            e->symbolType == SYMBOL_VARIABLE && e->offset < minOffset) {
            minOffset = e->offset;
        }
    }

    int localBytes = -minOffset ;  
    if (localBytes > 0) {
        int aligned = (localBytes + 15) & ~15;
        out(1, "sub rsp, %d\n", aligned);
    }

    gBlock(1, d->declarationSuffix->functionSuffix->block);

    out(0, "%s:\n", fnEndLbl);
    funEpi(1);
    free(fnEndLbl); fnEndLbl = NULL;
    genFn = callersFunctionName;
}


static void gExtern(Declaration *d){
    out(0,"extern %s\n",*d->identifier);
}

/* ──────── DECLARATIONS / FILE ──────── */
static void gDecl(Declaration*d){
    if(d->declarationType==DECLARATION_EXTERN) {
        gExtern(d);
        return;
    }
    if(d->declarationSuffix->type==DECLARATION_SUFFIX_FUNCTION &&
       d->declarationSuffix->functionSuffix->type==SUFFIX_BLOCK) gFunction(d);
}
static void gDeclList(DeclarationList*l){ for(;l;l=l->next) gDecl(l->declaration); }

/* ──────── API ──────── */
void initializeGeneratorModule(){ logger=createLogger("Generator"); }
void shutdownGeneratorModule () { if(logger) destroyLogger(logger); }

void generate(CompilerState *st,SymbolTable *tbl){
    symTable=tbl; labelCount=0;

    printSymbolTable(symTable);

    filePro();
    Program *root=(Program*)st->abstractSyntaxtTree;

    if(root && root->type==PROGRAM_DECLARATIONS) gDeclList(root->declarationList);
    fileEpi();
}
