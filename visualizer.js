// DOM Elements
const codeInput = document.getElementById('codeInput');
const btnCompile = document.getElementById('btnCompile');
const btnStep = document.getElementById('btnStep');
const btnPlay = document.getElementById('btnPlay');
const consoleOutput = document.getElementById('consoleOutput');
const currentCharDisplay = document.getElementById('currentChar');
const dfaTokenObj = document.getElementById('dfaToken');
const pdaActionList = document.getElementById('pdaActionList');
const pdaStack = document.getElementById('pdaStack');

// Globals
let animationQueue = [];
let isPlaying = false;
let playInterval = null;

// ==========================================
// 1. COMPILER LOGIC (Ported from C++)
// Generates a trace of animation events
// ==========================================

function compileCode(input) {
    animationQueue = []; // Reset trace
    let tokens = [];
    
    // --- Phase 1: Lexical Analysis (DFA) ---
    let i = 0;
    while (i < input.length) {
        let c = input[i];

        if (/\s/.test(c)) { i++; continue; }

        animationQueue.push({ type: 'DFA_START', char: c });

        if (/[a-zA-Z]/.test(c)) {
            let word = "";
            while (i < input.length && /[a-zA-Z0-9]/.test(input[i])) {
                animationQueue.push({ type: 'DFA_MOVE', char: input[i], to: 's1' });
                word += input[i];
                i++;
            }
            let type = (word === "if" || word === "else") ? "KEYWORD" : "ID";
            tokens.push({ type, value: word });
            animationQueue.push({ type: 'DFA_ACCEPT', node: 's1', val: word });
            continue;
        }

        if (/[0-9]/.test(c)) {
            let num = "";
            while (i < input.length && /[0-9]/.test(input[i])) {
                animationQueue.push({ type: 'DFA_MOVE', char: input[i], to: 's2' });
                num += input[i];
                i++;
            }
            tokens.push({ type: "NUMBER", value: num });
            animationQueue.push({ type: 'DFA_ACCEPT', node: 's2', val: num });
            continue;
        }

        if (/[\=\>\<\!]/.test(c)) {
            animationQueue.push({ type: 'DFA_MOVE', char: c, to: 's3' });
            let op = c;
            i++;
            if (i < input.length && input[i] === '=') {
                animationQueue.push({ type: 'DFA_MOVE', char: '=', to: 's4' });
                op += "=";
                i++;
            }
            tokens.push({ type: "OPERATOR", value: op });
            animationQueue.push({ type: 'DFA_ACCEPT', node: (op.length>1?'s4':'s3'), val: op });
            continue;
        }

        if (c === '&' || c === '|') {
            animationQueue.push({ type: 'DFA_MOVE', char: c, to: 's6' });
            let op = c;
            i++;
            if (i < input.length && input[i] === c) {
                animationQueue.push({ type: 'DFA_MOVE', char: c, to: 's7' });
                op += c;
                tokens.push({ type: "LOGICAL", value: op });
                animationQueue.push({ type: 'DFA_ACCEPT', node: 's7', val: op });
                i++;
            } else {
                tokens.push({ type: "ERROR", value: op });
                animationQueue.push({ type: 'DFA_ERROR', node: 's6' });
            }
            continue;
        }

        if (/[\(\)\{\}\;]/.test(c)) {
            animationQueue.push({ type: 'DFA_MOVE', char: c, to: 's5' });
            tokens.push({ type: "SYMBOL", value: c });
            animationQueue.push({ type: 'DFA_ACCEPT', node: 's5', val: c });
            i++;
            continue;
        }

        tokens.push({ type: "ERROR", value: c });
        i++;
    }

    // --- Phase 2: Syntax Analysis (PDA) ---
    let currentPos = 0;
    
    function peek() { return currentPos < tokens.length ? tokens[currentPos] : {type: "EOF", value:""}; }
    function match(expectedType) {
        let t = peek();
        if (t.type === "EOF") return;
        
        if (t.type === expectedType) {
            if (t.value === "{" || t.value === "(") {
                animationQueue.push({ type: 'PDA_PUSH', val: t.value });
            } else if (t.value === "}" || t.value === ")") {
                animationQueue.push({ type: 'PDA_POP', val: t.value });
            } else {
                animationQueue.push({ type: 'PDA_READ', val: t.value });
            }
            currentPos++;
        } else {
            animationQueue.push({ type: 'PDA_ERROR', val: `Expected ${expectedType} but found ${t.value}` });
        }
    }

    function parseCondition() {
        match("ID");
        match("OPERATOR");
        if (peek().type === "ID") match("ID"); else match("NUMBER");
        
        if (peek().type === "LOGICAL") {
            match("LOGICAL");
            parseCondition();
        }
    }

    function parseBlock() {
        match("SYMBOL"); // {
        match("ID");
        match("OPERATOR"); // =
        if (peek().type === "NUMBER") match("NUMBER"); else match("ID");
        match("SYMBOL"); // ;
        match("SYMBOL"); // }
    }

    function parseIfStatement() {
        match("KEYWORD"); // if
        match("SYMBOL"); // (
        parseCondition();
        match("SYMBOL"); // )
        parseBlock();

        if (peek().value === "else") {
            match("KEYWORD");
            if (peek().value === "if") parseIfStatement();
            else parseBlock();
        }
    }

    if (tokens.length > 0) parseIfStatement();
    animationQueue.push({ type: 'DONE' });
}

