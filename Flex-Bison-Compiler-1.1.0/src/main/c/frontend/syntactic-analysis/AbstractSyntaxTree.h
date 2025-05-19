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
typedef enum DeclarationSuffixType DeclarationSuffixType;
typedef enum ExpressionType ExpressionType;
typedef enum IdentifierSuffixType IdentifierSuffixType;
typedef enum ParameterArrayType ParameterArrayType;
typedef enum ProgramType ProgramType;
typedef enum StatementType StatementType;
typedef enum StatementType StatementType;
typedef enum VariableSuffixType VariableSuffixType;

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

enum DeclarationSuffixType {
    VARIABLE_SUFFIX_DECLARATION,
    DECLARATION_SUFFIX_FUNCTION
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

enum IdentifierSuffixType {
    IDENTIFIER_SUFFIX_NONE,
    IDENTIFIER_SUFFIX_FUNCTION_CALL,
    IDENTIFIER_SUFFIX_ARRAY_ACCESS
};

enum ParameterArrayType {
    PARAMETER_ARRAY_NONE,
    PARAMETER_ARRAY_BRACKETS
};

enum ProgramType {
    PROGRAM_EMPTY,
    PROGRAM_DECLARATIONS
};

enum StatementType {
    STATEMENT_DECLARATION,
    STATEMENT_IF,
    STATEMENT_WHILE,
    STATEMENT_FOR,
    STATEMENT_RETURN,
    STATEMENT_EXPRESSION,
    STATEMENT_BLOCK,
    STATEMENT_EMPTY
};

enum VariableSuffixType {
    VARIABLE_SUFFIX_NONE,
    VARIABLE_SUFFIX_ASSIGNMENT,
    VARIABLE_SUFFIX_ARRAY
};

struct ConstantInteger {
    int value;
};

struct ConstantCharacter {
    char value;
};

struct Constant {
    union {
        ConstantInteger* integer;
        ConstantCharacter* character;
    };
    int type; // 0 for integer, 1 for character
};

struct Identifier {
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
        Identifier* identifier;
        Constant* constant;
        struct {
            Identifier* identifierArray;
            Expression* indexExpression;
        };
        struct {
            Identifier* identifierFunc;
            ListArguments* arguments;
        };
    };
    ExpressionType type;
};

struct ListArguments {
    Expression* expression;
    ListArguments* next;
};

struct IdentifierSuffix {
    union {
        ListArguments* arguments;
        Expression* indexExpression;
    };
    IdentifierSuffixType type;
};

struct VariableSuffix {
    union {
        Expression* expression;
        ConstantInteger* arraySize;
    };
    VariableSuffixType type;
};

struct Parameter {
    DataType type;
    Identifier* identifier;
    ParameterArray* array;
};

struct ParameterArray {
    ParameterArrayType type;
};

struct ParameterList {
    Parameter* parameter;
    ParameterList* next;
};

struct Parameters {
    union {
        ParameterList* list;
    };
    int type; // 0 for void, 1 for list, 2 for empty
};

struct Block {
    Statements* statements;
};

struct FunctionSuffix {
    union {
        Block* block;
    };
    int type; // 0 for ;, 1 for block
};

struct DeclarationSuffix {
    union {
        VariableSuffix* variableSuffix;
        struct {
            Parameters* parameters;
            FunctionSuffix* functionSuffix;
        };
    };
    DeclarationSuffixType type;
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
    StatementType type;
};

struct Statements {
    Statement* statement;
    Statements* next;
};

struct Program {
    union {
        DeclarationList* declarationList;
    };
    ProgramType type;
};

/**
 * Node recursive destructors.
 */
void releaseBlock(Block* block);
void releaseConstant(Constant* constant);
void releaseConstantCharacter(ConstantCharacter* constantCharacter);
void releaseConstantInteger(ConstantInteger* constantInteger);
void releaseDeclaration(Declaration* declaration);
void releaseDeclarationList(DeclarationList* declarationList);
void releaseDeclarationSuffix(DeclarationSuffix* declarationSuffix);
void releaseExpression(Expression* expression);
void releaseFunctionSuffix(FunctionSuffix* functionSuffix);
void releaseIdentifier(Identifier* identifier);
void releaseIdentifierSuffix(IdentifierSuffix* identifierSuffix);
void releaseListArguments(ListArguments* listArguments);
void releaseParameter(Parameter* parameter);
void releaseParameterArray(ParameterArray* parameterArray);
void releaseParameterList(ParameterList* parameterList);
void releaseParameters(Parameters* parameters);
void releaseProgram(Program* program);
void releaseStatement(Statement* statement);
void releaseStatements(Statements* statements);
void releaseStatementExpression(StatementExpression* statementExpression);
void releaseStatementFor(StatementFor* statementFor);
void releaseStatementIf(StatementIf* statementIf);
void releaseStatementReturn(StatementReturn* statementReturn);
void releaseStatementWhile(StatementWhile* statementWhile);
void releaseVariableSuffix(VariableSuffix* variableSuffix);

#endif