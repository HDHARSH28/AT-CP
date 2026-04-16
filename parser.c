#include "compiler.h"

static int currentToken;
static char pdaStack[200][MAX_TEXT];
static int pdaTop;

int newNode(int kind) {
    int index = nodeCount;
    nodeCount++;

    nodes[index].kind = kind;
    nodes[index].left[0] = '\0';
    nodes[index].op[0] = '\0';
    nodes[index].right[0] = '\0';
    nodes[index].logicalOp[0] = '\0';
    nodes[index].condRight = -1;
    nodes[index].condition = -1;
    nodes[index].thenBranch = -1;
    nodes[index].elseBranch = -1;
    nodes[index].childCount = 0;
    nodes[index].varName[0] = '\0';
    nodes[index].expr[0] = '\0';

    return index;
}

void logPDA(const char* msg) {
    if (pdaTraceCount < MAX_TRACE) {
        strcpy(pdaTrace[pdaTraceCount], msg);
        pdaTraceCount++;
    }
}

void pdaPush(const char* symbol) {
    char buf[MAX_LINE];
    pdaTop++;
    strcpy(pdaStack[pdaTop], symbol);
    sprintf(buf, "     PUSH '%s'  |  Stack top -> %s", symbol, pdaStack[pdaTop]);
    logPDA(buf);
}

void pdaPop() {
    char buf[MAX_LINE];
    if (pdaTop < 0) return;
    if (pdaTop > 0)
        sprintf(buf, "     POP  '%s'  |  Stack top -> %s", pdaStack[pdaTop], pdaStack[pdaTop - 1]);
    else
        sprintf(buf, "     POP  '%s'  |  Stack EMPTY", pdaStack[pdaTop]);
    pdaTop--;
    logPDA(buf);
}

void addDerivation(const char* rule) {
    if (derivCount < MAX_DERIV) {
        strcpy(derivation[derivCount], rule);
        derivCount++;
    }
}

struct Token* peek() { return &tokens[currentToken]; }
int atEnd() { return tokens[currentToken].type == TOKEN_END; }
int check(int type) { return !atEnd() && tokens[currentToken].type == type; }
int advance() { if (!atEnd()) currentToken++; return currentToken - 1; }
int match(int type) { if (check(type)) { advance(); return 1; } return 0; }

void syntaxError(const char* msg) {
    if (!hasError) {
        hasError = 1;
        sprintf(errorMsg, "Syntax Error at line %d, col %d near '%s': %s",
                peek()->line, peek()->col, peek()->text, msg);
    }
}

int consume(int type, const char* msg) {
    if (check(type)) return advance();
    syntaxError(msg);
    return -1;
}

/* Forward declarations */
int parseStatement(void);
int parseIfStmt(void);
int parseCondition(void);
int parseBlock(void);
int parseAssign(void);
int parseDecl(void);

void parseExpr(char* result) {
    int idx;
    result[0] = '\0';

    if (check(TOKEN_ID) || check(TOKEN_NUM)) {
        idx = advance();
        strcpy(result, tokens[idx].text);
    } else {
        syntaxError("Expected a value");
        return;
    }

    if (check(TOKEN_PLUS) || check(TOKEN_MINUS)) {
        idx = advance();
        strcat(result, " ");
        strcat(result, tokens[idx].text);
        strcat(result, " ");
        if (check(TOKEN_ID) || check(TOKEN_NUM)) {
            idx = advance();
            strcat(result, tokens[idx].text);
        } else {
            syntaxError("Expected a value after operator");
        }
    }
}

int parseStatement() {
    int node = -1;
    if (hasError) return -1;

    pdaPush("stmt");
    if (check(TOKEN_IF)) { addDerivation("stmt -> if_stmt"); node = parseIfStmt(); }
    else if (check(TOKEN_LBRACE)) { addDerivation("stmt -> block"); node = parseBlock(); }
    else if (check(TOKEN_INT)) { addDerivation("stmt -> decl_stmt"); node = parseDecl(); }
    else if (check(TOKEN_ID)) { addDerivation("stmt -> assign_stmt"); node = parseAssign(); }
    else { syntaxError("Expected 'if', '{', 'int', or identifier"); }
    pdaPop();
    return node;
}

int parseIfStmt() {
    int node;
    if (hasError) return -1;

    pdaPush("if_stmt");
    if (consume(TOKEN_IF, "Expected 'if'") < 0) { pdaPop(); return -1; }
    logPDA("     MATCH 'if'");

    if (consume(TOKEN_LPAREN, "Expected '(' after 'if'") < 0) { pdaPop(); return -1; }
    pdaPush("(");
    logPDA("     MATCH '('");

    node = newNode(NODE_IF);
    nodes[node].condition = parseCondition();
    if (hasError) { pdaPop(); pdaPop(); return -1; }

    if (consume(TOKEN_RPAREN, "Expected ')' after condition") < 0) { pdaPop(); pdaPop(); return -1; }
    pdaPop();
    logPDA("     MATCH ')' -- balanced parentheses");

    nodes[node].thenBranch = parseStatement();
    if (hasError) { pdaPop(); return -1; }

    if (match(TOKEN_ELSE)) {
        logPDA("     MATCH 'else'");
        addDerivation("if_stmt -> if ( cond ) stmt else stmt");
        nodes[node].elseBranch = parseStatement();
    } else {
        addDerivation("if_stmt -> if ( cond ) stmt");
    }

    pdaPop();
    return node;
}

