#include <stdio.h>
#include<string.h>
#include "lexer.h" // Include your lexer header

int main(int argc, char* argv[]) 
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename.c>\n", argv[0]);
        return 1;
    }

    initializeLexer(argv[1]);

    printf("Parsing : %s : Started\n", argv[1]);

    Token currentToken;
    do 
    {
        currentToken = getNextToken();
        if (strlen(currentToken.lexeme) > 0) // Only print if a lexeme was found
        { 
            // Print with fixed-width columns for alignment
            printf("%-20s: %s\n", getTokenTypeString(currentToken.type), currentToken.lexeme);
        }
    } while (currentToken.type != UNKNOWN || strlen(currentToken.lexeme) > 0); // Continue until true EOF

    printf("Parsing : %s : Done\n", argv[1]);

    closeLexer(); // Call closeLexer to report any pending errors

    return 0;
}