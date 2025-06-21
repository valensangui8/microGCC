
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
	Block* block;
	Constant* constant;
	ConstantCharacter* constantCharacter;
	ConstantInteger* constantInteger;
	Declaration* declaration;
	DeclarationList* declarationList;
	DeclarationSuffix* declarationSuffix;
	Expression* expression;
	FunctionSuffix* functionSuffix;
	Identifier* identifier;
	IdentifierSuffix* identifierSuffix;
	ListArguments* listArguments;
	Parameter* parameter;
	ParameterArray * parameterArray;
	ParameterList* parameterList;
	Parameters* parameters;
	Program* program;
	Statement* statement;
	Statements* statements;
	StatementExpression* statementExpression;
	StatementFor* statementFor;
	StatementIf* statementIf;
	StatementReturn* statementReturn;
	StatementWhile* statementWhile;
	VariableSuffix* variableSuffix;
	VariableSuffixPrima* variableSuffixPrima;
	ConstantExpression * constantExpression;

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
%type <block> Block
%type <constant> Constant
%type <constantCharacter> ConstantCharacter
%type <constantInteger> ConstantInteger
%type <declaration> Declaration
%type <declarationList> DeclarationList
%type <declarationSuffix> DeclarationSuffix
%type <expression> Expression
%type <expression> ExpressionAditiva
%type <expression> ExpressionAnd
%type <expression> ExpressionAsignacion
%type <expression> ExpressionIgualdad
%type <expression> ExpressionLvalue
%type <expression> ExpressionMultiplicativa
%type <expression> ExpressionOpt
%type <expression> ExpressionOr
%type <expression> ExpressionPrimaria
%type <expression> ExpressionRelacional
%type <expression> ExpressionUnaria
%type <functionSuffix> FunctionSuffix
%type <identifier> Identifier
%type <identifierSuffix> IdentifierSuffix
%type <listArguments> Arguments
%type <listArguments> ListArguments
%type <parameter> Parameter
%type <parameterArray> ParameterArray
%type <parameterList> ParameterList
%type <parameters> Parameters
%type <program> Programa
%type <statement> Statement
%type <statements> Statements
%type <statementExpression> StatementExpression
%type <statementFor> StatementFor
%type <statementIf> StatementIf
%type <statementReturn> StatementReturn
%type <statementWhile> StatementWhile
%type <dataType> Type
%type <variableSuffix> VariableSuffix
%type <variableSuffixPrima> VariableSuffixPrima
%type <constantExpression> ConstantExpression



%%

Programa: DeclarationList                                             { $$ = DeclarationListProgramSemanticAction(currentCompilerState(), $1); }
	| %empty                                                           { $$ = EmptyProgramSemanticAction(currentCompilerState()); }
	;

DeclarationList: Declaration                                          { $$ = SingleDeclarationListSemanticAction($1); }
	| DeclarationList Declaration                                     { $$ = AppendDeclarationListSemanticAction($1, $2); }
	;

Declaration: Type Identifier DeclarationSuffix                      { $$ = RegularDeclarationSemanticAction($1, $2, $3); }
	| EXTERN Type Identifier DeclarationSuffix                      { $$ = ExternDeclarationSemanticAction($2, $3, $4); }
	;

DeclarationSuffix: VariableSuffixPrima SEMICOLON                            { $$ = VariableDeclarationSuffixSemanticAction($1); }
	| OPEN_PARENTHESIS Parameters CLOSE_PARENTHESIS FunctionSuffix      { $$ = FunctionDeclarationSuffixSemanticAction($2, $4); }
	;

FunctionSuffix: SEMICOLON                                               { $$ = EmptyFunctionSuffixSemanticAction(); }
	| Block                                                           { $$ = BlockFunctionSuffixSemanticAction($1); }
	;

VariableSuffix: %empty                                                 { $$ = EmptyVariableSuffixSemanticAction(); }
	| ASSIGN Expression                                                 { $$ = AssignmentVariableSuffixSemanticAction($2); }
	| OPEN_BRACKET INTEGER CLOSE_BRACKET                               { $$ = ArrayVariableSuffixSemanticAction($2); }
	;

