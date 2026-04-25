#include "compiler.h"
#include <cctype>

vector<Token> tokens;

void tokenize(string input) {
    // Stateless across runs: always start with a fresh token stream.
    tokens.clear();
    int i = 0;
    while (i < (int)input.length()) {
        char c = input[i];

        if (isspace(c)) { i++; continue; }

        // Keywords and identifiers share the same lexical pattern.
        if (isalpha(c) || c == '_') {
            string word = "";
            cout << "  [DFA Trace]:" << endl;
            while (i < (int)input.length() && (isalpha(input[i]) || isdigit(input[i]) || input[i] == '_')) {
                cout << "    State 0 --reads '" << input[i] << "'--> State 1" << endl;
                word += input[i++];
            }
            if      (word == "if")   tokens.push_back({"KEYWORD", word});
            else if (word == "else") tokens.push_back({"KEYWORD", word});
            else                     tokens.push_back({"ID", word});
            cout << "    => Accepted Token: [" << word << "]\n" << endl;
            continue;
        }

        // Number literals are restricted to integer sequences.
        if (isdigit(c)) {
            string num = "";
            cout << "  [DFA Trace]:" << endl;
            while (i < (int)input.length() && isdigit(input[i])) {
                cout << "    State 0 --reads '" << input[i] << "'--> State 2" << endl;
                num += input[i++];
            }
            tokens.push_back({"NUMBER", num});
            cout << "    => Accepted Token: [" << num << "]\n" << endl;
            continue;
        }

        // Support relational and arithmetic operators.
        if (c == '=' || c == '>' || c == '<' || c == '!' || c == '+' || c == '-' || c == '*' || c == '/') {
            string op = string(1, c);
            cout << "  [DFA Trace]:" << endl;
            cout << "    State 0 --reads '" << c << "'--> State 3" << endl;
            i++;
            if ((c == '=' || c == '>' || c == '<' || c == '!') &&
                i < (int)input.length() && input[i] == '=') {
                op += "=";
                cout << "    State 3 --reads '='--> State 4" << endl;
                i++;
            }
            tokens.push_back({"OPERATOR", op});
            cout << "    => Accepted Token: [" << op << "]\n" << endl;
            continue;
        }

        // Structural symbols used by parser grammar.
        if (c == '(' || c == ')' || c == '{' || c == '}' || c == ';') {
            cout << "  [DFA Trace]:" << endl;
            cout << "    State 0 --reads '" << c << "'--> State 5" << endl;
            tokens.push_back({"SYMBOL", string(1, c)});
            cout << "    => Accepted Token: [" << c << "]\n" << endl;
            i++;
            continue;
        }

        cout << "  [DFA] Unknown character: '" << c << "' — skipped" << endl;
        tokens.push_back({"ERROR", string(1, c)});
        i++;
    }
}