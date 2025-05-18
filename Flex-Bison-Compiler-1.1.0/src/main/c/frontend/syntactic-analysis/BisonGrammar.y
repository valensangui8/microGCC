
%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */
	int integer;
	char character;
	char* string;
	DataType dataType;
	Token token;

	/** Non-terminals. */
	Bloque* bloque;
	Constante* constante;
	ConstanteCaracter* constanteCaracter;
	ConstanteEntera* constanteEntera;
	Declaracion* declaracion;
	DeclaracionLista* declaracionLista;
	DeclaracionSufijo* declaracionSufijo;
	Expression* expression;
	FuncionSufijo* funcionSufijo;
	Identificador* identificador;
	IdentificadorSufijo* identificadorSufijo;
	ListaArgumentos* listaArgumentos;
	Parametro* parametro;
	ParametroArray* parametroArray;
	ParametroLista* parametroLista;
	Parametros* parametros;
	Program* program;
	Sentencia* sentencia;
	Sentencias* sentencias;
	SentenciaExpresion* sentenciaExpresion;
	SentenciaFor* sentenciaFor;
	SentenciaIf* sentenciaIf;
	SentenciaReturn* sentenciaReturn;
	SentenciaWhile* sentenciaWhile;
	VariableSufijo* variableSufijo;
}

/** Terminals. */
%token <integer> INTEGER
%token <character> CHARACTER
%token <string> IDENTIFIER

%token <token> ADD
%token <token> AND
%token <token> ASSIGN
%token <token> CHAR
%token <token> CLOSE_BRACE
%token <token> CLOSE_BRACKET
%token <token> CLOSE_PARENTHESIS
%token <token> COMMA
%token <token> DIV
%token <token> ELSE
%token <token> EQUAL
%token <token> EXTERN
%token <token> FOR
%token <token> GREATER
%token <token> GREATER_EQUAL
%token <token> IF
%token <token> INT
%token <token> LESS
%token <token> LESS_EQUAL
%token <token> MOD
%token <token> MUL
%token <token> NOT
%token <token> NOT_EQUAL
%token <token> OPEN_BRACE
%token <token> OPEN_BRACKET
%token <token> OPEN_PARENTHESIS
%token <token> OR
%token <token> RETURN
%token <token> SEMICOLON
%token <token> SUB
%token <token> VOID
%token <token> WHILE

%token <token> UNKNOWN

/** Non-terminals. */
%type <bloque> Bloque
%type <constante> Constante
%type <constanteCaracter> ConstanteCaracter
%type <constanteEntera> ConstanteEntera
%type <declaracion> Declaracion
%type <declaracionLista> DeclaracionLista
%type <declaracionSufijo> DeclaracionSufijo
%type <expression> Expresion
%type <expression> ExpresionAditiva
%type <expression> ExpresionAnd
%type <expression> ExpresionAsignacion
%type <expression> ExpresionIgualdad
%type <expression> ExpresionLvalue
%type <expression> ExpresionMultiplicativa
%type <expression> ExpresionMultiplicativaSufijo
%type <expression> ExpresionOpt
%type <expression> ExpresionOr
%type <expression> ExpresionPrimaria
%type <expression> ExpresionRelacional
%type <expression> ExpresionUnaria
%type <funcionSufijo> FuncionSufijo
%type <identificador> Identificador
%type <identificadorSufijo> IdentificadorSufijo
%type <listaArgumentos> Argumentos
%type <listaArgumentos> ListaArgumentos
%type <parametro> Parametro
%type <parametroArray> ParametroArray
%type <parametroLista> ParametroLista
%type <parametros> Parametros
%type <program> Programa
%type <sentencia> Sentencia
%type <sentencias> Sentencias
%type <sentenciaExpresion> SentenciaExpresion
%type <sentenciaFor> SentenciaFor
%type <sentenciaIf> SentenciaIf
%type <sentenciaReturn> SentenciaReturn
%type <sentenciaWhile> SentenciaWhile
%type <dataType> Tipo
%type <variableSufijo> VariableSufijo


%%

Programa: DeclaracionLista                                             { $$ = DeclaracionListaProgramSemanticAction(currentCompilerState(), $1); }
	| %empty                                                           { $$ = EmptyProgramSemanticAction(currentCompilerState()); }
	;

DeclaracionLista: Declaracion                                          { $$ = SingleDeclarationListSemanticAction($1); }
	| DeclaracionLista Declaracion                                     { $$ = AppendDeclarationListSemanticAction($1, $2); }
	;

Declaracion: Tipo Identificador DeclaracionSufijo                      { $$ = RegularDeclarationSemanticAction($1, $2, $3); }
	| EXTERN Tipo Identificador DeclaracionSufijo                      { $$ = ExternDeclarationSemanticAction($2, $3, $4); }
	;

