
%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

DIGIT           [0-9]
LETTER          [a-zA-Z]
IDENTIFIER      {LETTER}({LETTER}|{DIGIT}|_)*

%%

"/*"								{ BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"//"[^\n]*                          { IgnoredLexemeAction(createLexicalAnalyzerContext()); }


"int"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), INT); }
"char"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), CHAR); }
"void"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), VOID); }
"if"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), IF); }
"else"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), ELSE); }
"while"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), WHILE); }
"for"								{ return KeywordLexemeAction(createLexicalAnalyzerContext(), FOR); }
"return"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), RETURN); }
"extern"							{ return KeywordLexemeAction(createLexicalAnalyzerContext(), EXTERN); }

"+"									{ return OperatorLexemeAction(createLexicalAnalyzerContext(), ADD); }
"-"									{ return OperatorLexemeAction(createLexicalAnalyzerContext(), SUB); }
"*"									{ return OperatorLexemeAction(createLexicalAnalyzerContext(), MUL); }
"/"									{ return OperatorLexemeAction(createLexicalAnalyzerContext(), DIV); }
"%"									{ return OperatorLexemeAction(createLexicalAnalyzerContext(), MOD); }
"="									{ return OperatorLexemeAction(createLexicalAnalyzerContext(), ASSIGN); }
"=="								{ return OperatorLexemeAction(createLexicalAnalyzerContext(), EQUAL); }
"!="								{ return OperatorLexemeAction(createLexicalAnalyzerContext(), NOT_EQUAL); }
"<"									{ return OperatorLexemeAction(createLexicalAnalyzerContext(), LESS); }
">"									{ return OperatorLexemeAction(createLexicalAnalyzerContext(), GREATER); }
"<="								{ return OperatorLexemeAction(createLexicalAnalyzerContext(), LESS_EQUAL); }
">="								{ return OperatorLexemeAction(createLexicalAnalyzerContext(), GREATER_EQUAL); }
"&&"								{ return OperatorLexemeAction(createLexicalAnalyzerContext(), AND); }
"||"								{ return OperatorLexemeAction(createLexicalAnalyzerContext(), OR); }
"!"									{ return OperatorLexemeAction(createLexicalAnalyzerContext(), NOT); }

"("									{ return PunctuationLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									{ return PunctuationLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }
"["									{ return PunctuationLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"									{ return PunctuationLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }
"{"									{ return PunctuationLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACE); }
"}"									{ return PunctuationLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACE); }
";"									{ return PunctuationLexemeAction(createLexicalAnalyzerContext(), SEMICOLON); }
","									{ return PunctuationLexemeAction(createLexicalAnalyzerContext(), COMMA); }

{DIGIT}+							{ return IntegerLexemeAction(createLexicalAnalyzerContext()); }
'[^\\]'								{ return CharacterLexemeAction(createLexicalAnalyzerContext()); }
'\\n'								{ return EscapeCharacterLexemeAction(createLexicalAnalyzerContext(), '\n'); }
'\\t'								{ return EscapeCharacterLexemeAction(createLexicalAnalyzerContext(), '\t'); }
'\\r'								{ return EscapeCharacterLexemeAction(createLexicalAnalyzerContext(), '\r'); }
'\\0'								{ return EscapeCharacterLexemeAction(createLexicalAnalyzerContext(), '\0'); }
'\\\\'								{ return EscapeCharacterLexemeAction(createLexicalAnalyzerContext(), '\\'); }
'\\\'                                { return EscapeCharacterLexemeAction(createLexicalAnalyzerContext(), '\''); }
'\\\"'                               { return EscapeCharacterLexemeAction(createLexicalAnalyzerContext(), '\"'); }
{IDENTIFIER}						{ return IdentifierLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"