// ==========================================
// 2. VISUALIZATION ENGINE
// Iterates through animationQueue and updates DOM
// ==========================================

let animIndex = 0;

function logConsole(msg, color="var(--success)") {
    consoleOutput.innerHTML += `<div style="color:${color}; margin-bottom:4px;">${msg}</div>`;
    consoleOutput.scrollTop = consoleOutput.scrollHeight;
}

function clearActiveStates() {
    document.querySelectorAll('.state').forEach(el => {
        el.classList.remove('active', 'accept', 'error');
    });
}

function moveDfaTokenTo(nodeId) {
    const node = document.getElementById(nodeId);
    if (!node) return;
    dfaTokenObj.classList.remove('hidden');
    // Calculate center relative to canvas
    const rect = node.getBoundingClientRect();
    const canvasRect = document.getElementById('dfaCanvas').getBoundingClientRect();
    dfaTokenObj.style.left = `${rect.left - canvasRect.left + rect.width/2}px`;
    dfaTokenObj.style.top = `${rect.top - canvasRect.top + rect.height/2}px`;
    node.classList.add('active');
}

async function stepAnimation() {
    if (animIndex >= animationQueue.length) {
        logConsole("Simulation Complete!", "#8b5cf6");
        pauseAutoPlay();
        btnStep.disabled = true;
        btnPlay.disabled = true;
        dfaTokenObj.classList.add('hidden');
        clearActiveStates();
        return;
    }

    const event = animationQueue[animIndex++];
    
    switch(event.type) {
        // DFA EVENTS
        case 'DFA_START':
            clearActiveStates();
            moveDfaTokenTo('s0');
            currentCharDisplay.innerText = event.char;
            break;
        case 'DFA_MOVE':
            clearActiveStates();
            moveDfaTokenTo(event.to);
            currentCharDisplay.innerText = event.char;
            break;
        case 'DFA_ACCEPT':
            document.getElementById(event.node).classList.add('accept');
            logConsole(`DFA Accepted Token: [${event.val}]`);
            break;
        case 'DFA_ERROR':
            document.getElementById(event.node).classList.add('error');
            logConsole(`DFA Lexical Error!`, "var(--error)");
            break;

        // PDA EVENTS
        case 'PDA_READ':
            logConsole(`PDA Read: ${event.val}`, "#94a3b8");
            let li1 = document.createElement('li');
            li1.innerText = `Read: ${event.val}`;
            pdaActionList.appendChild(li1);
            li1.scrollIntoView();
            break;
        case 'PDA_PUSH':
            logConsole(`PDA Stack PUSH: ${event.val}`, "var(--accent)");
            let li2 = document.createElement('li');
            li2.innerText = `PUSH: ${event.val}`;
            li2.style.color = "var(--accent)";
            pdaActionList.appendChild(li2);
            li2.scrollIntoView();

            let stackItem = document.createElement('div');
            stackItem.className = 'stack-item';
            stackItem.innerText = event.val;
            // Insert after base (since it's flex column-reverse)
            pdaStack.insertBefore(stackItem, pdaStack.firstChild);
            break;
        case 'PDA_POP':
            logConsole(`PDA Stack POP`, "#f59e0b");
            let li3 = document.createElement('li');
            li3.innerText = `POP (Matched ${event.val})`;
            li3.style.color = "#f59e0b";
            pdaActionList.appendChild(li3);
            li3.scrollIntoView();

            // Find top element (first .stack-item)
            let topItem = pdaStack.querySelector('.stack-item');
            if (topItem) {
                topItem.classList.add('popping');
                setTimeout(() => topItem.remove(), 300);
            }
            break;
        case 'PDA_ERROR':
            logConsole(`PDA Syntax Error: ${event.val}`, "var(--error)");
            break;
    }
}

// ==========================================
// 3. UI CONTROLS
// ==========================================

btnCompile.addEventListener('click', () => {
    // Reset UI
    consoleOutput.innerHTML = "";
    pdaActionList.innerHTML = "";
    document.querySelectorAll('.stack-item').forEach(el => el.remove());
    clearActiveStates();
    dfaTokenObj.classList.add('hidden');
    currentCharDisplay.innerText = "...";
    
    // Compile & Queue
    logConsole("Compiling Code & Generating Trace...");
    compileCode(codeInput.value);
    animIndex = 0;
    
    // Enable controls
    btnStep.disabled = false;
    btnPlay.disabled = false;
    logConsole(`Generated ${animationQueue.length} animation steps. Ready to simulate.`);
});

btnStep.addEventListener('click', () => {
    stepAnimation();
});

function pauseAutoPlay() {
    isPlaying = false;
    btnPlay.innerText = "Play Auto";
    clearInterval(playInterval);
}

btnPlay.addEventListener('click', () => {
    if (isPlaying) {
        pauseAutoPlay();
    } else {
        isPlaying = true;
        btnPlay.innerText = "Pause";
        playInterval = setInterval(stepAnimation, 500); // 500ms per step
    }
});
