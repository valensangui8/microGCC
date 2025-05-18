#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef enum DataType DataType;
typedef enum DeclarationType DeclarationType;
typedef enum DeclaracionSufijoType DeclaracionSufijoType;
typedef enum ExpressionType ExpressionType;
typedef enum IdentificadorSufijoType IdentificadorSufijoType;
typedef enum ParametroArrayType ParametroArrayType;
typedef enum ProgramType ProgramType;
typedef enum SentenciaType SentenciaType;
typedef enum StatementType StatementType;
typedef enum VariableSufijoType VariableSufijoType;

typedef struct Bloque Bloque;
typedef struct Constante Constante;
typedef struct ConstanteCaracter ConstanteCaracter;
typedef struct ConstanteEntera ConstanteEntera;
typedef struct Declaracion Declaracion;
typedef struct DeclaracionLista DeclaracionLista;
typedef struct DeclaracionSufijo DeclaracionSufijo;
typedef struct Expression Expression;
typedef struct FuncionSufijo FuncionSufijo;
typedef struct Identificador Identificador;
typedef struct IdentificadorSufijo IdentificadorSufijo;
typedef struct ListaArgumentos ListaArgumentos;
typedef struct Parametro Parametro;
typedef struct ParametroArray ParametroArray;
typedef struct ParametroLista ParametroLista;
typedef struct Parametros Parametros;
typedef struct Program Program;
typedef struct Sentencia Sentencia;
typedef struct Sentencias Sentencias;
typedef struct SentenciaExpresion SentenciaExpresion;
typedef struct SentenciaFor SentenciaFor;
typedef struct SentenciaIf SentenciaIf;
typedef struct SentenciaReturn SentenciaReturn;
typedef struct SentenciaWhile SentenciaWhile;
typedef struct VariableSufijo VariableSufijo;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum DataType {
    TYPE_INT,
    TYPE_CHAR
};

enum DeclarationType {
    DECLARATION_REGULAR,
    DECLARATION_EXTERN
};

enum DeclaracionSufijoType {
    DECLARACION_SUFIJO_VARIABLE,
    DECLARACION_SUFIJO_FUNCTION
};

enum ExpressionType {
    EXPRESSION_ASSIGNMENT,
    EXPRESSION_OR,
    EXPRESSION_AND,
    EXPRESSION_EQUAL,
    EXPRESSION_NOT_EQUAL,
    EXPRESSION_LESS,
    EXPRESSION_GREATER,
    EXPRESSION_LESS_EQUAL,
    EXPRESSION_GREATER_EQUAL,
    EXPRESSION_ADDITION,
    EXPRESSION_SUBTRACTION,
    EXPRESSION_MULTIPLICATION,
    EXPRESSION_DIVISION,
    EXPRESSION_MODULO,
    EXPRESSION_NOT,
    EXPRESSION_IDENTIFIER,
    EXPRESSION_CONSTANT,
    EXPRESSION_PARENTHESIS,
    EXPRESSION_ARRAY_ACCESS,
    EXPRESSION_FUNCTION_CALL
};

enum IdentificadorSufijoType {
    IDENTIFICADOR_SUFIJO_NONE,
    IDENTIFICADOR_SUFIJO_FUNCTION_CALL,
    IDENTIFICADOR_SUFIJO_ARRAY_ACCESS
};

enum ParametroArrayType {
    PARAMETRO_ARRAY_NONE,
    PARAMETRO_ARRAY_BRACKETS
};

enum ProgramType {
    PROGRAM_EMPTY,
    PROGRAM_DECLARATIONS
};

enum SentenciaType {
    SENTENCIA_DECLARATION,
    SENTENCIA_IF,
    SENTENCIA_WHILE,
    SENTENCIA_FOR,
    SENTENCIA_RETURN,
    SENTENCIA_EXPRESSION,
    SENTENCIA_BLOQUE,
    SENTENCIA_EMPTY
};

enum VariableSufijoType {
    VARIABLE_SUFIJO_NONE,
    VARIABLE_SUFIJO_ASSIGNMENT,
    VARIABLE_SUFIJO_ARRAY
};

struct ConstanteEntera {
    int value;
};

struct ConstanteCaracter {
    char value;
};

struct Constante {
    union {
        ConstanteEntera* entera;
        ConstanteCaracter* caracter;
    };
    int type; // 0 for entera, 1 for caracter
};

struct Identificador {
    char* name;
};

