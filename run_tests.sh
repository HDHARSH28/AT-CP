#!/bin/bash

# ============================================================
# COMPILER TEST SUITE — FINAL (100+ TESTS)
# ============================================================

GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

PASS=0
FAIL=0
TOTAL=0

# ================= BUILD =================
echo -e "${BOLD}Building compiler...${RESET}"

g++ -o compiler DFA.cpp PDA.cpp main.cpp

if [ $? -ne 0 ]; then
    echo -e "${RED}Build FAILED${RESET}"
    exit 1
fi

echo -e "${GREEN}Build successful!${RESET}"

# ================= RUN TEST =================
run_test() {
    local desc="$1"
    local input="$2"
    local expected="$3"

    TOTAL=$((TOTAL+1))

    # write the test input to a temporary file
    printf '%s' "$input" > input.txt

    # run the compiler (it reads from stdin)
    ./compiler < input.txt > output.txt 2>&1

    # capture the line that contains the verdict
    local result
    result=$(grep -i "syntax is" output.txt || echo "NO VERDICT")

    # build the single‑line report
    local line="[$TOTAL] $desc | Expected: $expected | Got: ${result##*: }"

    if echo "$result" | grep -qi "syntax is $expected"; then
        echo -e "${GREEN}PASS${RESET} $line"
        PASS=$((PASS+1))
    else
        echo -e "${RED}FAIL${RESET} $line"
        FAIL=$((FAIL+1))
    fi
}

# ================= VALID =================
echo -e "\n${CYAN}${BOLD}VALID CASES${RESET}"

run_test "Simple if"                "if (x > 5) { y = 10; }"                     "VALID"
run_test "if-else"                  "if (x > 5) { y = 10; } else { z = 20; }"   "VALID"
run_test "else-if"                  "if (x > 5) { y = 10; } else if (a > 3) { b = 7; }" "VALID"
run_test "nested"                   "if (x > 5) { if (y > 2) { z = 1; } }"      "VALID"
run_test "multi stmt"               "if (x > 5) { a = 1; b = 2; c = 3; }"       "VALID"

# operators
run_test "=="                       "if (x == 5) { y = 1; }"                    "VALID"
run_test ">="                       "if (x >= 5) { y = 1; }"                    "VALID"
run_test "<="                       "if (x <= 5) { y = 1; }"                    "VALID"
run_test "!="                       "if (x != 5) { y = 1; }"                    "VALID"

# conditions
run_test "num-var"                  "if (5 > x) { y = 1; }"                     "VALID"
run_test "var-var"                  "if (a > b) { c = d; }"                     "VALID"
run_test "num-num"                  "if (5 > 3) { x = 1; }"                     "VALID"

# assignments
run_test "assign var"               "if (x > 5) { y = z; }"                     "VALID"
run_test "assign add"               "if (x > 0) { y = x + 1; }"                 "VALID"
run_test "assign mul"               "if (x > 0) { y = x * 2; }"                 "VALID"

# ================= INVALID =================
echo -e "\n${CYAN}${BOLD}INVALID CASES${RESET}"

run_test "missing ;"                "if (x > 5) { y = 10 }"                     "INVALID"
run_test "no braces"                "if (x > 5) y = 10;"                        "INVALID"
run_test "bad cond"                 "if (x 5) { y = 10; }"                      "INVALID"
run_test "empty cond"               "if () { y = 10; }"                         "INVALID"
run_test "else only"                "else { y = 10; }"                          "INVALID"

# ================= EDGE =================
echo -e "\n${CYAN}${BOLD}EDGE CASES${RESET}"

run_test "empty input"              ""                                          "INVALID"
run_test "garbage"                  "@#$%"                                      "INVALID"
run_test "empty block"              "if (x > 5) { }"                            "INVALID"

# ================= AUTO GEN =================
echo -e "\n${CYAN}${BOLD}AUTO TESTS${RESET}"

for i in {1..40}
do
    run_test "auto valid $i" "if (x > $i) { y = $i; }" "VALID"
done

for i in {1..40}
do
    run_test "auto invalid $i" "if (x > $i { y = $i; }" "INVALID"
done

# ================= FINAL SUMMARY =================
echo ""
echo -e "${BOLD}====================================${RESET}"

echo -e "Total  : $TOTAL"
echo -e "${GREEN}Passed : $PASS${RESET}"
echo -e "${RED}Failed : $FAIL${RESET}"

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}${BOLD}ALL TESTS PASSED ✅${RESET}"
else
    echo -e "${RED}${BOLD}SOME TESTS FAILED ❌${RESET}"
fi

echo -e "${BOLD}====================================${RESET}"

# exit status
[ $FAIL -eq 0 ] && exit 0 || exit 1