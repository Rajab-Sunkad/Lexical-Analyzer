#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h> // For exit and EXIT_FAILURE
#include "lexer.h"

// Static global variables for lexer state
static FILE* inputFile;
static char currentChar;
static int eofFlag = 0; // To indicate if EOF has been reached
static int lineNum = 1; // Track current line number for better error messages

// Counters for unmatched delimiters
static int paren_count = 0;   // ()
static int brace_count = 0;   // {}
static int bracket_count = 0; // []

// Predefined lists
static const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

static const char* operators = "+-*/%=!<>|&^~";
static const char* symbols = "(),;{}[]";

// --- Helper function to get the next character ---
static void getNextChar() 
{
    if (!eofFlag) 
    {
        int c = fgetc(inputFile);
        if (c == EOF) 
        {
            eofFlag = 1;
            currentChar = '\0'; // Null terminator to signify end
        } 
        else 
        {
            currentChar = (char)c;
            if (currentChar == '\n')
            {
                lineNum++; // Increment line number on newline
            }
        }
    }
}


// --- Function Implementations ---

void initializeLexer(const char* filename) 
{
    inputFile = fopen(filename, "r");
    if (inputFile == NULL) 
    {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
    printf("Open   : %s : Success\n", filename);
    getNextChar(); // Read the first character
    // Reset counts and line number for new file
    paren_count = 0;
    brace_count = 0;
    bracket_count = 0;
    lineNum = 1;
}

void closeLexer() 
{
    if (inputFile != NULL) 
    {
        fclose(inputFile);
        inputFile = NULL;
    }
    // Report any unmatched delimiters at EOF
    if (paren_count > 0) 
    {
        fprintf(stderr, "Error at EOF: Missing ')' (unmatched opening parentheses)\n");
    }
    if (brace_count > 0) 
    {
        fprintf(stderr, "Error at EOF: Missing '}' (unmatched opening brace)\n");
    }
    if (bracket_count > 0) 
    {
        fprintf(stderr, "Error at EOF: Missing ']' (unmatched opening bracket)\n");
    }
}

int isKeyword(const char* str) 
{
    for (int i = 0; i < MAX_KEYWORDS; i++) 
    {
        if (keywords[i] == NULL) 
        {
            break;
        }
        if (strcmp(str, keywords[i]) == 0) 
        {
            return 1;
        }
    }
    return 0;
}

int isOperator(const char* str) 
{
    if (strlen(str) == 1) 
    {
        for (int i = 0; operators[i] != '\0'; i++) 
        {
            if (str[0] == operators[i]) 
            {
                return 1;
            }
        }
    }
    return 0;
}

// Check if a character is one of the designated 'SYMBOL' characters
int isSymbolCharacter(char ch) 
{
    for (int i = 0; symbols[i] != '\0'; i++) 
    {
        if (ch == symbols[i]) 
        {
            return 1;
        }
    }
    return 0;
}

int isConstant(const char* str) 
{
    // This function is less relevant now as type is assigned directly in getNextToken
    if (isdigit(str[0])) 
    {
        for (int i = 1; str[i] != '\0'; i++) 
        {
            if (!isdigit(str[i])) 
            {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

int isIdentifier(const char* str) 
{
    if (!isalpha(str[0]) && str[0] != '_') 
    {
        return 0;
    }
    for (int i = 1; str[i] != '\0'; i++) 
    {
        if (!isalnum(str[i]) && str[i] != '_') {
            return 0;
        }
    }
    return 1;
}

void categorizeToken(Token* token) 
{
    // This function is mostly a placeholder now as getNextToken directly assigns types.
    if (isKeyword(token->lexeme)) 
    {
        token->type = KEYWORD;
    } 
    else if (isIdentifier(token->lexeme)) 
    {
        token->type = IDENTIFIER;
    } 
    else if (isConstant(token->lexeme)) 
    {
        token->type = INTEGRAL_CONSTANT;
    } 
    else if (isOperator(token->lexeme)) 
    {
        token->type = OPERATOR;
    } 
    else if (isSymbolCharacter(token->lexeme[0]) && strlen(token->lexeme) == 1) 
    {
        token->type = SYMBOL;
    } 
    else 
    {
        token->type = UNKNOWN;
    }
}

Token getNextToken() 
{
    Token token;
    memset(&token, 0, sizeof(Token));

    while (1) 
    {
        // Skip whitespace
        while (isspace(currentChar) && !eofFlag) 
        {
            getNextChar();
        }

        // If EOF is reached after skipping, return UNKNOWN with empty lexeme
        if (eofFlag) 
        {
            token.type = UNKNOWN;
            strcpy(token.lexeme, "");
            return token;
        }

        // --- Handle Preprocessor Directives (lines starting with #) ---
        if (currentChar == '#') 
        {
            while (currentChar != '\n' && !eofFlag) 
            {
                getNextChar();
            }
            if (currentChar == '\n') 
            {
                getNextChar();
            }
            continue;
        }

        // --- Handle Comments ---
        if (currentChar == '/') 
        {
            char next = fgetc(inputFile);
            if (next == EOF) 
            {
                ungetc(next, inputFile);
                break;
            }
            ungetc(next, inputFile);

            if (next == '/') 
            { // Single-line comment //
                getNextChar();
                getNextChar();
                while (currentChar != '\n' && !eofFlag) 
                {
                    getNextChar();
                }
                if (currentChar == '\n') 
                {
                    getNextChar();
                }
                continue;
            } 
            else if (next == '*') 
            { // Multi-line comment /* ... */
                getNextChar();
                getNextChar();
                int prevChar = 0;
                while (!eofFlag && !(prevChar == '*' && currentChar == '/')) 
                {
                    prevChar = currentChar;
                    getNextChar();
                }
                if (eofFlag) 
                {
                    fprintf(stderr, "Error at line %d: Unclosed multi-line comment '/*'\n", lineNum);
                    token.type = UNKNOWN;
                    strcpy(token.lexeme, "");
                    return token;
                }
                getNextChar(); // Consume the '/' of "*/"
                continue;
            }
        }
        break; // If not whitespace, directive, or comment, break loop to tokenize
    }

    // Now, actual tokenization logic starts after skipping leading non-code elements
    // 1. Check for string literals (e.g., "Hello World\n")
    if (currentChar == '"') 
    {
        int i = 0;
        int startLine = lineNum;
        token.lexeme[i++] = currentChar;
        getNextChar();
        while (currentChar != '"' && currentChar != '\n' && !eofFlag && i < MAX_TOKEN_SIZE - 2) 
        {
            if (currentChar == '\\') 
            {
                token.lexeme[i++] = currentChar;
                getNextChar();
                if (!eofFlag) 
                {
                    token.lexeme[i++] = currentChar;
                    getNextChar();
                }
            } 
            else 
            {
                token.lexeme[i++] = currentChar;
                getNextChar();
            }
        }
        if (currentChar == '"') 
        {
            token.lexeme[i++] = currentChar;
            getNextChar();
            token.lexeme[i] = '\0';
            token.type = CONSTANT; // String literal is a generic constant type
            return token;
        } 
        else 
        {
            token.type = UNKNOWN; // Remains UNKNOWN for unclosed string, but error message will be detailed
            token.lexeme[i] = '\0';
            fprintf(stderr, "Error at line %d: Missing '\"' (unclosed string literal) after \"%s\n", startLine, token.lexeme);
            while (currentChar != '\n' && !eofFlag) 
            {
                getNextChar();
            }
            if (currentChar == '\n') 
            {
                getNextChar();
            }
            return getNextToken();
        }
    }

    // 2. Check for character literals (e.g., 'a', '\n')
    if (currentChar == '\'') 
    {
        int i = 0;
        int startLine = lineNum;
        token.lexeme[i++] = currentChar; // Store opening quote
        getNextChar();

        // Handle content (single character or escape sequence)
        if (currentChar == '\\') 
        { // Escape sequence
            token.lexeme[i++] = currentChar;
            getNextChar();
            if (!eofFlag && (currentChar == '\'' || currentChar == '\\' || currentChar == 'n' ||
                             currentChar == 't' || currentChar == 'b' || currentChar == 'r' ||
                             currentChar == 'f' || currentChar == 'a' || currentChar == 'v' ||
                             isdigit(currentChar))) 
                             {
                token.lexeme[i++] = currentChar;
                getNextChar();
            } 
            else 
            {
                fprintf(stderr, "Warning at line %d: Invalid escape sequence in character literal\n", startLine);
                if(!eofFlag) { token.lexeme[i++] = currentChar; getNextChar(); }
            }
        } 
        else if (currentChar != '\'' && !eofFlag && currentChar != '\n') // Single character
        { 
            token.lexeme[i++] = currentChar;
            getNextChar();
        }

        if (currentChar == '\'') 
        { // Closing quote
            token.lexeme[i++] = currentChar;
            getNextChar();
            token.lexeme[i] = '\0';
            token.type = CONSTANT; // Character literal is a generic constant type
            return token;
        } 
        else 
        {
            token.type = UNKNOWN; // Remains UNKNOWN for unclosed char literal
            token.lexeme[i] = '\0';
            fprintf(stderr, "Error at line %d: Missing ''' (unclosed character literal) after '%s\n", startLine, token.lexeme);
            while (currentChar != '\n' && !eofFlag && currentChar != ';') 
            {
                getNextChar();
            }
            if (currentChar == '\n') getNextChar();
            return getNextToken();
        }
    }

    // 3. Check for identifiers and keywords
    if (isalpha(currentChar) || currentChar == '_') 
    {
        int i = 0;
        int startLine = lineNum; // Store line for identifier error
        token.lexeme[i++] = currentChar;
        getNextChar();
        while ((isalnum(currentChar) || currentChar == '_') && !eofFlag && i < MAX_TOKEN_SIZE - 1) 
        {
            token.lexeme[i++] = currentChar;
            getNextChar();
        }
        token.lexeme[i] = '\0';

        // Validate the identifier after it's fully read
        if (!isIdentifier(token.lexeme)) // isIdentifier checks starting char, but this is a double check
        { 
            fprintf(stderr, "Error at line %d: Invalid identifier '%s'. Identifiers must start with a letter or underscore.\n", startLine, token.lexeme);
            token.type = UNKNOWN; // Mark as UNKNOWN because it's fundamentally not an identifier
            return token;
        }

        if (isKeyword(token.lexeme)) 
        {
            token.type = KEYWORD;
        } 
        else 
        {
            token.type = IDENTIFIER;
        }
        return token;
    }

    // 4. Check for numeric constants (Integral_Constant with base validation)
    if (isdigit(currentChar)) 
    {
        int i = 0;
        int startLine = lineNum;
        token.lexeme[i++] = currentChar;
        getNextChar();

        // Handle 0x (hexadecimal) and 0b (binary) prefixes
        if (token.lexeme[0] == '0' && (currentChar == 'x' || currentChar == 'X')) 
        {
            token.lexeme[i++] = currentChar; // Store 'x' or 'X'
            getNextChar();
            int hasDigits = 0;
            while (isxdigit(currentChar) && !eofFlag && i < MAX_TOKEN_SIZE - 1) 
            {
                token.lexeme[i++] = currentChar;
                getNextChar();
                hasDigits = 1;
            }
            token.lexeme[i] = '\0';
            if (!hasDigits) 
            {
                fprintf(stderr, "Error at line %d: Hexadecimal literal '0%c' must be followed by hexadecimal digits (0-9, A-F).\n", startLine, token.lexeme[1]);
                token.type = INVALID_NUMBER; // Specific type for invalid number format
                return token;
            }
            // Check for invalid characters immediately after a valid hex number
            if (isalnum(currentChar) || currentChar == '_') 
            {
                 fprintf(stderr, "Error at line %d: Invalid character '%c' in hexadecimal literal '%s'.\n", startLine, currentChar, token.lexeme);
                 token.type = INVALID_NUMBER;
                 getNextChar(); // Consume the invalid character
                 return token;
            }
            token.type = INTEGRAL_CONSTANT;
            return token;
        }
        else if (token.lexeme[0] == '0' && (currentChar == 'b' || currentChar == 'B')) 
        {
            token.lexeme[i++] = currentChar; // Store 'b' or 'B'
            getNextChar();
            int hasDigits = 0;
            while ((currentChar == '0' || currentChar == '1') && !eofFlag && i < MAX_TOKEN_SIZE - 1) 
            {
                token.lexeme[i++] = currentChar;
                getNextChar();
                hasDigits = 1;
            }
            token.lexeme[i] = '\0';
            if (!hasDigits) 
            {
                fprintf(stderr, "Error at line %d: Binary literal '0%c' must be followed by binary digits (0 or 1).\n", startLine, token.lexeme[1]);
                token.type = INVALID_NUMBER; // Specific type for invalid number format
                return token;
            }
            // Check for invalid characters after binary digits
            if (isalnum(currentChar) || currentChar == '_') 
            {
                 fprintf(stderr, "Error at line %d: Invalid character '%c' in binary literal '%s'.\n", startLine, currentChar, token.lexeme);
                 token.type = INVALID_NUMBER;
                 // Consume the invalid character to continue
                 getNextChar();
                 return token;
            }
            token.type = INTEGRAL_CONSTANT;
            return token;
        }
        else if (token.lexeme[0] == '0' && isdigit(currentChar)) // Octal (starts with 0, followed by digits 0-7)
        { 
            while (isdigit(currentChar) && !eofFlag && i < MAX_TOKEN_SIZE - 1) 
            {
                if (currentChar >= '8' && currentChar <= '9') // Use range correctly for '8' and '9'
                { 
                    fprintf(stderr, "Error at line %d: Invalid digit '%c' in octal literal '0%s'. Octal digits must be 0-7.\n", startLine, currentChar, token.lexeme + 1);
                    token.lexeme[i++] = currentChar; // Add invalid char for error reporting
                    getNextChar();
                    token.lexeme[i] = '\0';
                    token.type = INVALID_NUMBER; // Specific type for invalid number format
                    // Consume the rest of the invalid number-like sequence
                    while(isalnum(currentChar) && !eofFlag) getNextChar();
                    return token;
                }
                token.lexeme[i++] = currentChar;
                getNextChar();
            }
            // Check for invalid characters immediately after a valid octal number
            if (isalnum(currentChar) || currentChar == '_') 
            {
                 fprintf(stderr, "Error at line %d: Invalid character '%c' in octal literal '%s'.\n", startLine, currentChar, token.lexeme);
                 token.type = INVALID_NUMBER;
                 getNextChar(); // Consume the invalid character
                 return token;
            }
            token.lexeme[i] = '\0';
            token.type = INTEGRAL_CONSTANT;
            return token;
        }
        else { // Decimal literal (starts with non-zero digit, or just '0' if not followed by x/b)
            while (isdigit(currentChar) && !eofFlag && i < MAX_TOKEN_SIZE - 1) 
            {
                token.lexeme[i++] = currentChar;
                getNextChar();
            }
            // Check for invalid characters immediately after a valid decimal number
            if (isalnum(currentChar) || currentChar == '_') 
            {
                 fprintf(stderr, "Error at line %d: Invalid character '%c' in decimal literal '%s'.\n", startLine, currentChar, token.lexeme);
                 token.type = INVALID_NUMBER;
                 getNextChar(); // Consume the invalid character
                 return token;
            }
            token.lexeme[i] = '\0';
            token.type = INTEGRAL_CONSTANT;
            return token;
        }
    }

    // 5. Check for operators (multi-character first, then single)
    char temp_lexeme[4] = {0};
    temp_lexeme[0] = currentChar;
    getNextChar();

    // Check for common 2-char operators (and potential 3-char like <<=)
    if ( (temp_lexeme[0] == '=' && currentChar == '=') ||
         (temp_lexeme[0] == '!' && currentChar == '=') ||
         (temp_lexeme[0] == '+' && currentChar == '+') ||
         (temp_lexeme[0] == '-' && currentChar == '-') ||
         (temp_lexeme[0] == '&' && currentChar == '&') ||
         (temp_lexeme[0] == '|' && currentChar == '|') ||
         (temp_lexeme[0] == '/' && currentChar == '=') ||
         (temp_lexeme[0] == '*' && currentChar == '=') ||
         (temp_lexeme[0] == '%' && currentChar == '=') ||
         (temp_lexeme[0] == '^' && currentChar == '=') ||
         (temp_lexeme[0] == '~' && currentChar == '=')
        ) {
        temp_lexeme[1] = currentChar;
        temp_lexeme[2] = '\0';
        getNextChar();
        strcpy(token.lexeme, temp_lexeme);
        token.type = OPERATOR;
        return token;
    } 
    else if (
        (temp_lexeme[0] == '<' && currentChar == '<') ||
        (temp_lexeme[0] == '>' && currentChar == '>')
    ) 
    {
        temp_lexeme[1] = currentChar;
        temp_lexeme[2] = '\0';
        getNextChar();
        if (!eofFlag && currentChar == '=') 
        {
            temp_lexeme[2] = currentChar;
            temp_lexeme[3] = '\0';
            getNextChar();
        }
        strcpy(token.lexeme, temp_lexeme);
        token.type = OPERATOR;
        return token;
    }
    // 6. Check for single character operators or SYMBOLS
    else if (isOperator(temp_lexeme)) 
    {
        strcpy(token.lexeme, temp_lexeme);
        token.type = OPERATOR;
        return token;
    }
    else if (isSymbolCharacter(temp_lexeme[0]))
    {
        strcpy(token.lexeme, temp_lexeme);
        token.type = SYMBOL;
        // Update counts for delimiters
        if (token.lexeme[0] == '(') paren_count++;
        else if (token.lexeme[0] == ')') paren_count--;
        else if (token.lexeme[0] == '{') brace_count++;
        else if (token.lexeme[0] == '}') brace_count--;
        else if (token.lexeme[0] == '[') bracket_count++;
        else if (token.lexeme[0] == ']') bracket_count--;

        // Simple error check for premature closing
        if (paren_count < 0) 
        {
            fprintf(stderr, "Error at line %d: Unmatched ')'\n", lineNum);
            paren_count = 0;
        }
        if (brace_count < 0) 
        {
            fprintf(stderr, "Error at line %d: Unmatched '}'\n", lineNum);
            brace_count = 0;
        }
        if (bracket_count < 0) 
        {
            fprintf(stderr, "Error at line %d: Unmatched ']'\n", lineNum);
            bracket_count = 0;
        }
        return token;
    }

    // If none of the above, it's an UNKNOWN token
    if (!eofFlag) 
    {
        token.lexeme[0] = currentChar;
        token.lexeme[1] = '\0';
        token.type = UNKNOWN;
        fprintf(stderr, "Warning: Unknown token '%s' at line %d\n", token.lexeme, lineNum);
        getNextChar();
        return token;
    }

    token.type = UNKNOWN;
    strcpy(token.lexeme, "");
    return token;
}

// Function to return string representation of TokenType
const char* getTokenTypeString(TokenType type) 
{
    switch (type) 
    {
        case KEYWORD:
            return "Keyword";
        case OPERATOR:
            return "Operator";
        case SYMBOL:
            return "Symbol";
        case CONSTANT: // For string and char literals
            return "Literal";
        case INTEGRAL_CONSTANT: // String for INTEGRAL_CONSTANT type
            return "Integral constant";
        case IDENTIFIER:
            return "Identifier";
        case INVALID_NUMBER: // <--- NEW: String for INVALID_NUMBER type
            return "Invalid number";
        case UNKNOWN:
            return "Unknown";
        case SPECIAL_CHARACTER: // Fallback, should not be hit
            return "Special Character (Fallback)";
        default:
            return "Invalid Type";
    }
}