struct Expression {
    union {
        struct {
            Expression* leftExpression;
            Expression* rightExpression;
        };
        struct {
            Expression* singleExpression;
        };
        Identificador* identifier;
        Constante* constant;
        struct {
            Identificador* identifierArray;
            Expression* indexExpression;
        };
        struct {
            Identificador* identifierFunc;
            ListaArgumentos* arguments;
        };
    };
    ExpressionType type;
};

struct ListaArgumentos {
    Expression* expression;
    ListaArgumentos* next;
};

struct IdentificadorSufijo {
    union {
        ListaArgumentos* arguments;
        Expression* indexExpression;
    };
    IdentificadorSufijoType type;
};

struct VariableSufijo {
    union {
        Expression* expression;
        ConstanteEntera* arraySize;
    };
    VariableSufijoType type;
};

struct Parametro {
    DataType type;
    Identificador* identifier;
    ParametroArray* array;
};

struct ParametroArray {
    ParametroArrayType type;
};

struct ParametroLista {
    Parametro* parametro;
    ParametroLista* next;
};

struct Parametros {
    union {
        ParametroLista* lista;
    };
    int type; // 0 for void, 1 for lista, 2 for empty
};

struct Bloque {
    Sentencias* sentencias;
};

struct FuncionSufijo {
    union {
        Bloque* bloque;
    };
    int type; // 0 for ;, 1 for bloque
};

struct DeclaracionSufijo {
    union {
        VariableSufijo* variableSufijo;
        struct {
            Parametros* parametros;
            FuncionSufijo* funcionSufijo;
        };
    };
    DeclaracionSufijoType type;
};

struct Declaracion {
    DataType dataType;
    Identificador* identifier;
    DeclaracionSufijo* declaracionSufijo;
    DeclarationType declarationType;
};

struct DeclaracionLista {
    Declaracion* declaracion;
    DeclaracionLista* next;
};

struct SentenciaExpresion {
    Expression* expression;
};

struct SentenciaIf {
    Expression* condition;
    Bloque* thenBloque;
    Bloque* elseBloque;
    int hasElse;
};

struct SentenciaWhile {
    Expression* condition;
    Bloque* bloque;
};

struct SentenciaFor {
    Expression* init;
    int hasInit;
    Expression* condition;
    int hasCondition;
    Expression* update;
    int hasUpdate;
    Bloque* bloque;
};

struct SentenciaReturn {
    Expression* expression;
    int hasExpression;
};

struct Sentencia {
    union {
        struct {
            DataType dataType;
            Identificador* identifier;
            VariableSufijo* variableSufijo;
        };
        SentenciaIf* sentenciaIf;
        SentenciaWhile* sentenciaWhile;
        SentenciaFor* sentenciaFor;
        SentenciaReturn* sentenciaReturn;
        SentenciaExpresion* sentenciaExpresion;
        Bloque* bloque;
    };
    SentenciaType type;
};

struct Sentencias {
    Sentencia* sentencia;
    Sentencias* next;
};

struct Program {
    union {
        DeclaracionLista* declaracionLista;
    };
    ProgramType type;
};

/**
 * Node recursive destructors.
 */
void releaseBloque(Bloque* bloque);
void releaseConstante(Constante* constante);
void releaseConstanteCaracter(ConstanteCaracter* constanteCaracter);
void releaseConstanteEntera(ConstanteEntera* constanteEntera);
void releaseDeclaracion(Declaracion* declaracion);
void releaseDeclaracionLista(DeclaracionLista* declaracionLista);
void releaseDeclaracionSufijo(DeclaracionSufijo* declaracionSufijo);
void releaseExpression(Expression* expression);
void releaseFuncionSufijo(FuncionSufijo* funcionSufijo);
void releaseIdentificador(Identificador* identificador);
void releaseIdentificadorSufijo(IdentificadorSufijo* identificadorSufijo);
void releaseListaArgumentos(ListaArgumentos* listaArgumentos);
void releaseParametro(Parametro* parametro);
void releaseParametroArray(ParametroArray* parametroArray);
void releaseParametroLista(ParametroLista* parametroLista);
void releaseParametros(Parametros* parametros);
void releaseProgram(Program* program);
void releaseSentencia(Sentencia* sentencia);
void releaseSentencias(Sentencias* sentencias);
void releaseSentenciaExpresion(SentenciaExpresion* sentenciaExpresion);
void releaseSentenciaFor(SentenciaFor* sentenciaFor);
void releaseSentenciaIf(SentenciaIf* sentenciaIf);
void releaseSentenciaReturn(SentenciaReturn* sentenciaReturn);
void releaseSentenciaWhile(SentenciaWhile* sentenciaWhile);
void releaseVariableSufijo(VariableSufijo* variableSufijo);

#endif
