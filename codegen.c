#include "compiler.h"

static int labelNum = 0;
static int tempNum = 0;

void addCode(const char* line) {
    if (codeCount < MAX_CODE) {
        strcpy(code[codeCount], line);
        codeCount++;
    }
}

void makeLabel(char* buf) {
    sprintf(buf, "L%d", labelNum++);
}

void makeTemp(char* buf) {
    sprintf(buf, "t%d", tempNum++);
}

void generateCondition(int nodeIndex, char* resultTemp) {
    char temp[32], line[MAX_LINE];

    makeTemp(temp);
    sprintf(line, "%s = %s %s %s", temp, nodes[nodeIndex].left,
            nodes[nodeIndex].op, nodes[nodeIndex].right);
    addCode(line);
    strcpy(resultTemp, temp);

    if (nodes[nodeIndex].condRight >= 0) {
        char temp2[32], temp3[32];
        generateCondition(nodes[nodeIndex].condRight, temp2);
        makeTemp(temp3);
        sprintf(line, "%s = %s %s %s", temp3, temp,
                nodes[nodeIndex].logicalOp, temp2);
        addCode(line);
        strcpy(resultTemp, temp3);
    }
}

void generateCode(int nodeIndex) {
    char line[MAX_LINE];
    int i;

    if (nodeIndex < 0) return;

    switch (nodes[nodeIndex].kind) {

    case NODE_PROGRAM:
    case NODE_BLOCK:
        for (i = 0; i < nodes[nodeIndex].childCount; i++) {
            generateCode(nodes[nodeIndex].children[i]);
        }
        break;

    case NODE_ASSIGN:
        sprintf(line, "%s = %s", nodes[nodeIndex].varName, nodes[nodeIndex].expr);
        addCode(line);
        break;

    case NODE_DECL:
        if (nodes[nodeIndex].expr[0] != '\0') {
            sprintf(line, "%s = %s", nodes[nodeIndex].varName, nodes[nodeIndex].expr);
            addCode(line);
        }
        break;

    case NODE_IF: {
        char condTemp[32];
        generateCondition(nodes[nodeIndex].condition, condTemp);

        if (nodes[nodeIndex].elseBranch >= 0) {
            char labelTrue[32], labelFalse[32], labelEnd[32];
            makeLabel(labelTrue);
            makeLabel(labelFalse);
            makeLabel(labelEnd);

            sprintf(line, "if %s goto %s", condTemp, labelTrue); addCode(line);
            sprintf(line, "goto %s", labelFalse); addCode(line);
            sprintf(line, "%s:", labelTrue); addCode(line);

            generateCode(nodes[nodeIndex].thenBranch);

            sprintf(line, "goto %s", labelEnd); addCode(line);
            sprintf(line, "%s:", labelFalse); addCode(line);

            generateCode(nodes[nodeIndex].elseBranch);

            sprintf(line, "%s:", labelEnd); addCode(line);
        } else {
            char labelTrue[32], labelEnd[32];
            makeLabel(labelTrue);
            makeLabel(labelEnd);

            sprintf(line, "if %s goto %s", condTemp, labelTrue); addCode(line);
            sprintf(line, "goto %s", labelEnd); addCode(line);
            sprintf(line, "%s:", labelTrue); addCode(line);

            generateCode(nodes[nodeIndex].thenBranch);

            sprintf(line, "%s:", labelEnd); addCode(line);
        }
        break;
    }

    default:
        break;
    }
}
