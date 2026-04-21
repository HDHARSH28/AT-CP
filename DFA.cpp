#include "compiler.h"
#include <cctype>

// Define global tokens vector
vector<Token> tokens;

void tokenize(string input) {
    tokens.clear(); // Ensure we start fresh
    int i = 0;
    int currentLine = 1; // Track the current line number
    while (i < input.length()) {
        char c = input[i];

        // Skip spaces and track newlines
        if (isspace(c)) {
            if (c == '\n') currentLine++;
            i++;
            continue;
        }

        // DFA State: Identifiers and Keywords (letters)
        if (isalpha(c)) {
            string word = "";
            int state = 0;
            cout << "  [DFA Trace]: " << endl;
            while (i < input.length() && (isalpha(input[i]) || isdigit(input[i]))) {
                cout << "    State " << state << " --reads '" << input[i] << "'--> State 1" << endl;
                word += input[i];
                state = 1;
                i++;
            }
            if (word == "if") tokens.push_back({"KEYWORD", word, currentLine});
            else if (word == "else") tokens.push_back({"KEYWORD", word, currentLine});
            else tokens.push_back({"ID", word, currentLine}); // variable names
            
            cout << "    => Final State! Accepted Token: [" << word << "]\n" << endl;
            continue;
        }

        // DFA State: Numbers (digits)
        if (isdigit(c)) {
            string num = "";
            int state = 0;
            cout << "  [DFA Trace]: " << endl;
            while (i < input.length() && isdigit(input[i])) {
                cout << "    State " << state << " --reads '" << input[i] << "'--> State 2" << endl;
                num += input[i];
                state = 2;
                i++;
            }
            tokens.push_back({"NUMBER", num, currentLine});
            cout << "    => Final State! Accepted Token: [" << num << "]\n" << endl;
            continue;
        }

        // DFA State: Operators and Symbols
        if (c == '=' || c == '>' || c == '<' || c == '!') {
            string op = string(1, c);
            cout << "  [DFA Trace]: " << endl;
            cout << "    State 0 --reads '" << c << "'--> State 3" << endl;
            i++;
            if (i < input.length() && input[i] == '=') { // Example: == or >= or <=
                op += "=";
                cout << "    State 3 --reads '='--> State 4" << endl;
                i++;
            }
            tokens.push_back({"OPERATOR", op, currentLine});
            cout << "    => Final State! Accepted Token: [" << op << "]\n" << endl;
            continue;
        }

        // DFA State: Logical Operators
        if (c == '&' || c == '|') {
            string op = string(1, c);
            cout << "  [DFA Trace]: " << endl;
            cout << "    State 0 --reads '" << c << "'--> State 6" << endl;
            i++;
            if (i < input.length() && input[i] == c) { // && or ||
                op += input[i];
                cout << "    State 6 --reads '" << c << "'--> State 7" << endl;
                i++;
                tokens.push_back({"LOGICAL", op, currentLine});
                cout << "    => Final State! Accepted Token: [" << op << "]\n" << endl;
            } else {
                tokens.push_back({"ERROR", op, currentLine});
                cout << "    => Error State!" << endl;
            }
            continue;
        }

        // Parentheses and Braces
        if (c == '(' || c == ')' || c == '{' || c == '}' || c == ';') {
            cout << "  [DFA Trace]: " << endl;
            cout << "    State 0 --reads '" << c << "'--> State 5" << endl;
            tokens.push_back({"SYMBOL", string(1, c), currentLine});
            cout << "    => Final State! Accepted Token: [" << c << "]\n" << endl;
            i++;
            continue;
        }

        // Unknown character
        tokens.push_back({"ERROR", string(1, c), currentLine});
        i++;
    }
}
