#include <stdio.h>
#include <string.h>
#define ERROR = -1

enum lex {
 BEGIN = 1,
 O_PAR = 23,
 C_PAR = 25,
 O_KEY = 27,
 C_KEY = 29,
 COMMA = 31,
 SEMICOLON = 21,
 T_INTEGER = 5,
 T_STRING = 13,
 OP_GE_LE = 38,
 OP_GT_LT = 33,
 OP_ATTR = 19,
 OP_COMP = 38,
 OP_DIFF = 38,
 ID = 17,
 OP_MINUS = 2,
 OP_DECR = 40,
 OP_PLUS = 14,
 OP_INCR = 40,
 OP_MULT = 36,
 OP_MOD = 36,
 OP_DIV = 26,
 OP_NOT = 41,
 OP_OR = 36,
 OP_AND = 36,
 COMMENT_LINE = 11,
 COMMENT_BLOCK = 22,
 T_FLOAT = 6,
 T_FLOAT2 = 7,
 T_EXP = 10,
 T_EXP2 = 43,
 T_EOF = 4896,
 IGNORE = -1000
} LEXEMAS;

typedef struct TokenStruct {
	char tokenText[200];
	int tokenType;
  char tokenTypeText[40];
  int tokenLine;
  int tokenCol;
} Token;

int line = 0;
int col = 1;
char lastChar;

Token createToken (char text, int type, char typeText[]) {
	Token t;
	t.tokenType = type;
  t.tokenLine = line;
  t.tokenCol = col;
  t.tokenText[0] = text;
  strcpy(t.tokenTypeText, typeText);
	return t;
}

Token createTextToken (char text[], int type, char typeText[]) {
	Token t;
	t.tokenType = type;
  t.tokenCol = col;
  t.tokenLine = line;
  strcpy(t.tokenTypeText, typeText);
	strcpy(t.tokenText, text);
	return t;
}

char nextChar(char currentChar, FILE *arq) {
	lastChar = currentChar;
	col++;
	return fgetc(arq);
}

void printToken (Token t) {
  if(t.tokenType != IGNORE) printf("%s@%s@%d@%d\n", t.tokenTypeText, t.tokenText, t.tokenLine, t.tokenCol);
}

Token identify(char currentChar, FILE * arq, int state) {
	switch(state) {
		case BEGIN:
			switch(currentChar) {
        case '(': state=O_PAR; return createToken(currentChar, O_PAR, "O_PAR");
        case ')': state=C_PAR; return createToken(currentChar, C_PAR, "C_PAR");
        case '{': state=O_KEY; return createToken(currentChar, O_KEY, "O_KEY");
        case '}': state=C_KEY; return createToken(currentChar, C_KEY, "C_KEY");
        case ',': state=COMMA; return createToken(currentChar, COMMA, "COMMA");
        case ';': state=SEMICOLON; return createToken(currentChar, SEMICOLON, "SEMICOLON");
				case '>': case '<': return identify(nextChar(currentChar, arq), arq, OP_GT_LT);
				case '=': return identify(nextChar(currentChar, arq), arq, OP_ATTR);
				case '!': return identify(nextChar(currentChar, arq), arq, OP_NOT);
				case EOF: ;
          Token t;
          t.tokenType = T_EOF;
          strcpy(t.tokenText, "EOF");
          return t;
				default:
				 // outros identificadores
				 // erro
				 break;
			}
			break;
		case OP_GT_LT:
			if (currentChar == '=') {
				char token_text[2];
				token_text[0] = lastChar;
				token_text[1] = currentChar;
        return createTextToken(token_text, OP_GE_LE, "OP_GE_LE");
			}
      return createToken(lastChar, OP_GT_LT, "OP_GT_LT");
		case OP_ATTR:
			if (currentChar == '=') {
				char token_text[2];
				token_text[0] = lastChar;
				token_text[1] = currentChar;
        return createTextToken(token_text, OP_COMP, "OP_COMP");
			}
      return createToken(lastChar, OP_ATTR, "OP_ATTR");
		case OP_NOT:
			if (currentChar == '=') {
				char token_text[2];
				token_text[0] = lastChar;
				token_text[1] = currentChar;
        return createTextToken(token_text, OP_DIFF, "OP_DIFF");
			}
      return createToken(lastChar, OP_NOT, "OP_NOT");

	}
}

int main (args) {
	char file[]="teste.jl";
	char ch;
	FILE *arq;

	arq=fopen(file, "r");
	if (arq == NULL) {
		printf("Não foi possível ler o arquivo\n");
		return 1;
	}

	Token token;
	//printf("TOKEN@LEXEMA@LINHA@COLUNA");
	while((ch=fgetc(arq))!=EOF) {
		col++;
		token = identify(ch, arq, BEGIN);
		// jeito correto de imprimir
		//printf("%s@%d@%d@%d\n", token.tokenText, token.tokenType, line, col);
		// print pra controle
		printf("%s -> %d\n", token.tokenText, token.tokenType);
	}

	fclose(arq);
	return 0;
}
