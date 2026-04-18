#include "compiler.h"

// Define global parsing variables
int currentPos = 0;
bool hasError = false;
stack<string> pdaStack;
int labelCounter = 1;
vector<string> intermediateCode;

// ========================================================================
// PHASE 2 & 3: SYNTAX ANALYSIS (PDA) & CODE GENERATION
// Parses tokens using stack mirroring, and outputs Three-Address Code
// ========================================================================

Token peek() {
    if (currentPos < tokens.size()) return tokens[currentPos];
    return {"EOF", ""};
}

void match(string expectedType, string expectedValue) {
    Token t = peek();
    if (t.type == "EOF") return;

    if (t.type == expectedType && (expectedValue == "" || t.value == expectedValue)) {
        
        // Classic PDA: Only push opening brackets to the stack
        if (t.value == "{" || t.value == "(") {
            cout << "  [PDA] PUSH: '" << t.value << "' onto stack" << endl;
            pdaStack.push(t.value); 
        } 
        // Classic PDA: Pop matching bracket when closing is found
        else if (t.value == "}" || t.value == ")") {
            if (!pdaStack.empty()) {
                cout << "  [PDA] POP : '" << pdaStack.top() << "' off stack (Matched with '" << t.value << "')" << endl;
                pdaStack.pop();
            }
        } 
        else {
            cout << "  [PDA] Read: '" << t.value << "' (Stack unchanged)" << endl;
        }

        currentPos++;
    } else {
        cout << "  [PDA] Syntax Error! Expected " << (expectedValue != "" ? expectedValue : expectedType) << endl;
        hasError = true;
    }
}

// Parse: "x > 5"
string parseCondition() {
    string conditionStr = "";
    
    // Parse the condition e.g. "x > 5"
    Token left = peek();
    match("ID", "");
    conditionStr += left.value + " ";

    Token op = peek();
    match("OPERATOR", "");
    conditionStr += op.value + " ";

    Token right = peek();
    if (right.type == "ID") match("ID", "");
    else match("NUMBER", "");
    conditionStr += right.value;

    return conditionStr; // returns something like "x > 5"
}

// Parse: "{ y = 10; }"
void parseBlock() {
    match("SYMBOL", "{");
    
    // Simple assignment inside block (e.g. y = 10;)
    Token var = peek();
    match("ID", "");
    string varName = var.value;

    match("OPERATOR", "=");

    Token val = peek();
    if (val.type == "NUMBER") match("NUMBER", "");
    else match("ID", "");
    string valName = val.value;

    match("SYMBOL", ";");

    match("SYMBOL", "}"); // Automatically triggers the PDA POP in match()

    // Generate TAC code for assignment
    intermediateCode.push_back(varName + " = " + valName);
}

// Parse: "if (...) {...} else {...}"
void parseIfStatement() {
    match("KEYWORD", "if");
    
    match("SYMBOL", "(");
    string cond = parseCondition();
    match("SYMBOL", ")"); // Automatically triggers the PDA POP in match()

    // Generate TAC Code for IF Condition
    string L1 = "L" + to_string(labelCounter++);
    string L2 = "L" + to_string(labelCounter++);
    intermediateCode.push_back("if (" + cond + ") goto " + L1);
    intermediateCode.push_back("goto " + L2);
    intermediateCode.push_back(L1 + ":");

    parseBlock(); // Parse True block

    if (peek().value == "else") {
        match("KEYWORD", "else");
        
        string L3 = "L" + to_string(labelCounter++);
        intermediateCode.push_back("goto " + L3); // Skip else block if true finished
        intermediateCode.push_back(L2 + ":"); // Start of false block
        
        if (peek().value == "if") {
            // It's an "else if", recursively parse it
            parseIfStatement();
        } else {
            // It's a normal "else" block
            parseBlock(); // Parse False block
        }
        
        intermediateCode.push_back(L3 + ":"); // End of entire if-else
    } else {
        intermediateCode.push_back(L2 + ":"); // End of if
    }
}
