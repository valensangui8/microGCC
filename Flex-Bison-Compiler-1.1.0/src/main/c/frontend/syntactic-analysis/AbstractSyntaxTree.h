#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

typedef enum {
    TYPE_INT,
    TYPE_CHAR
} DataType;

typedef enum {
    DECLARATION_REGULAR,
    DECLARATION_EXTERN
} DeclarationType;

typedef enum {
    DECLARATION_SUFFIX_VARIABLE,
    DECLARATION_SUFFIX_FUNCTION
} DeclarationSuffixType;

typedef enum {
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
} ExpressionType;

typedef enum {
    IDENTIFIER_SUFFIX_NONE,
    IDENTIFIER_SUFFIX_FUNCTION_CALL,
    IDENTIFIER_SUFFIX_ARRAY_ACCESS
} IdentifierSuffixType;

typedef enum {
    PARAMETER_ARRAY_NONE,
    PARAMETER_ARRAY_BRACKETS
} ParameterArrayType;

typedef enum {
    PROGRAM_EMPTY,
    PROGRAM_DECLARATIONS
} ProgramType;

typedef enum {
    STATEMENT_DECLARATION,
    STATEMENT_IF,
    STATEMENT_WHILE,
    STATEMENT_FOR,
    STATEMENT_RETURN,
    STATEMENT_EXPRESSION,
    STATEMENT_BLOCK,
    STATEMENT_EMPTY
} StatementType;

typedef enum {
    VARIABLE_SUFFIX_NONE,
    VARIABLE_SUFFIX_ASSIGNMENT,
    VARIABLE_SUFFIX_ARRAY
} VariableSuffixType;

typedef struct Block Block;
typedef struct Constant Constant;
typedef struct ConstantCharacter ConstantCharacter;
typedef struct ConstantInteger ConstantInteger;
typedef struct Declaration Declaration;
typedef struct DeclarationList DeclarationList;
typedef struct DeclarationSuffix DeclarationSuffix;
typedef struct Expression Expression;
typedef struct FunctionSuffix FunctionSuffix;
typedef struct Identifier Identifier;
typedef struct IdentifierSuffix IdentifierSuffix;
typedef struct ListArguments ListArguments;
typedef struct Parameter Parameter;
typedef struct ParameterArray ParameterArray;
typedef struct ParameterList ParameterList;
typedef struct Parameters Parameters;
typedef struct Program Program;
typedef struct Statement Statement;
typedef struct Statements Statements;
typedef struct StatementExpression StatementExpression;
typedef struct StatementFor StatementFor;
typedef struct StatementIf StatementIf;
typedef struct StatementReturn StatementReturn;
typedef struct StatementWhile StatementWhile;
typedef struct VariableSuffix VariableSuffix;

struct ConstantInteger {
    int value;
};

struct ConstantCharacter {
    char value;
};

struct Constant {
    int type;
    union {
        ConstantInteger* integer;
        ConstantCharacter* character;
    };
};

struct Identifier {
    char* name;
};

struct Expression {
    ExpressionType type;
    union {
        struct { Expression* leftExpression; Expression* rightExpression; };
        Expression* singleExpression;
        Identifier* identifier;
        Constant* constant;
        struct { Identifier* identifierArray; Expression* indexExpression; };
        struct { Identifier* identifierFunc; ListArguments* arguments; };
    };
};

struct ListArguments {
    Expression* expression;
    ListArguments* next;
};

struct IdentifierSuffix {
    IdentifierSuffixType type;
    union {
        ListArguments* arguments;
        Expression* indexExpression;
    };
};

struct VariableSuffix {
    VariableSuffixType type;
    union {
        Expression* expression;
        ConstantInteger* arraySize;
    };
};

struct ParameterArray {
    ParameterArrayType type;
};

struct Parameter {
    DataType type;
    Identifier* identifier;
    ParameterArray* array;
};

struct ParameterList {
    Parameter* parameter;
    ParameterList* next;
};

struct Parameters {
    int type;
    ParameterList* list;
};

struct FunctionSuffix {
    int type;
    Block* block;
};

struct DeclarationSuffix {
    DeclarationSuffixType type;
    union {
        VariableSuffix* variableSuffix;
        struct {
            Parameters* parameters;
            FunctionSuffix* functionSuffix;
        };
    };
};

struct Declaration {
    DataType dataType;
    Identifier* identifier;
    DeclarationSuffix* declarationSuffix;
    DeclarationType declarationType;
};

struct DeclarationList {
    Declaration* declaration;
    DeclarationList* next;
};

struct StatementExpression {
    Expression* expression;
};

struct StatementIf {
    Expression* condition;
    Block* thenBlock;
    Block* elseBlock;
    int hasElse;
};

struct StatementWhile {
    Expression* condition;
    Block* block;
};

struct StatementFor {
    Expression* init;
    int hasInit;
    Expression* condition;
    int hasCondition;
    Expression* update;
    int hasUpdate;
    Block* block;
};

struct StatementReturn {
    Expression* expression;
    int hasExpression;
};

struct Statement {
    StatementType type;
    union {
        struct {
            DataType dataType;
            Identifier* identifier;
            VariableSuffix* variableSuffix;
        };
        StatementIf* statementIf;
        StatementWhile* statementWhile;
        StatementFor* statementFor;
        StatementReturn* statementReturn;
        StatementExpression* statementExpression;
        Block* block;
    };
};

struct Statements {
    Statement* statement;
    Statements* next;
};

struct Block {
    Statements* statements;
};

struct Program {
    ProgramType type;
    union {
        DeclarationList* declarationList;
    };
};

#endif
