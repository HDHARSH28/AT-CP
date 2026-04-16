#include "compiler.h"

/* Local variables for Lexer state */
static int pos;
static int curLine;
static int curCol;

void logDFA(int fromState, char ch, int toState) {
    char display[4];
    if (ch >= 32 && ch <= 126) { display[0] = ch; display[1] = '\0'; }
    else { display[0] = ' '; display[1] = '\0'; }

    if (dfaTraceCount < MAX_TRACE) {
        sprintf(dfaTrace[dfaTraceCount],
                "     %s --'%s'--> %s",
                dfaStateName(fromState), display, dfaStateName(toState));
        dfaTraceCount++;
    }
}

void skipSpaces() {
    while (pos < sourceLen) {
        char c = source[pos];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (c == '\n') { curLine++; curCol = 1; }
            else { curCol++; }
            pos++;
        } else {
            break;
        }
    }
}

struct Token readNextToken() {
    struct Token tok;
    int startLine, startCol;
    char c;
    int len;

    tok.type = TOKEN_INVALID;
    tok.text[0] = '\0';
    tok.line = curLine;
    tok.col = curCol;

    startLine = curLine;
    startCol = curCol;

    c = source[pos];

    if (c == '(') {
        tok.type = TOKEN_LPAREN; tok.text[0] = '('; tok.text[1] = '\0';
        tok.line = startLine; tok.col = startCol;
        logDFA(0, c, 8);
        pos++; curCol++;
        return tok;
    }
    if (c == ')') {
        tok.type = TOKEN_RPAREN; tok.text[0] = ')'; tok.text[1] = '\0';
        tok.line = startLine; tok.col = startCol;
        logDFA(0, c, 8);
        pos++; curCol++;
        return tok;
    }
    if (c == '{') {
        tok.type = TOKEN_LBRACE; tok.text[0] = '{'; tok.text[1] = '\0';
        tok.line = startLine; tok.col = startCol;
        logDFA(0, c, 8);
        pos++; curCol++;
        return tok;
    }
    if (c == '}') {
        tok.type = TOKEN_RBRACE; tok.text[0] = '}'; tok.text[1] = '\0';
        tok.line = startLine; tok.col = startCol;
        logDFA(0, c, 8);
        pos++; curCol++;
        return tok;
    }
    if (c == ';') {
        tok.type = TOKEN_SEMI; tok.text[0] = ';'; tok.text[1] = '\0';
        tok.line = startLine; tok.col = startCol;
        logDFA(0, c, 8);
        pos++; curCol++;
        return tok;
    }
    if (c == '+') {
        tok.type = TOKEN_PLUS; tok.text[0] = '+'; tok.text[1] = '\0';
        tok.line = startLine; tok.col = startCol;
        logDFA(0, c, 8);
        pos++; curCol++;
        return tok;
    }
    if (c == '-') {
        tok.type = TOKEN_MINUS; tok.text[0] = '-'; tok.text[1] = '\0';
        tok.line = startLine; tok.col = startCol;
        logDFA(0, c, 8);
        pos++; curCol++;
        return tok;
    }

    if (isalpha(c) || c == '_') {
        logDFA(0, c, 1);
        len = 0;
        while (pos < sourceLen && (isalpha(source[pos]) || isdigit(source[pos]) || source[pos] == '_')) {
            if (len < MAX_TEXT - 1) tok.text[len++] = source[pos];
            pos++; curCol++;
        }
        tok.text[len] = '\0';
        logDFA(1, ' ', 8);

        if (strcmp(tok.text, "if") == 0)        tok.type = TOKEN_IF;
        else if (strcmp(tok.text, "else") == 0) tok.type = TOKEN_ELSE;
        else if (strcmp(tok.text, "int") == 0)  tok.type = TOKEN_INT;
        else                                    tok.type = TOKEN_ID;

        tok.line = startLine; tok.col = startCol;
        return tok;
    }

