#include "compiler.h"

const char* getTokenName(int type) {
    if (type == TOKEN_IF)      return "KEYWORD_IF";
    if (type == TOKEN_ELSE)    return "KEYWORD_ELSE";
    if (type == TOKEN_INT)     return "KEYWORD_INT";
    if (type == TOKEN_ID)      return "IDENTIFIER";
    if (type == TOKEN_NUM)     return "NUMBER";
    if (type == TOKEN_RELOP)   return "REL_OP";
    if (type == TOKEN_ASSIGN)  return "ASSIGN_OP";
    if (type == TOKEN_AND)     return "LOGICAL_AND";
    if (type == TOKEN_OR)      return "LOGICAL_OR";
    if (type == TOKEN_LPAREN)  return "LPAREN";
    if (type == TOKEN_RPAREN)  return "RPAREN";
    if (type == TOKEN_LBRACE)  return "LBRACE";
    if (type == TOKEN_RBRACE)  return "RBRACE";
    if (type == TOKEN_SEMI)    return "SEMICOLON";
    if (type == TOKEN_PLUS)    return "PLUS";
    if (type == TOKEN_MINUS)   return "MINUS";
    if (type == TOKEN_END)     return "END";
    return "INVALID";
}

const char* dfaStateName(int state) {
    if (state == 0) return "S0(start)";
    if (state == 1) return "S1(word)";
    if (state == 2) return "S2(digit)";
    if (state == 3) return "S3(angle)";
    if (state == 4) return "S4(eq)";
    if (state == 5) return "S5(bang)";
    if (state == 6) return "S6(amp)";
    if (state == 7) return "S7(pipe)";
    if (state == 8) return "ACCEPT";
    if (state == 9) return "DEAD";
    return "?";
}

void printLine() {
    fprintf(out, "============================================================\n");
}

void printBanner(const char* title) {
    fprintf(out, "\n");
    printLine();
    fprintf(out, "  %s\n", title);
    printLine();
}

void printSourceCode() {
    int i, lineNum = 1;
    fprintf(out, "\n  Source code (from input.txt):\n");
    fprintf(out, "  --------------------------------------------\n");
    fprintf(out, "  %3d | ", lineNum);
    for (i = 0; i < sourceLen; i++) {
        if (source[i] == '\n') {
            fprintf(out, "\n");
            lineNum++;
            if (i + 1 < sourceLen) fprintf(out, "  %3d | ", lineNum);
        } else {
            fprintf(out, "%c", source[i]);
        }
    }
    fprintf(out, "\n  --------------------------------------------\n");
}

void printTokenTable() {
    int i;
    printBanner("PHASE 1 -- LEXICAL ANALYSIS (DFA)");
    fprintf(out, "\n  Tokens recognized:\n");
    fprintf(out, "  %-14s %-16s %-6s %s\n", "LEXEME", "TOKEN TYPE", "LINE", "COL");
    fprintf(out, "  ------------------------------------------\n");
    for (i = 0; i < tokenCount; i++) {
        if (tokens[i].type == TOKEN_END) break;
        fprintf(out, "  %-14s %-16s %-6d %d\n",
                tokens[i].text, getTokenName(tokens[i].type),
                tokens[i].line, tokens[i].col);
    }
}

void printDFATrace() {
    int i;
    fprintf(out, "\n  DFA State Transitions:\n");
    for (i = 0; i < dfaTraceCount; i++) {
        fprintf(out, "%s\n", dfaTrace[i]);
    }
}

void printPDATrace() {
    int i;
    printBanner("PHASE 2 -- SYNTAX ANALYSIS (PDA)");
    fprintf(out, "\n  PDA Stack Operations:\n");
    for (i = 0; i < pdaTraceCount; i++) {
        fprintf(out, "%s\n", pdaTrace[i]);
    }
}

