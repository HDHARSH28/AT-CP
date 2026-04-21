#include "compiler.h"
#include <fstream>

int main() {
    // Open output.txt and redirect cout (all console prints) to it
    ofstream fout("output.txt");
    streambuf *coutbuf = cout.rdbuf(); 
    cout.rdbuf(fout.rdbuf());

    cout << "--- SIMPLE COMPILER ---" << endl;

    // Open input.txt and read the source code
    ifstream fin("input.txt");
    if (!fin) {
        cout << "Error: Could not open input.txt" << endl;
        // Restore cout to print error
        cout.rdbuf(coutbuf);
        cout << "Error: Could not open input.txt" << endl;
        return 1;
    }

    string code = "";
    string line;
    while (getline(fin, line)) {
        code += line + "\n";
    }
    fin.close();
    
    cout << "Source Code: " << code << "\n\n";

    // 1. Lexical Analysis
    tokenize(code);
    
    cout << "1. LEXER Result (Tokens List):" << endl;
    for (Token t : tokens) {
        cout << "[" << t.type << " : '" << t.value << "'] ";
    }
    cout << "\n\n";

    // 2. Syntax Analysis (PDA)
    cout << "2. PARSER Result (PDA tracing):" << endl;
    
    // Reset Globals in case of reruns (though main only runs once)
    currentPos = 0;
    hasError = false;
    intermediateCode.clear();
    errorMessages.clear();

    if (tokens.size() > 0 && tokens[0].type != "EOF") {
        parseIfStatement();
    }

    if (hasError) {
        cout << "\n======================================" << endl;
        cout << "Result: Syntax is INVALID." << endl;
        cout << "======================================" << endl;
        for (string err : errorMessages) {
            cout << "-> " << err << endl;
        }
        cout << "======================================\n" << endl;
    } else {
        cout << "\nResult: Syntax is VALID." << endl;
    }

    // 3. Code Generation (TAC)
    cout << "\n3. CODE GENERATION (Three-Address Code):" << endl;
    for (string tac_line : intermediateCode) {
        cout << tac_line << endl;
    }

    // Restore cout to console and print success message there
    cout.rdbuf(coutbuf);
    cout << "Compilation successful! Output saved in output.txt." << endl;

    return 0;
}