    if (isdigit(c)) {
        logDFA(0, c, 2);
        len = 0;
        while (pos < sourceLen && isdigit(source[pos])) {
            if (len < MAX_TEXT - 1) tok.text[len++] = source[pos];
            pos++; curCol++;
        }
        tok.text[len] = '\0';
        logDFA(2, ' ', 8);
        tok.type = TOKEN_NUM;
        tok.line = startLine; tok.col = startCol;
        return tok;
    }

    if (c == '<' || c == '>') {
        logDFA(0, c, 3);
        tok.text[0] = c; len = 1;
        pos++; curCol++;
        if (pos < sourceLen && source[pos] == '=') {
            tok.text[1] = '='; len = 2;
            logDFA(3, '=', 8);
            pos++; curCol++;
        } else {
            logDFA(3, ' ', 8);
        }
        tok.text[len] = '\0';
        tok.type = TOKEN_RELOP;
        tok.line = startLine; tok.col = startCol;
        return tok;
    }

    if (c == '=') {
        logDFA(0, c, 4);
        tok.text[0] = '='; len = 1;
        pos++; curCol++;
        if (pos < sourceLen && source[pos] == '=') {
            tok.text[1] = '='; len = 2;
            logDFA(4, '=', 8);
            pos++; curCol++;
            tok.text[len] = '\0';
            tok.type = TOKEN_RELOP;
        } else {
            logDFA(4, ' ', 8);
            tok.text[len] = '\0';
            tok.type = TOKEN_ASSIGN;
        }
        tok.line = startLine; tok.col = startCol;
        return tok;
    }

    if (c == '!') {
        logDFA(0, c, 5);
        tok.text[0] = '!'; len = 1;
        pos++; curCol++;
        if (pos < sourceLen && source[pos] == '=') {
            tok.text[1] = '='; len = 2;
            logDFA(5, '=', 8);
            pos++; curCol++;
            tok.text[len] = '\0';
            tok.type = TOKEN_RELOP;
        } else {
            logDFA(5, ' ', 8);
            tok.text[len] = '\0';
            tok.type = TOKEN_INVALID;
        }
        tok.line = startLine; tok.col = startCol;
        return tok;
    }

    if (c == '&') {
        logDFA(0, c, 6);
        tok.text[0] = '&'; len = 1;
        pos++; curCol++;
        if (pos < sourceLen && source[pos] == '&') {
            tok.text[1] = '&'; len = 2;
            logDFA(6, '&', 8);
            pos++; curCol++;
            tok.text[len] = '\0';
            tok.type = TOKEN_AND;
        } else {
            logDFA(6, ' ', 9);
            tok.text[len] = '\0';
            tok.type = TOKEN_INVALID;
        }
        tok.line = startLine; tok.col = startCol;
        return tok;
    }

    if (c == '|') {
        logDFA(0, c, 7);
        tok.text[0] = '|'; len = 1;
        pos++; curCol++;
        if (pos < sourceLen && source[pos] == '|') {
            tok.text[1] = '|'; len = 2;
            logDFA(7, '|', 8);
            pos++; curCol++;
            tok.text[len] = '\0';
            tok.type = TOKEN_OR;
        } else {
            logDFA(7, ' ', 9);
            tok.text[len] = '\0';
            tok.type = TOKEN_INVALID;
        }
        tok.line = startLine; tok.col = startCol;
        return tok;
    }

    logDFA(0, c, 9);
    tok.text[0] = c; tok.text[1] = '\0';
    tok.type = TOKEN_INVALID;
    tok.line = startLine; tok.col = startCol;
    pos++; curCol++;
    return tok;
}

void tokenize() {
    tokenCount = 0;
    pos = 0;
    curLine = 1;
    curCol = 1;
    dfaTraceCount = 0;

    while (tokenCount < MAX_TOKENS - 1) {
        skipSpaces();
        if (pos >= sourceLen) {
            tokens[tokenCount].type = TOKEN_END;
            tokens[tokenCount].text[0] = '\0';
            tokens[tokenCount].line = curLine;
            tokens[tokenCount].col = curCol;
            tokenCount++;
            break;
        }
        tokens[tokenCount] = readNextToken();
        tokenCount++;
    }
}
