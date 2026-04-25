#include "compiler.h"

int currentPos = 0;
bool hasError = false;
stack<string> pdaStack;
int labelCounter = 1;
vector<string> intermediateCode;

// Safe token lookahead; returns EOF sentinel beyond stream end.
Token peek() {
    if (currentPos < (int)tokens.size()) return tokens[currentPos];
    return {"EOF", ""};
}

void match(string expectedType, string expectedValue) {
    Token t = peek();
    if (t.type == "EOF") {
        if (expectedValue != "") {
            cout << "  [PDA] Syntax Error! Unexpected end of input, expected '" << expectedValue << "'" << endl;
            hasError = true;
        }
        return;
    }

    if (t.type == expectedType && (expectedValue == "" || t.value == expectedValue)) {
        if (t.value == "{" || t.value == "(") {
            cout << "  [PDA] PUSH: '" << t.value << "' onto stack" << endl;
            pdaStack.push(t.value);
        } else if (t.value == "}" || t.value == ")") {
            if (!pdaStack.empty()) {
                cout << "  [PDA] POP : '" << pdaStack.top() << "' off stack (matched '" << t.value << "')" << endl;
                pdaStack.pop();
            } else {
                cout << "  [PDA] Syntax Error! Closing '" << t.value << "' has no matching opener" << endl;
                hasError = true;
            }
        } else {
            cout << "  [PDA] Read: '" << t.value << "'" << endl;
        }
        currentPos++;
    } else {
        cout << "  [PDA] Syntax Error! Expected '"
             << (expectedValue != "" ? expectedValue : expectedType)
             << "' but got '" << t.value << "' (" << t.type << ")" << endl;
        hasError = true;
        // Advance one token to avoid getting stuck on the same error.
        currentPos++;
    }
}

string parseExpression() {
    Token t = peek();
    if (t.type == "ID") {
        match("ID", "");
        Token next = peek();
        if (next.type == "OPERATOR" &&
            (next.value == "+" || next.value == "-" || next.value == "*" || next.value == "/")) {
            string op = next.value;
            match("OPERATOR", "");
            Token right = peek();
            string rval = right.value;
            if (right.type == "ID")     match("ID", "");
            else                        match("NUMBER", "");
            return t.value + " " + op + " " + rval;
        }
        return t.value;
    } else if (t.type == "NUMBER") {
        match("NUMBER", "");
        return t.value;
    } else {
        cout << "  [PDA] Syntax Error! Expected ID or NUMBER, got '" << t.value << "'" << endl;
        hasError = true;
        currentPos++;
        return "?";
    }
}

string parseCondition() {
    string left = parseExpression();

    Token op = peek();
    if (op.type != "OPERATOR") {
        cout << "  [PDA] Syntax Error! Expected operator in condition, got '" << op.value << "'" << endl;
        hasError = true;
        return left + " ? ?";
    }
    match("OPERATOR", "");

    string right = parseExpression();

    return left + " " + op.value + " " + right;
}

void parseStatement() {
    Token t = peek();

    if (t.type == "KEYWORD" && t.value == "if") {
        parseIfStatement();
        return;
    }

    if (t.type == "ID") {
        Token var = peek();
        match("ID", "");
        string varName = var.value;

        match("OPERATOR", "=");

        string val = parseExpression();
        match("SYMBOL", ";");

        intermediateCode.push_back(varName + " = " + val);
        return;
    }

    cout << "  [PDA] Syntax Error! Expected assignment or if, got '" << t.value << "'" << endl;
    hasError = true;
    currentPos++;
}

void parseBlock() {
    match("SYMBOL", "{");

    if (peek().value == "}") {
        cout << "  [PDA] Syntax Error! Empty block — at least one statement required" << endl;
        hasError = true;
        match("SYMBOL", "}");
        return;
    }

    while (peek().value != "}" && peek().type != "EOF") {
        parseStatement();
        if (hasError) break;
    }

    match("SYMBOL", "}");
}

void parseIfStatement() {
    match("KEYWORD", "if");
    match("SYMBOL", "(");
    string cond = parseCondition();
    match("SYMBOL", ")");

    string L1 = "L" + to_string(labelCounter++);
    string L2 = "L" + to_string(labelCounter++);
    intermediateCode.push_back("if (" + cond + ") goto " + L1);
    intermediateCode.push_back("goto " + L2);
    intermediateCode.push_back(L1 + ":");

    parseBlock();

    if (peek().value == "else") {
        match("KEYWORD", "else");
        string L3 = "L" + to_string(labelCounter++);
        intermediateCode.push_back("goto " + L3);
        intermediateCode.push_back(L2 + ":");

        // Recursive call naturally handles else-if chains.
        if (peek().value == "if") {
            parseIfStatement();
        } else {
            parseBlock();
        }

        intermediateCode.push_back(L3 + ":");
    } else {
        intermediateCode.push_back(L2 + ":");
    }
}