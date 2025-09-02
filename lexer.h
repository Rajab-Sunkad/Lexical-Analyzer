#ifndef LEXER_H
#define LEXER_H

#define MAX_KEYWORDS 20
#define MAX_TOKEN_SIZE 100

typedef enum {
    KEYWORD,
    OPERATOR,
    SPECIAL_CHARACTER, // Less used now
    CONSTANT,          // For string/char literals
    INTEGRAL_CONSTANT, // For valid integer numbers (decimal, hex, oct, bin)
    IDENTIFIER,
    SYMBOL,
    INVALID_NUMBER,    // <--- NEW: For ill-formed numbers
    UNKNOWN
} TokenType;

typedef struct {
    char lexeme[MAX_TOKEN_SIZE];
    TokenType type;
} Token;

void initializeLexer(const char* filename);
Token getNextToken();
void categorizeToken(Token* token); // Still declared but largely unused
int isKeyword(const char* str);
int isOperator(const char* str);
int isSymbolCharacter(char ch);
int isConstant(const char* str); // For checking if a string IS a constant, but now we'll have more specific types
int isIdentifier(const char* str);

const char* getTokenTypeString(TokenType type);
void closeLexer();

#endif