DeclaracionSufijo: VariableSufijo SEMICOLON                            { $$ = VariableDeclaracionSufijoSemanticAction($1); }
	| OPEN_PARENTHESIS Parametros CLOSE_PARENTHESIS FuncionSufijo      { $$ = FunctionDeclaracionSufijoSemanticAction($2, $4); }
	;

FuncionSufijo: SEMICOLON                                               { $$ = EmptyFuncionSufijoSemanticAction(); }
	| Bloque                                                           { $$ = BlockFuncionSufijoSemanticAction($1); }
	;

VariableSufijo: %empty                                                 { $$ = EmptyVariableSufijoSemanticAction(); }
	| ASSIGN Expresion                                                 { $$ = AssignmentVariableSufijoSemanticAction($2); }
	| OPEN_BRACKET INTEGER CLOSE_BRACKET                               { $$ = ArrayVariableSufijoSemanticAction($2); }
	;

Parametros: VOID                                                       { $$ = VoidParametrosSemanticAction(); }
	| ParametroLista                                                   { $$ = ListParametrosSemanticAction($1); }
	| %empty                                                           { $$ = EmptyParametrosSemanticAction(); }
	;

ParametroLista: Parametro                                              { $$ = SingleParametroListaSemanticAction($1); }
	| Parametro COMMA ParametroLista                                   { $$ = AppendParametroListaSemanticAction($1, $3); }
	;

Parametro: Tipo Identificador ParametroArray                           { $$ = ParametroSemanticAction($1, $2, $3); }
	;

ParametroArray: OPEN_BRACKET CLOSE_BRACKET                             { $$ = ArrayParametroArraySemanticAction(); }
	| %empty                                                           { $$ = EmptyParametroArraySemanticAction(); }
	;

Tipo: INT                                                              { $$ = TYPE_INT; }
	| CHAR                                                             { $$ = TYPE_CHAR; }
	;

Bloque: OPEN_BRACE Sentencias CLOSE_BRACE                              { $$ = BloqueSemanticAction($2); }
	;

Sentencias: Sentencia Sentencias                                       { $$ = AppendSentenciasSemanticAction($1, $2); }
	| %empty                                                           { $$ = EmptySentenciasSemanticAction(); }
	;

Sentencia: Tipo Identificador VariableSufijo SEMICOLON                 { $$ = DeclarationSentenciaSemanticAction($1, $2, $3); }
	| SentenciaIf                                                      { $$ = IfSentenciaSemanticAction($1); }
	| SentenciaWhile                                                   { $$ = WhileSentenciaSemanticAction($1); }
	| SentenciaFor                                                     { $$ = ForSentenciaSemanticAction($1); }
	| SentenciaReturn                                                  { $$ = ReturnSentenciaSemanticAction($1); }
	| SentenciaExpresion                                               { $$ = ExpressionSentenciaSemanticAction($1); }
	| Bloque                                                           { $$ = BlockSentenciaSemanticAction($1); }
	| SEMICOLON                                                        { $$ = EmptySentenciaSemanticAction(); }
	;

SentenciaExpresion: Expresion SEMICOLON                                { $$ = SentenciaExpresionSemanticAction($1); }
	;

SentenciaIf: IF OPEN_PARENTHESIS Expresion CLOSE_PARENTHESIS Bloque    { $$ = SimpleSentenciaIfSemanticAction($3, $5); }
	| IF OPEN_PARENTHESIS Expresion CLOSE_PARENTHESIS Bloque ELSE Bloque { $$ = WithElseSentenciaIfSemanticAction($3, $5, $7); }
	;

SentenciaWhile: WHILE OPEN_PARENTHESIS Expresion CLOSE_PARENTHESIS Bloque { $$ = SentenciaWhileSemanticAction($3, $5); }
	;

SentenciaFor: FOR OPEN_PARENTHESIS ExpresionOpt SEMICOLON ExpresionOpt SEMICOLON ExpresionOpt CLOSE_PARENTHESIS Bloque { $$ = SentenciaForSemanticAction($3, $5, $7, $9); }
	;

ExpresionOpt: Expresion                                                { $$ = $1; }
	| %empty                                                           { $$ = NULL; }
	;

SentenciaReturn: RETURN ExpresionOpt SEMICOLON                         { $$ = SentenciaReturnSemanticAction($2); }
	;

Expresion: ExpresionAsignacion                                         { $$ = $1; }
	;

ExpresionAsignacion: ExpresionOr                                       { $$ = $1; }
	| ExpresionLvalue ASSIGN ExpresionAsignacion                       { $$ = AssignmentExpressionSemanticAction($1, $3); }
	;

