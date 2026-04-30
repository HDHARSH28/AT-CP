#!/usr/bin/env bash

set -u

GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

PASS=0
FAIL=0
TOTAL=0

echo -e "${BOLD}Building compiler...${RESET}"
if ! g++ -Wall -Wextra -o compiler DFA.cpp PDA.cpp main.cpp; then
    echo -e "${RED}Build failed${RESET}"
    exit 1
fi
echo -e "${GREEN}Build successful${RESET}"

run_test() {
    local desc="$1"
    local source="$2"
    local expected="$3"

    TOTAL=$((TOTAL + 1))
    printf '%s\n' "$source" > input.txt
    ./compiler >/tmp/mini_compiler_stdout.txt 2>/tmp/mini_compiler_stderr.txt

    local verdict
    verdict=$(grep -E "Result: Syntax is (VALID|INVALID)" output.txt | tail -n 1 || true)

    if [[ "$verdict" == *"Syntax is $expected."* ]]; then
        echo -e "${GREEN}PASS${RESET} [$TOTAL] $desc"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}FAIL${RESET} [$TOTAL] $desc"
        echo "  Expected: $expected"
        echo "  Got     : ${verdict:-NO VERDICT}"
        echo "  Source  : $source"
        echo "  Parser errors:"
        grep -E "\[PDA\] Syntax Error|Unknown character" output.txt | sed 's/^/    /' || echo "    none"
        FAIL=$((FAIL + 1))
    fi
}

echo -e "\n${CYAN}${BOLD}Targeted Valid Cases${RESET}"
run_test "simple if" "if (x > 5) { y = 10; }" "VALID"
run_test "if else" "if (x > 5) { y = 10; } else { y = 20; }" "VALID"
run_test "else if chain" "if (x > 5) { y = 10; } else if (x == 5) { y = 0; } else { y = 20; }" "VALID"
run_test "top-level assignment" "x = 1;" "VALID"
run_test "while increment" "while (x < 5) { x = x + 1; }" "VALID"
run_test "while with if" "while (x < 5) { if (x == 3) { y = 1; } x = x + 1; }" "VALID"
run_test "do while increment" "do { x = x + 1; } while (x < 5);" "VALID"
run_test "do while with if" "do { if (x != 0) { y = y + 1; } x = x - 1; } while (x > 0);" "VALID"
run_test "for loop increment" "for (i = 0; i < 5; i = i + 1) { sum = sum + i; }" "VALID"
run_test "for loop decrement" "for (i = 5; i > 0; i = i - 1) { sum = sum + i; }" "VALID"
run_test "for loop with if" "for (i = 0; i < 3; i = i + 1) { if (i != 1) { y = y + i; } }" "VALID"

echo -e "\n${CYAN}${BOLD}Targeted Invalid Cases${RESET}"
run_test "empty input" "" "INVALID"
run_test "missing if braces" "if (x > 5) y = 10;" "INVALID"
run_test "empty block" "while (x < 5) { }" "INVALID"
run_test "bad relational operator" "if (x = 5) { y = 1; }" "INVALID"
run_test "missing while right paren" "while (x < 5 { x = x + 1; }" "INVALID"
run_test "do while missing semicolon" "do { x = x + 1; } while (x < 5)" "INVALID"
run_test "for missing initializer" "for (; i < 5; i = i + 1) { x = 1; }" "INVALID"
run_test "for missing condition" "for (i = 0; ; i = i + 1) { x = 1; }" "INVALID"
run_test "for missing update" "for (i = 0; i < 5; ) { x = 1; }" "INVALID"
run_test "nested while" "while (x < 5) { while (y < 3) { y = y + 1; } x = x + 1; }" "INVALID"
run_test "nested for in while" "while (x < 5) { for (i = 0; i < 3; i = i + 1) { y = y + i; } x = x + 1; }" "INVALID"
run_test "nested do in for" "for (i = 0; i < 3; i = i + 1) { do { y = y + 1; } while (y < 3); }" "INVALID"
run_test "unknown character" "while (x < 5) { y = @; }" "INVALID"

echo -e "\n${CYAN}${BOLD}Generated Valid Loop Cases${RESET}"
ops=(">" "<" ">=" "<=" "==" "!=")
arith=("+" "-" "*" "/")

for i in $(seq 1 60); do
    op="${ops[$((i % ${#ops[@]}))]}"
    ar="${arith[$((i % ${#arith[@]}))]}"
    run_test "generated while valid $i" "while (x $op $i) { y = x $ar $i; x = x + 1; }" "VALID"
done

for i in $(seq 1 60); do
    op="${ops[$((i % ${#ops[@]}))]}"
    ar="${arith[$((i % ${#arith[@]}))]}"
    run_test "generated do while valid $i" "do { y = y $ar $i; x = x + 1; } while (x $op $i);" "VALID"
done

for i in $(seq 1 60); do
    op="${ops[$((i % ${#ops[@]}))]}"
    ar="${arith[$((i % ${#arith[@]}))]}"
    run_test "generated for valid $i" "for (i = 0; i $op $i; i = i + 1) { total = total $ar i; }" "VALID"
done

echo -e "\n${CYAN}${BOLD}Generated Invalid Loop Cases${RESET}"
for i in $(seq 1 20); do
    run_test "generated while invalid missing semicolon $i" "while (x < $i) { x = x + 1 }" "INVALID"
done

for i in $(seq 1 20); do
    run_test "generated do invalid missing while $i" "do { x = x + 1; } (x < $i);" "INVALID"
done

for i in $(seq 1 20); do
    run_test "generated for invalid missing semicolon $i" "for (i = 0 i < $i; i = i + 1) { x = x + 1; }" "INVALID"
done

for i in $(seq 1 20); do
    run_test "generated nested loop invalid $i" "for (i = 0; i < $i; i = i + 1) { while (x < $i) { x = x + 1; } }" "INVALID"
done

echo ""
echo -e "${BOLD}====================================${RESET}"
echo "Total  : $TOTAL"
echo -e "${GREEN}Passed : $PASS${RESET}"
echo -e "${RED}Failed : $FAIL${RESET}"
echo -e "${BOLD}====================================${RESET}"

if [[ $TOTAL -lt 200 ]]; then
    echo -e "${RED}Internal test suite error: fewer than 200 tests were generated${RESET}"
    exit 1
fi

if [[ $FAIL -eq 0 ]]; then
    echo -e "${GREEN}${BOLD}ALL TESTS PASSED${RESET}"
    exit 0
fi

echo -e "${RED}${BOLD}SOME TESTS FAILED${RESET}"
exit 1
