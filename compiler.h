#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

struct Token {
    string type;
    string value;
};

// Global token stream produced by the lexer.
extern vector<Token> tokens;

void tokenize(string input);

// Shared parser and code-generation state.
extern int currentPos;
extern bool hasError;
extern stack<string> pdaStack;
extern int labelCounter;
extern int loopDepth;
extern vector<string> intermediateCode;

// Returns the current token without consuming it.
Token peek();
// Consumes one token and validates its type and optional value.
void match(string expectedType, string expectedValue = "");
// Parses an identifier/number and optional arithmetic tail.
string parseExpression();
// Parses a relational condition used by if statements.
string parseCondition();
// Parses one statement (assignment, branch, or loop).
void parseStatement();
// Parses a non-empty statement block enclosed in braces.
void parseBlock();
// Parses if / else if / else and emits intermediate code labels.
void parseIfStatement();
// Parses while loops and emits labels/gotos.
void parseWhileStatement();
// Parses do-while loops and emits labels/gotos.
void parseDoWhileStatement();
// Parses simple for loops and emits labels/gotos.
void parseForStatement();

#endif
