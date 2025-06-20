#include "Generator.h"
#include "../semantic-analysis/SymbolTable.h"

static Logger      *logger   = NULL;
static SymbolTable *symTable = NULL;
static int          lblCnt   = 0;
static char        *fnEndLbl = NULL;
static int          totalW   = 0;

static char *pad(unsigned n){ return indentation(' ', n, 4); }
static void out(unsigned n,const char *fmt,...)
{
    va_list ap; va_start(ap,fmt);
    char *p  = pad(n);
    char *ef = concatenate(2,p,fmt);
    vfprintf(stdout,ef,ap); fflush(stdout);
    free(ef); free(p); va_end(ap);
}
static char *newLbl(void){ char*l=calloc(20,1); sprintf(l,".L%d",lblCnt++); return l; }

static inline int sizeWords(SymbolEntry *e)
{
    int base = (e->dataType == TYPE_INT) ? 2 : 1;          /* 2→8  | 1→1  */
    return e->isArray ? base * e->arraySize : base;
}
static inline int localDisp(SymbolEntry *e)
{
    int szW        = sizeWords(e);
    int wordsAfter = totalW - (e->offset + szW);           /* palabras por debajo */
    return -(wordsAfter*4 + szW*4);                        /* → bytes negativos   */
}
static inline int effOff(SymbolEntry *e)
{
    if(e->symbolType == SYMBOL_PARAMETER){
        int idx = (e->offset - 4) / 2;                     /* 0,1,… */
        return 16 + idx*8;                                 /* +16,+24,… */
    }
    return localDisp(e);                                   /* locales ≤ -8 */
}
static inline void load(unsigned n,const char*reg,SymbolEntry*e)
{
    int o=effOff(e);
    out(n,o>=0? "mov %s,[rbp+%d]  ; %s\n" : "mov %s,[rbp-%d]  ; %s\n",
        reg, o>=0?o:-o, e->name);
}
static inline void store(unsigned n,SymbolEntry*e)
{
    int o=effOff(e);
    out(n,o>=0? "mov [rbp+%d],rax  ; %s\n" : "mov [rbp-%d],rax  ; %s\n",
        o>=0?o:-o, e->name);
}

static void filePro(void){
    out(0,"section .text\n"
          "global _start\n\n"
          "_start:\n");
    out(1,"call main\n"
          "mov rdi, rax\n"
          "mov rax, 60\n"
          "syscall\n\n");
}
static void fileEpi(void){ out(0,"; end of file\n"); }
static void epi(unsigned n){ out(n,"mov rsp, rbp\npop rbp\nret\n"); }

static void gExpr (unsigned,Expression*);
static void gStmt (unsigned,Statement*);
static void gBlock(unsigned,Block*);
static void gDeclList(DeclarationList*);
static void gArgsRev(unsigned,ListArguments*,int);

static void gConst(unsigned n,Constant*c){ out(n,"mov rax,%d\n",*c->integer); }
static void gIdent(unsigned n,const char*s){ load(n,"rax",lookupSymbol(symTable,s)); }

static void gBinary(unsigned n,Expression*e,const char*op)
{
    gExpr(n,e->leftExpression);  out(n,"push rax\n");
    gExpr(n,e->rightExpression); out(n,"pop rbx\n");
    out(n,"%s rbx, rax\nmov rax, rbx\n",op);
}
static void gCompare(unsigned n,Expression*e,const char*jmp)
{
    char *T=newLbl(), *End=newLbl();
    gExpr(n,e->leftExpression);  out(n,"push rax\n");
    gExpr(n,e->rightExpression); out(n,"pop rbx\n");
    out(n,"cmp rbx, rax\n%s %s\n",jmp,T);
    out(n,"mov rax,0\njmp %s\n",End);
    out(0,"%s:\n",T); out(n,"mov rax,1\n");
    out(0,"%s:\n",End); free(T); free(End);
}

static void gArrayAcc(unsigned n,const char*arr,Expression*idx)
{
    gExpr(n,idx); out(n,"mov rbx, rax\n");
    SymbolEntry*e = lookupSymbol(symTable,arr);
    if(e->dataType==TYPE_INT) out(n,"shl rbx,3\n");          /* ×8 */
    int base=effOff(e);
    out(n, base>=0? "lea rdi,[rbp+%d]\n" : "lea rdi,[rbp-%d]\n",
        base>=0?base:-base);
    out(n,"add rdi, rbx\nmov rax,[rdi]\n");
}
static void gLValue(unsigned n,Expression*lval)
{
    if(lval->type==EXPRESSION_IDENTIFIER){
        store(n,lookupSymbol(symTable,*lval->identifier));
    }else{
        out(n,"push rax\n");
        gExpr(n,lval->indexExpression); out(n,"mov rbx, rax\n");
        SymbolEntry*e=lookupSymbol(symTable,*lval->identifierArray);
        if(e->dataType==TYPE_INT) out(n,"shl rbx,3\n");
        int base=effOff(e);
        out(n, base>=0? "lea rdi,[rbp+%d]\n" : "lea rdi,[rbp-%d]\n",
            base>=0?base:-base);
        out(n,"add rdi, rbx\npop rax\nmov [rdi], rax\n");
    }
}

static void gArgsRev(unsigned n,ListArguments*a,int k)
{
    if(k>1) gArgsRev(n,a->next,k-1);
    gExpr(n,a->expression); out(n,"push rax\n");
}
static void gCall(unsigned n,const char*name,ListArguments*a)
{
    int c=0; for(ListArguments*t=a;t;t=t->next) ++c;
    if(c) gArgsRev(n,a,c);
    out(n,"call %s\n",name);
    if(c) out(n,"add rsp,%d\n",c*8);
}