void printCFG() {
    fprintf(out, "\n  Context-Free Grammar (CFG) used:\n");
    fprintf(out, "    program    -> stmt_list\n");
    fprintf(out, "    stmt_list  -> stmt stmt_list | epsilon\n");
    fprintf(out, "    stmt       -> if_stmt | block | assign_stmt | decl_stmt\n");
    fprintf(out, "    if_stmt    -> 'if' '(' condition ')' stmt\n");
    fprintf(out, "                | 'if' '(' condition ')' stmt 'else' stmt\n");
    fprintf(out, "    condition  -> id relop expr\n");
    fprintf(out, "                | condition '&&' condition\n");
    fprintf(out, "                | condition '||' condition\n");
    fprintf(out, "    block      -> '{' stmt_list '}'\n");
    fprintf(out, "    assign_stmt-> id '=' expr ';'\n");
    fprintf(out, "    decl_stmt  -> 'int' id ';'  |  'int' id '=' expr ';'\n");
    fprintf(out, "    expr       -> id | number | id '+' id | id '-' id\n");
}

void printDerivation() {
    int i;
    fprintf(out, "\n  Derivation Steps (Leftmost):\n");
    for (i = 0; i < derivCount; i++) {
        fprintf(out, "    %d. %s\n", i + 1, derivation[i]);
    }
}

void printAST(int nodeIndex, int depth) {
    int i;
    if (nodeIndex < 0) return;

    for (i = 0; i < depth * 3; i++) fprintf(out, " ");

    switch (nodes[nodeIndex].kind) {
    case NODE_PROGRAM:
        fprintf(out, "[PROGRAM]\n");
        for (i = 0; i < nodes[nodeIndex].childCount; i++)
            printAST(nodes[nodeIndex].children[i], depth + 1);
        break;

    case NODE_IF: {
        int c;
        fprintf(out, "[IF]\n");
        for (i = 0; i < depth * 3; i++) fprintf(out, " ");
        fprintf(out, "   condition: ");
        c = nodes[nodeIndex].condition;
        while (c >= 0) {
            fprintf(out, "%s %s %s", nodes[c].left, nodes[c].op, nodes[c].right);
            if (nodes[c].condRight >= 0) fprintf(out, " %s ", nodes[c].logicalOp);
            c = nodes[c].condRight;
        }
        fprintf(out, "\n");
        for (i = 0; i < depth * 3; i++) fprintf(out, " ");
        fprintf(out, "   THEN:\n");
        printAST(nodes[nodeIndex].thenBranch, depth + 2);
        if (nodes[nodeIndex].elseBranch >= 0) {
            for (i = 0; i < depth * 3; i++) fprintf(out, " ");
            fprintf(out, "   ELSE:\n");
            printAST(nodes[nodeIndex].elseBranch, depth + 2);
        }
        break;
    }

    case NODE_BLOCK:
        fprintf(out, "[BLOCK]\n");
        for (i = 0; i < nodes[nodeIndex].childCount; i++)
            printAST(nodes[nodeIndex].children[i], depth + 1);
        break;

    case NODE_ASSIGN:
        fprintf(out, "[ASSIGN] %s = %s\n", nodes[nodeIndex].varName, nodes[nodeIndex].expr);
        break;

    case NODE_DECL:
        fprintf(out, "[DECL] int %s", nodes[nodeIndex].varName);
        if (nodes[nodeIndex].expr[0] != '\0')
            fprintf(out, " = %s", nodes[nodeIndex].expr);
        fprintf(out, "\n");
        break;

    default:
        break;
    }
}

void printIntermediateCode() {
    int i;
    printBanner("PHASE 3 -- INTERMEDIATE CODE GENERATION");
    fprintf(out, "\n  Three-Address Code (TAC):\n\n");
    for (i = 0; i < codeCount; i++) {
        int len = strlen(code[i]);
        if (len > 0 && code[i][len - 1] == ':')
            fprintf(out, "  %s\n", code[i]);
        else
            fprintf(out, "      %s\n", code[i]);
    }
}