VariableSuffixPrima: %empty                                                 { $$ = EmptyVariableSuffixSemanticAction(); }
	| ASSIGN ConstantExpression                                                 { $$ = AssignmentVariableSuffixSemanticAction($2); }
	| OPEN_BRACKET INTEGER CLOSE_BRACKET                               { $$ = ArrayVariableSuffixSemanticAction($2); }
	;


ConstantExpression:
    Constant                                                            {$$ = ConstantExpressionSemanticAction($1);}
;

Parameters: VOID                                                       { $$ = VoidParametersSemanticAction(); }
	| ParameterList                                                   { $$ = ListParametersSemanticAction($1); }
	| %empty                                                           { $$ = EmptyParametersSemanticAction(); }
	;

ParameterList: Parameter                                              { $$ = SingleParameterListSemanticAction($1); }
	| Parameter COMMA ParameterList                                   { $$ = AppendParameterListSemanticAction($1, $3); }
	;

Parameter: Type Identifier ParameterArray                           { $$ = ParameterSemanticAction($1, $2, $3); }
	;

ParameterArray: OPEN_BRACKET CLOSE_BRACKET                             { $$ = ArrayParameterArraySemanticAction(); }
	| %empty                                                           { $$ = EmptyParameterArraySemanticAction(); }
	;

Type: INT                                                              { $$ = TYPE_INT; }
	| CHAR                                                             { $$ = TYPE_CHAR; }
	;

Block: OPEN_BRACE Statements CLOSE_BRACE                              { $$ = BlockSemanticAction($2); }
	;

Statements: Statement Statements                                       { $$ = AppendStatementsSemanticAction($1, $2); }
	| %empty                                                           { $$ = EmptyStatementsSemanticAction(); }
	;

Statement: Type Identifier VariableSuffix SEMICOLON                 { $$ = DeclarationStatementSemanticAction($1, $2, $3); }
	| StatementIf                                                      { $$ = IfStatementSemanticAction($1); }
	| StatementWhile                                                   { $$ = WhileStatementSemanticAction($1); }
	| StatementFor                                                     { $$ = ForStatementSemanticAction($1); }
	| StatementReturn                                                  { $$ = ReturnStatementSemanticAction($1); }
	| StatementExpression                                               { $$ = ExpressionStatementSemanticAction($1); }
	| Block                                                           { $$ = BlockStatementSemanticAction($1); }
	| SEMICOLON                                                        { $$ = EmptyStatementSemanticAction(); }
	;

StatementExpression: Expression SEMICOLON                                { $$ = StatementExpressionSemanticAction($1); }
	;

StatementIf: IF OPEN_PARENTHESIS Expression CLOSE_PARENTHESIS Block    { $$ = SimpleStatementIfSemanticAction($3, $5); }
	| IF OPEN_PARENTHESIS Expression CLOSE_PARENTHESIS Block ELSE Block { $$ = WithElseStatementIfSemanticAction($3, $5, $7); }
	;

StatementWhile: WHILE OPEN_PARENTHESIS Expression CLOSE_PARENTHESIS Block { $$ = StatementWhileSemanticAction($3, $5); }
	;

StatementFor: FOR OPEN_PARENTHESIS ExpressionOpt SEMICOLON ExpressionOpt SEMICOLON ExpressionOpt CLOSE_PARENTHESIS Block { $$ = StatementForSemanticAction($3, $5, $7, $9); }
	;

ExpressionOpt: Expression                                                { $$ = $1; }
	| %empty                                                           { $$ = NULL; }
	;

StatementReturn: RETURN ExpressionOpt SEMICOLON                         { $$ = StatementReturnSemanticAction($2); }
	;

Expression: ExpressionAsignacion                                         { $$ = $1; }
	;

ExpressionAsignacion: ExpressionOr                                       { $$ = $1; }
	| ExpressionLvalue ASSIGN ExpressionAsignacion                       { $$ = AssignmentExpressionSemanticAction($1, $3); }
	;

ExpressionOr: ExpressionAnd                                              { $$ = $1; }
	| ExpressionOr OR ExpressionAnd                                      { $$ = OrExpressionSemanticAction($1, $3); }
	;