int parseCondition() {
    int node, idx;
    char rule[MAX_LINE];
    if (hasError) return -1;

    pdaPush("condition");
    node = newNode(NODE_COND);

    if (consume(TOKEN_ID, "Expected identifier in condition") < 0) { pdaPop(); return -1; }
    strcpy(nodes[node].left, tokens[currentToken - 1].text);

    if (consume(TOKEN_RELOP, "Expected relational operator") < 0) { pdaPop(); return -1; }
    strcpy(nodes[node].op, tokens[currentToken - 1].text);

    if (check(TOKEN_ID) || check(TOKEN_NUM)) {
        idx = advance();
        strcpy(nodes[node].right, tokens[idx].text);
    } else {
        syntaxError("Expected identifier or number in condition");
        pdaPop(); return -1;
    }

    sprintf(rule, "condition -> id %s expr", nodes[node].op);
    addDerivation(rule);

    if (check(TOKEN_AND) || check(TOKEN_OR)) {
        idx = advance();
        strcpy(nodes[node].logicalOp, tokens[idx].text);
        sprintf(rule, "     MATCH '%s'", nodes[node].logicalOp);
        logPDA(rule);
        sprintf(rule, "condition -> condition %s condition", nodes[node].logicalOp);
        addDerivation(rule);
        nodes[node].condRight = parseCondition();
    }

    pdaPop();
    return node;
}

int parseBlock() {
    int node;
    if (hasError) return -1;

    pdaPush("block");
    if (consume(TOKEN_LBRACE, "Expected '{'") < 0) { pdaPop(); return -1; }
    pdaPush("{");
    logPDA("     MATCH '{'");

    node = newNode(NODE_BLOCK);

    while (!check(TOKEN_RBRACE) && !atEnd() && !hasError) {
        if (nodes[node].childCount < MAX_CHILDREN) {
            nodes[node].children[nodes[node].childCount] = parseStatement();
            nodes[node].childCount++;
        }
    }

    if (consume(TOKEN_RBRACE, "Expected '}'") < 0) { pdaPop(); pdaPop(); return -1; }
    pdaPop();
    logPDA("     MATCH '}' -- balanced braces");
    addDerivation("block -> { stmt_list }");

    pdaPop();
    return node;
}

int parseAssign() {
    int node, idx;
    if (hasError) return -1;

    node = newNode(NODE_ASSIGN);
    idx = consume(TOKEN_ID, "Expected identifier");
    if (idx < 0) return -1;
    strcpy(nodes[node].varName, tokens[idx].text);

    if (consume(TOKEN_ASSIGN, "Expected '='") < 0) return -1;
    parseExpr(nodes[node].expr);
    if (hasError) return -1;
    if (consume(TOKEN_SEMI, "Expected ';'") < 0) return -1;

    addDerivation("assign_stmt -> id = expr ;");
    return node;
}

int parseDecl() {
    int node, idx;
    if (hasError) return -1;

    node = newNode(NODE_DECL);
    if (consume(TOKEN_INT, "Expected 'int'") < 0) return -1;
    idx = consume(TOKEN_ID, "Expected identifier");
    if (idx < 0) return -1;
    strcpy(nodes[node].varName, tokens[idx].text);

    if (match(TOKEN_ASSIGN)) {
        parseExpr(nodes[node].expr);
        if (hasError) return -1;
        addDerivation("decl_stmt -> int id = expr ;");
    } else {
        addDerivation("decl_stmt -> int id ;");
    }

    if (consume(TOKEN_SEMI, "Expected ';'") < 0) return -1;
    return node;
}

int parseProgram() {
    int root;

    currentToken = 0;
    hasError = 0;
    errorMsg[0] = '\0';
    pdaTop = -1;
    pdaTraceCount = 0;
    derivCount = 0;

    logPDA("--- PDA Simulation Start ---");
    logPDA("     Initial stack: [ $ ]");
    pdaPush("$");
    pdaPush("program");

    root = newNode(NODE_PROGRAM);

    while (!atEnd() && !hasError) {
        if (nodes[root].childCount < MAX_CHILDREN) {
            nodes[root].children[nodes[root].childCount] = parseStatement();
            nodes[root].childCount++;
        }
    }

    if (!hasError) {
        pdaPop();
        pdaPop();
        logPDA("     Stack is empty -> INPUT ACCEPTED");
    }
    logPDA("--- PDA Simulation End ---");

    return root;
}
