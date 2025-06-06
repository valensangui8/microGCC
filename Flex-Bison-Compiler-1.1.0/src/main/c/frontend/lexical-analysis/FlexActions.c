#include "FlexActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;
static boolean _logIgnoredLexemes = true;

void initializeFlexActionsModule() {
    _logIgnoredLexemes = getBooleanOrDefault("LOG_IGNORED_LEXEMES", _logIgnoredLexemes);
    _logger = createLogger("FlexActions");
}

void shutdownFlexActionsModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
    }
}

/* PRIVATE FUNCTIONS */

static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext);

/**
 * Logs a lexical-analyzer context in DEBUGGING level.
 */
static void _logLexicalAnalyzerContext(const char * functionName, LexicalAnalyzerContext * lexicalAnalyzerContext) {
    char * escapedLexeme = escape(lexicalAnalyzerContext->lexeme);
    logDebugging(_logger, "%s: %s (context = %d, length = %d, line = %d)",
        functionName,
        escapedLexeme,
        lexicalAnalyzerContext->currentContext,
        lexicalAnalyzerContext->length,
        lexicalAnalyzerContext->line);
    free(escapedLexeme);
}

/* PUBLIC FUNCTIONS */

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
    if (_logIgnoredLexemes) {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
}

void EndMultilineCommentLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
    if (_logIgnoredLexemes) {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
}

void IgnoredLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
    if (_logIgnoredLexemes) {
        _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    }
}

Token KeywordLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->token = token;
    return token;
}

Token OperatorLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->token = token;
    return token;
}

Token PunctuationLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, Token token) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->token = token;
    return token;
}

Token IntegerLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->integer = atoi(lexicalAnalyzerContext->lexeme);
    return INTEGER;
}

Token CharacterLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->character = lexicalAnalyzerContext->lexeme[1];
    return CHARACTER;
}

Token EscapeCharacterLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext, char character) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->character = character;
    return CHARACTER;
}

Token IdentifierLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    lexicalAnalyzerContext->semanticValue->string = strdup(lexicalAnalyzerContext->lexeme);
    return IDENTIFIER;
}

Token UnknownLexemeAction(LexicalAnalyzerContext * lexicalAnalyzerContext) {
    _logLexicalAnalyzerContext(__FUNCTION__, lexicalAnalyzerContext);
    return UNKNOWN;
}