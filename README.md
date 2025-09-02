# Lexical-Analyzer
Lexical Analyzer is the first phase of a compiler that converts a stream of characters from a source program into a sequence of tokens.   This project implements a simple lexical analyzer in C, which scans a '.c'  file and classifies tokens such as keywords, identifiers, constants, operators, and symbols.

---

## ⚡ Features
- Reads input C source file line by line
- Identifies and classifies:
  - Keywords
  - Identifiers
  - Numeric constants (decimal, octal, hexadecimal, binary)
  - String and character literals
  - Operators (`+`, `-`, `*`, `/`, `%`, `==`, etc.)
  - Delimiters and symbols (`;`, `{`, `}`, `(`, `)`, `,`, etc.)
- Reports lexical errors (invalid tokens)

---
## 🛠️ Technologies Used
- **Language:** C
- **Concepts:** File Handling, String Handling, Symbol Tables, State Machine
- **Compiler:** GCC

---

## 🎯 Use of the Lexical Analyzer
The use of this project is to convert raw source code into meaningful tokens, which makes it possible for compilers, interpreters, and code tools to understand and process programming languages.

---

## 🖥️ Usage
```
# Compile the project
gcc main.c lexer.c -o lexer

# Run with a test C source file
./lexer test.c
```