ExpressionAnd: ExpressionIgualdad                                        { $$ = $1; }
	| ExpressionAnd AND ExpressionIgualdad                               { $$ = AndExpressionSemanticAction($1, $3); }
	;

ExpressionIgualdad: ExpressionRelacional                                 { $$ = $1; }
	| ExpressionIgualdad EQUAL ExpressionRelacional                      { $$ = EqualExpressionSemanticAction($1, $3); }
	| ExpressionIgualdad NOT_EQUAL ExpressionRelacional                  { $$ = NotEqualExpressionSemanticAction($1, $3); }
	;

ExpressionRelacional: ExpressionAditiva                                  { $$ = $1; }
	| ExpressionRelacional LESS ExpressionAditiva                        { $$ = LessExpressionSemanticAction($1, $3); }
	| ExpressionRelacional GREATER ExpressionAditiva                     { $$ = GreaterExpressionSemanticAction($1, $3); }
	| ExpressionRelacional LESS_EQUAL ExpressionAditiva                  { $$ = LessEqualExpressionSemanticAction($1, $3); }
	| ExpressionRelacional GREATER_EQUAL ExpressionAditiva               { $$ = GreaterEqualExpressionSemanticAction($1, $3); }
	;

ExpressionAditiva: ExpressionMultiplicativa                              { $$ = $1; }
	| ExpressionAditiva ADD ExpressionMultiplicativa                     { $$ = AdditionExpressionSemanticAction($1, $3); }
	| ExpressionAditiva SUB ExpressionMultiplicativa                     { $$ = SubtractionExpressionSemanticAction($1, $3); }
	;


ExpressionMultiplicativa
    : ExpressionMultiplicativa MUL ExpressionUnaria   { $$ = MultiplicationExpressionSemanticAction($1, $3); }
    | ExpressionMultiplicativa DIV ExpressionUnaria   { $$ = DivisionExpressionSemanticAction($1, $3); }
    | ExpressionMultiplicativa MOD ExpressionUnaria   { $$ = ModuloExpressionSemanticAction($1, $3); }
    | ExpressionUnaria                                { $$ = $1; }
    ;



ExpressionUnaria: ExpressionPrimaria                                     { $$ = $1; }
	| NOT ExpressionUnaria                                              { $$ = NotExpressionSemanticAction($2); }
	;

ExpressionPrimaria: Identifier IdentifierSuffix                   { $$ = IdentifierExpressionSemanticAction($1, $2); }
	| Constant                                                        { $$ = ConstantExpressionSemanticAction($1); }
	| OPEN_PARENTHESIS Expression CLOSE_PARENTHESIS                     { $$ = ParenthesisExpressionSemanticAction($2); }
	;

IdentifierSuffix: OPEN_PARENTHESIS Arguments CLOSE_PARENTHESIS     { $$ = FunctionCallIdentifierSuffixSemanticAction($2); }
	| OPEN_BRACKET Expression CLOSE_BRACKET                             { $$ = ArrayAccessIdentifierSuffixSemanticAction($2); }
	| %empty                                                           { $$ = EmptyIdentifierSuffixSemanticAction(); }
	;

ExpressionLvalue: Identifier                                         { $$ = SimpleExpressionLvalueSemanticAction($1); }
	| Identifier OPEN_BRACKET Expression CLOSE_BRACKET               { $$ = ArrayExpressionLvalueSemanticAction($1, $3); }
	;

Arguments: ListArguments                                            { $$ = $1; }
	| %empty                                                           { $$ = NULL; }
	;

ListArguments: Expression                                             { $$ = SingleListArgumentsSemanticAction($1); }
	| ListArguments COMMA Expression                                  { $$ = AppendListArgumentsSemanticAction($1, $3); }
	;


Constant: ConstantInteger                                             { $$ = IntegerConstantSemanticAction($1); }
	| ConstantCharacter                                                { $$ = CharacterConstantSemanticAction($1); }
	;

ConstantInteger: INTEGER                                               { $$ = ConstantIntegerSemanticAction($1); }
	;

ConstantCharacter: CHARACTER                                           { $$ = ConstantCharacterSemanticAction($1); }
	;

Identifier: IDENTIFIER                                                  { $$ = IdentifierSemanticAction($1); }
	;

%%