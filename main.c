#include "compiler.h"

/* Define all global variables here */
FILE *out;
char source[MAX_INPUT];
int  sourceLen;
struct Token tokens[MAX_TOKENS];
int tokenCount;
char dfaTrace[MAX_TRACE][MAX_LINE];
int  dfaTraceCount;
struct ASTNode nodes[MAX_NODES];
int nodeCount;
int hasError;
char errorMsg[256];
char pdaTrace[MAX_TRACE][MAX_LINE];
int  pdaTraceCount;
char derivation[MAX_DERIV][MAX_LINE];
int  derivCount;
int hasInvalidToken = 0;
char code[MAX_CODE][MAX_LINE];
int  codeCount;

int main() {
    FILE *fin;
    int ch, i;
    int root;

    /* ---- Step 1: Read input.txt ---- */
    fin = fopen("input.txt", "r");
    if (fin == NULL) {
        printf("Error: Cannot open 'input.txt'.\n");
        return 1;
    }

    sourceLen = 0;
    while (sourceLen < MAX_INPUT - 1) {
        ch = fgetc(fin);
        if (ch == EOF) break;
        source[sourceLen] = (char)ch;
        sourceLen++;
    }
    source[sourceLen] = '\0';
    fclose(fin);

    if (sourceLen == 0) {
        printf("Error: input.txt is empty.\n");
        return 1;
    }

    /* ---- Step 2: Open output.txt ---- */
    out = fopen("output.txt", "w");
    if (out == NULL) {
        printf("Error: Cannot open 'output.txt' for writing.\n");
        return 1;
    }

    printLine();
    fprintf(out, "  Conditional & Branching Mini Compiler\n");
    fprintf(out, "  Models: DFA (Lexer) | CFG + PDA (Parser) | TAC (CodeGen)\n");
    printLine();
    printSourceCode();

    /* ============================================================
       PHASE 1 : LEXICAL ANALYSIS (DFA)
       ============================================================ */
    tokenize();

    for (i = 0; i < tokenCount; i++) {
        if (tokens[i].type == TOKEN_INVALID) {
            hasInvalidToken = 1;
            fprintf(out, "\n  ** Lexical Error: Unknown symbol '%s' at line %d, col %d\n",
                    tokens[i].text, tokens[i].line, tokens[i].col);
        }
    }

    printTokenTable();
    printDFATrace();

    if (hasInvalidToken) {
        fprintf(out, "\n  Lexical analysis found errors. Stopping.\n");
        fclose(out);
        printf("Output written to output.txt (with errors).\n");
        return 1;
    }

    /* ============================================================
       PHASE 2 : SYNTAX ANALYSIS (PDA)
       ============================================================ */
    nodeCount = 0;
    root = parseProgram();

    printPDATrace();

    if (hasError) {
        fprintf(out, "\n  ** %s\n", errorMsg);
        fprintf(out, "\n  Syntax Validation: INVALID\n");
        fclose(out);
        printf("Output written to output.txt (with errors).\n");
        return 1;
    }

    printCFG();
    printDerivation();
    fprintf(out, "\n  Syntax Validation: VALID\n");
    fprintf(out, "\n  Abstract Syntax Tree (AST):\n");
    printAST(root, 2);

    /* ============================================================
       PHASE 3 : INTERMEDIATE CODE GENERATION
       ============================================================ */
    codeCount = 0;
    generateCode(root);
    printIntermediateCode();

    fprintf(out, "\n");
    printLine();
    fprintf(out, "  Compilation complete.\n");
    printLine();

    fclose(out);
    printf("Compilation successful! Output written to output.txt\n");

    return 0;
}
