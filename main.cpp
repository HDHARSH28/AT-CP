#include "compiler.h"
#include <fstream>

int main()
{
    // Redirect all diagnostic and result output to output.txt.
    ofstream fout("output.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(fout.rdbuf());

    cout << "--- SIMPLE COMPILER ---" << endl;

    ifstream fin("input.txt");
    if (!fin)
    {
        cout.rdbuf(coutbuf);
        cout << "Error: Could not open input.txt" << endl;
        return 1;
    }

    string code = "", line;
    while (getline(fin, line))
        code += line + "\n";
    fin.close();

    cout << "Source Code:\n"
         << code << "\n";

    tokenize(code);

    cout << "1. LEXER Result (Token List):" << endl;
    for (Token t : tokens)
        cout << "[" << t.type << ":'" << t.value << "'] ";
    cout << "\n\n";

    cout << "2. PARSER Result (PDA tracing):" << endl;

    // Reset parser/codegen state before each compilation run.
    currentPos = 0;
    hasError = false;
    labelCounter = 1;
    loopDepth = 0;
    intermediateCode.clear();
    while (!pdaStack.empty())
        pdaStack.pop();
    if (tokens.empty())
    {
        cout << "  [PDA] Syntax Error! Empty input — nothing to parse" << endl;
        hasError = true;
    }
    else
    {
        while (peek().type != "EOF")
        {
            if (peek().type == "KEYWORD" ||
                peek().type == "ID")
            {
                parseStatement();
            }
            else
            {
                cout << "  [PDA] Syntax Error! Top-level must start with a statement, got '"
                     << peek().value << "'" << endl;
                hasError = true;
                break;
            }
        }
    }

    if (hasError)
        cout << "\nResult: Syntax is INVALID." << endl;
    else
        cout << "\nResult: Syntax is VALID." << endl;

    // Print generated three-address code even when syntax errors occur.
    cout << "\n3. CODE GENERATION (Three-Address Code):" << endl;
    for (string &l : intermediateCode)
        cout << l << endl;

    cout.rdbuf(coutbuf);
    cout << "Done! Check output.txt" << endl;
    return 0;
}
