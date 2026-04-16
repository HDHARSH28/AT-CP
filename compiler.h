#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ================================================================
   GLOBAL LIMITS
   ================================================================ */
#define MAX_INPUT      4096
#define MAX_TOKENS     200
#define MAX_TEXT       64
#define MAX_NODES      200
#define MAX_CODE       200
#define MAX_TRACE      300
#define MAX_CHILDREN   50
#define MAX_LINE       200
#define MAX_DERIV      100

/* ================================================================
   TOKEN TYPES AND STRUCTURES
   ================================================================ */
#define TOKEN_IF       1
#define TOKEN_ELSE     2
#define TOKEN_INT      3
#define TOKEN_ID       4
#define TOKEN_NUM      5
#define TOKEN_RELOP    6
#define TOKEN_ASSIGN   7
#define TOKEN_AND      8
#define TOKEN_OR       9
#define TOKEN_LPAREN   10
#define TOKEN_RPAREN   11
#define TOKEN_LBRACE   12
#define TOKEN_RBRACE   13
#define TOKEN_SEMI     14
#define TOKEN_PLUS     15
#define TOKEN_MINUS    16
#define TOKEN_END      17
#define TOKEN_INVALID  18

struct Token {
    int  type;
    char text[MAX_TEXT];
    int  line;
    int  col;
};

/* ================================================================
   AST NODE TYPES AND STRUCTURES
   ================================================================ */
#define NODE_PROGRAM   1
#define NODE_IF        2
#define NODE_BLOCK     3
#define NODE_ASSIGN    4
#define NODE_DECL      5
#define NODE_COND      6

struct ASTNode {
    int  kind;
    char left[MAX_TEXT];
    char op[MAX_TEXT];
    char right[MAX_TEXT];
    char logicalOp[MAX_TEXT];
    int  condRight;
    int  condition;
    int  thenBranch;
    int  elseBranch;
    int  children[MAX_CHILDREN];
    int  childCount;
    char varName[MAX_TEXT];
    char expr[MAX_TEXT];
};

/* ================================================================
   GLOBAL VARIABLES (declared extern so all files share them)
   ================================================================ */
extern FILE *out;

/* Lexer Globals */
extern char source[MAX_INPUT];
extern int  sourceLen;
extern struct Token tokens[MAX_TOKENS];
extern int tokenCount;
extern char dfaTrace[MAX_TRACE][MAX_LINE];
extern int  dfaTraceCount;

/* Parser Globals */
extern struct ASTNode nodes[MAX_NODES];
extern int nodeCount;
extern int hasError;
extern char errorMsg[256];
extern char pdaTrace[MAX_TRACE][MAX_LINE];
extern int  pdaTraceCount;
extern char derivation[MAX_DERIV][MAX_LINE];
extern int  derivCount;
extern int hasInvalidToken;

/* CodeGen Globals */
extern char code[MAX_CODE][MAX_LINE];
extern int  codeCount;

/* ================================================================
   FUNCTION PROTOTYPES
   ================================================================ */

/* lexer.c */
void tokenize(void);

/* parser.c */
int parseProgram(void);
int newNode(int kind);

/* codegen.c */
void generateCode(int nodeIndex);

/* printer.c */
void printLine(void);
void printBanner(const char* title);
void printSourceCode(void);
void printTokenTable(void);
void printDFATrace(void);
void printPDATrace(void);
void printCFG(void);
void printDerivation(void);
void printAST(int nodeIndex, int depth);
void printIntermediateCode(void);
const char* getTokenName(int type);
const char* dfaStateName(int state);

#endif /* COMPILER_H */