static void gExpr(unsigned n,Expression*e)
{
    switch(e->type){
        case EXPRESSION_CONSTANT:       gConst(n,e->constant); break;
        case EXPRESSION_IDENTIFIER:     gIdent(n,*e->identifier); break;
        case EXPRESSION_ARRAY_ACCESS:   gArrayAcc(n,*e->identifierArray,e->indexExpression); break;
        case EXPRESSION_ASSIGNMENT:
            gExpr(n,e->rightExpression); gLValue(n,e->leftExpression); break;
        case EXPRESSION_ADDITION:       gBinary(n,e,"add"); break;
        case EXPRESSION_SUBTRACTION:    gBinary(n,e,"sub"); break;
        case EXPRESSION_MULTIPLICATION:{
            gExpr(n,e->leftExpression);  out(n,"push rax\n");
            gExpr(n,e->rightExpression); out(n,"pop rbx\nimul rax, rbx\n"); break; }
        case EXPRESSION_DIVISION:
        case EXPRESSION_MODULO:{
            gExpr(n,e->leftExpression);  out(n,"push rax\n");
            gExpr(n,e->rightExpression); out(n,"mov rbx, rax\npop rax\ncqo\nidiv rbx\n");
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
            gExpr(n,e->singleExpression);
            out(n,"cmp rax,0\nmov rax,0\nsete al\n"); break;
        case EXPRESSION_FUNCTION_CALL:  gCall(n,*e->identifierFunc,e->arguments); break;
        case EXPRESSION_PARENTHESIS:    gExpr(n,e->singleExpression); break;
    }
}

static void gIf(unsigned n,StatementIf*s)
{
    char *Else=newLbl(), *End=newLbl();
    gExpr(n,s->condition); out(n,"cmp rax,0\nje %s\n",s->hasElse?Else:End);
    gBlock(n,s->thenBlock);
    if(s->hasElse){
        out(n,"jmp %s\n",End);
        out(0,"%s:\n",Else); gBlock(n,s->elseBlock);
    }
    out(0,"%s:\n",End); free(Else); free(End);
}
static void gWhile(unsigned n,StatementWhile*w)
{
    char*Top=newLbl(),*End=newLbl();
    out(0,"%s:\n",Top);
    gExpr(n,w->condition); out(n,"cmp rax,0\nje %s\n",End);
    gBlock(n,w->block);    out(n,"jmp %s\n",Top);
    out(0,"%s:\n",End); free(Top); free(End);
}
static void gFor(unsigned n,StatementFor*f)
{
    char*Top=newLbl(),*End=newLbl();
    if(f->hasInit) gExpr(n,f->init);
    out(0,"%s:\n",Top);
    if(f->hasCondition){
        gExpr(n,f->condition); out(n,"cmp rax,0\nje %s\n",End);}
    gBlock(n,f->block);
    if(f->hasUpdate) gExpr(n,f->update);
    out(n,"jmp %s\n",Top);
    out(0,"%s:\n",End); free(Top); free(End);
}


static void gReturn(unsigned n,StatementReturn*r)
{
    if(r->hasExpression) gExpr(n,r->expression);
    out(n,"jmp %s\n",fnEndLbl);
}
static void gStmt(unsigned n,Statement*s)
{
    switch(s->type){
        case STATEMENT_DECLARATION:
            if(s->variableSuffix->type==VARIABLE_SUFFIX_ASSIGNMENT){
                gExpr(n,s->variableSuffix->expression);
                store(n,lookupSymbol(symTable,*s->identifier)); }
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
static void gBlock(unsigned n,Block*b)
{
    for(Statements*t=b->statements;t;t=t->next){
        gStmt(n,t->statement);
        if(t->statement->type==STATEMENT_RETURN) break;
    }
}


static void gFunction(Declaration*d)
{
    totalW = getCurrentOffset(symTable);          /* palabras totales */
    fnEndLbl = newLbl();

    out(0,"%s:\n",*d->identifier);
    out(1,"push rbp\nmov rbp, rsp\n");

    int localBytes = (totalW - 8) * 4;            /* quitar 8 palabras iniciales */
    if(localBytes){
        int aligned = (localBytes + 15) & ~15;    /* 16-byte align */
        out(1,"sub rsp,%d\n",aligned);
    }
    gBlock(1,d->declarationSuffix->functionSuffix->block);

    out(0,"%s:\n",fnEndLbl); epi(1);
    free(fnEndLbl); fnEndLbl=NULL;
}


static void gDecl(Declaration*d){
    if(d->declarationType==DECLARATION_EXTERN) return;
    if(d->declarationSuffix->type==DECLARATION_SUFFIX_FUNCTION &&
       d->declarationSuffix->functionSuffix->type==SUFFIX_BLOCK) gFunction(d);
}
static void gDeclList(DeclarationList*l){ for(;l;l=l->next) gDecl(l->declaration); }

/* ─────────────────────────────
   API
   ───────────────────────────── */
void initializeGeneratorModule(){ logger = createLogger("Generator"); }
void shutdownGeneratorModule () { if(logger) destroyLogger(logger); }

void generate(CompilerState *st,SymbolTable *tbl)
{
    symTable = tbl; lblCnt = 0;
    filePro();
    Program *root=(Program*)st->abstractSyntaxtTree;
    if(root && root->type==PROGRAM_DECLARATIONS) gDeclList(root->declarationList);
    fileEpi();
}
