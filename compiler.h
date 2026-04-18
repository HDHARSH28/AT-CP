#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

// ========================================================================
// COMMON STRUCTURES
// ========================================================================

// We break the input text into "Tokens" (words/symbols)
struct Token {
    string type;
    string value;
};

// ========================================================================
// DFA (LEXICAL ANALYZER) 
// ========================================================================
// Global Token List
extern vector<Token> tokens;

// Function to convert raw code string into tokens
void tokenize(string input);

// ========================================================================
// PDA (SYNTAX PARSER & CODE GENERATOR)
// ========================================================================
// Global parsing variables
extern int currentPos;
extern bool hasError;
extern stack<string> pdaStack; // Explicit Pushdown Automaton Stack
extern int labelCounter;
extern vector<string> intermediateCode; // To hold Three-Address Code output

// Parsing functions
Token peek();
void match(string expectedType, string expectedValue = "");
string parseCondition();
void parseBlock();
void parseIfStatement();

#endif
