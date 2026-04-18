#include "compiler.h"
#include <cctype>

// Define global tokens vector
vector<Token> tokens;

// ========================================================================
// PHASE 1: LEXICAL ANALYSIS (DFA)
// Converts raw text into meaningful Tokens (Keywords, IDs, Numbers, etc.)
// ========================================================================

void tokenize(string input) {
    tokens.clear(); // Ensure we start fresh
    int i = 0;
    while (i < input.length()) {
        char c = input[i];

        // Skip spaces
        if (isspace(c)) {
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
            if (word == "if") tokens.push_back({"KEYWORD", word});
            else if (word == "else") tokens.push_back({"KEYWORD", word});
            else tokens.push_back({"ID", word}); // variable names
            
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
            tokens.push_back({"NUMBER", num});
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
            tokens.push_back({"OPERATOR", op});
            cout << "    => Final State! Accepted Token: [" << op << "]\n" << endl;
            continue;
        }

        // Parentheses and Braces
        if (c == '(' || c == ')' || c == '{' || c == '}' || c == ';') {
            cout << "  [DFA Trace]: " << endl;
            cout << "    State 0 --reads '" << c << "'--> State 5" << endl;
            tokens.push_back({"SYMBOL", string(1, c)});
            cout << "    => Final State! Accepted Token: [" << c << "]\n" << endl;
            i++;
            continue;
        }

        // Unknown character
        tokens.push_back({"ERROR", string(1, c)});
        i++;
    }
}