ExpresionOr: ExpresionAnd                                              { $$ = $1; }
	| ExpresionOr OR ExpresionAnd                                      { $$ = OrExpressionSemanticAction($1, $3); }
	;

ExpresionAnd: ExpresionIgualdad                                        { $$ = $1; }
	| ExpresionAnd AND ExpresionIgualdad                               { $$ = AndExpressionSemanticAction($1, $3); }
	;

ExpresionIgualdad: ExpresionRelacional                                 { $$ = $1; }
	| ExpresionIgualdad EQUAL ExpresionRelacional                      { $$ = EqualExpressionSemanticAction($1, $3); }
	| ExpresionIgualdad NOT_EQUAL ExpresionRelacional                  { $$ = NotEqualExpressionSemanticAction($1, $3); }
	;

ExpresionRelacional: ExpresionAditiva                                  { $$ = $1; }
	| ExpresionRelacional LESS ExpresionAditiva                        { $$ = LessExpressionSemanticAction($1, $3); }
	| ExpresionRelacional GREATER ExpresionAditiva                     { $$ = GreaterExpressionSemanticAction($1, $3); }
	| ExpresionRelacional LESS_EQUAL ExpresionAditiva                  { $$ = LessEqualExpressionSemanticAction($1, $3); }
	| ExpresionRelacional GREATER_EQUAL ExpresionAditiva               { $$ = GreaterEqualExpressionSemanticAction($1, $3); }
	;

ExpresionAditiva: ExpresionMultiplicativa                              { $$ = $1; }
	| ExpresionAditiva ADD ExpresionMultiplicativa                     { $$ = AdditionExpressionSemanticAction($1, $3); }
	| ExpresionAditiva SUB ExpresionMultiplicativa                     { $$ = SubtractionExpressionSemanticAction($1, $3); }
	;

ExpresionMultiplicativa: ExpresionUnaria                              { $$ = $1; }
| ExpresionMultiplicativa ExpresionMultiplicativaSufijo               { $$ = $2; $2->leftExpression = $1; }
;

ExpresionMultiplicativaSufijo: MUL ExpresionUnaria                    { $$ = MultiplicationExpressionSemanticAction(NULL, $2); }
    | DIV ExpresionUnaria                                             { $$ = DivisionExpressionSemanticAction(NULL, $2); }
    | MOD ExpresionUnaria                                             { $$ = ModuloExpressionSemanticAction(NULL, $2); }
    ;

ExpresionUnaria: ExpresionPrimaria                                     { $$ = $1; }
	| NOT ExpresionUnaria                                              { $$ = NotExpressionSemanticAction($2); }
	;

ExpresionPrimaria: Identificador IdentificadorSufijo                   { $$ = IdentifierExpressionSemanticAction($1, $2); }
	| Constante                                                        { $$ = ConstantExpressionSemanticAction($1); }
	| OPEN_PARENTHESIS Expresion CLOSE_PARENTHESIS                     { $$ = ParenthesisExpressionSemanticAction($2); }
	;

IdentificadorSufijo: OPEN_PARENTHESIS Argumentos CLOSE_PARENTHESIS     { $$ = FunctionCallIdentificadorSufijoSemanticAction($2); }
	| OPEN_BRACKET Expresion CLOSE_BRACKET                             { $$ = ArrayAccessIdentificadorSufijoSemanticAction($2); }
	| %empty                                                           { $$ = EmptyIdentificadorSufijoSemanticAction(); }
	;

ExpresionLvalue: Identificador                                         { $$ = SimpleExpresionLvalueSemanticAction($1); }
	| Identificador OPEN_BRACKET Expresion CLOSE_BRACKET               { $$ = ArrayExpresionLvalueSemanticAction($1, $3); }
	;

Argumentos: ListaArgumentos                                            { $$ = $1; }
	| %empty                                                           { $$ = NULL; }
	;

ListaArgumentos: Expresion                                             { $$ = SingleListaArgumentosSemanticAction($1); }
	| ListaArgumentos COMMA Expresion                                  { $$ = AppendListaArgumentosSemanticAction($1, $3); }
	;

Constante: ConstanteEntera                                             { $$ = IntegerConstanteSemanticAction($1); }
	| ConstanteCaracter                                                { $$ = CharacterConstanteSemanticAction($1); }
	;

ConstanteEntera: INTEGER                                               { $$ = ConstanteEnteraSemanticAction($1); }
	;

ConstanteCaracter: CHARACTER                                           { $$ = ConstanteCaracterSemanticAction($1); }
	;

Identificador: IDENTIFIER                                              { $$ = IdentificadorSemanticAction($1); }
	;

%%