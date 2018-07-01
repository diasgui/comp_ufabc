#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

enum lex {
 ERROR = -1,
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

int line = 1;
int col = 0;
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

void newLine () {
  line++;
  col = 0;
}

Token identify(char currentChar, FILE * arq, int state) {
  switch(state) {
    case BEGIN: {
      switch(currentChar) {
        case '(': return createToken(currentChar, O_PAR, "ABRE_PARENTESIS");
        case ')': return createToken(currentChar, C_PAR, "FECHA_PARENTESIS");
        case '{': return createToken(currentChar, O_KEY, "ABRE_CHAVES");
        case '}': return createToken(currentChar, C_KEY, "FECHA_CHAVES");
        case ',': return createToken(currentChar, COMMA, "VIRGULA");
        case ';': return createToken(currentChar, SEMICOLON, "PONTO_VIRGULA");
        case '>': case '<': return identify(nextChar(currentChar, arq), arq, OP_GT_LT);
        case '=': return identify(nextChar(currentChar, arq), arq, OP_ATTR);
        case '!': return identify(nextChar(currentChar, arq), arq, OP_NOT);
        case '/': return identify(nextChar(currentChar, arq), arq, OP_DIV);
        case '.': return identify(currentChar, arq, T_FLOAT);
        case '"': return identify(nextChar(currentChar, arq), arq, T_STRING);
        case '\n': newLine(); return identify(nextChar(currentChar, arq), arq, BEGIN);
        case EOF: {
          Token t;
          t.tokenType = T_EOF;
          strcpy(t.tokenText, "EOF");
          return t;
        }
        default: {
         if (isdigit(currentChar)) return identify(currentChar, arq, T_FLOAT);
         if (isalpha(currentChar)) return identify(currentChar, arq, T_STRING);
         return createToken(currentChar, ERROR, "ERRO");
        }
      }
    }

    case T_FLOAT: {
      char in_int = currentChar;
      int i = 0;
      char numero[200];
      int isInt = 1;
      numero[i] = in_int; // Salva o que recebeu
      in_int = nextChar(in_int, arq); // Incrementa o char

      // Aqui só podemos ter duas possibilidades, um ponto ou um número
      if (in_int == '.') { // Números que começam com . (.1)
        isInt = 0;
        while (isdigit(in_int)) { // parte ddd do .ddd
          i++;
          numero[i] = in_int;
          in_int = nextChar(in_int, arq);
        }

        if (in_int != 'e' || in_int != 'E') { // Números que contém erros, como .1. ou .1a
          printToken(createTextToken(numero, T_FLOAT, "PONTO_FLUTUANTE"));
          return createTextToken(numero, ERROR, "ERRO");
        }
      }
      else { // Números que não começam com . (111.111)
        while (isdigit(in_int)) {
          i++;
          numero[i] = in_int; // parte ddddd
          in_int = nextChar(in_int, arq);
        }

        if (in_int == '.') {
          isInt = 0;
          i++;
          numero[i] = in_int;
          in_int = nextChar(in_int, arq);
          while (isdigit(in_int)) {
            i++;
            numero[i] = in_int; // parte ddddd.dddd
            in_int = nextChar(in_int, arq);
          }
        }
        else if (in_int != 'e' || in_int != 'E') { // Números que contém erros, como 1.1. ou 1.1a
          printToken(createTextToken(numero, T_FLOAT, "PONTO_FLUTUANTE"));
          return createTextToken(numero, ERROR, "ERRO");
        }
      }

      if (in_int == 'e' || in_int == 'E'){
        isInt = 0;
        i++;
        numero[i] = in_int;

      }

      if (isInt) return createTextToken(numero, T_INTEGER, "INTEIRO");
      if (!isInt) return createTextToken(numero, T_FLOAT, "PONTO_FLUTUANTE");
      return createTextToken(numero, ERROR, "ERRO");
    }

    case T_STRING: { // string
      char string[200];
      char atual = currentChar;
      int i = 0;

      while(atual != '\n' || atual != '"') {
        string[i] = atual;
        atual = nextChar(atual, arq);
        i++;
      }

      if (atual == '\n') return createTextToken(string, ERROR, "ERRO");
      return createTextToken(string, T_STRING, "STRING");
    }

    case OP_GT_LT: { // Operadores >, <, >= e <=
        char token_text[2];
        token_text[0] = lastChar;
        token_text[1] = currentChar;
        return createTextToken(token_text, OP_GE_LE, "OPERADOR_RELACIONAL");
      return createToken(lastChar, OP_GT_LT, "OPERADOR_RELACIONAL");
    }

    case OP_ATTR: {// Operadores = e ==
        if (currentChar == '=') {
        char token_text[2];
        token_text[0] = lastChar;
        token_text[1] = currentChar;
        return createTextToken(token_text, OP_COMP, "OPERADOR_RELACIONAL");
      }
      return createToken(lastChar, OP_ATTR, "ATRIBUICAO");
    }

    case OP_NOT: {// Operadores ! e !=
      if (currentChar == '=') {
        char token_text[2];
        token_text[0] = lastChar;
        token_text[1] = currentChar;
        return createTextToken(token_text, OP_DIFF, "OPERADOR_RELACIONAL");
      }
      return createToken(lastChar, ERROR, "ERRO");
    }
    case OP_DIV: { // Operador / e //
      if (currentChar == '/') return identify(nextChar(currentChar, arq), arq, COMMENT_LINE);
      if (currentChar == '*') return identify(nextChar(currentChar, arq), arq, COMMENT_BLOCK);
      return createToken(lastChar, OP_DIV, "OPERADOR");
    }

    case COMMENT_LINE: { // Comentários de linha
      if (currentChar == '\n') {
        return identify(nextChar(currentChar, arq), arq, BEGIN);
        newLine();
      }
      return identify(nextChar(currentChar, arq), arq, COMMENT_LINE);
    }

    case COMMENT_BLOCK: { // Comentários de bloco
      if (currentChar == '*' && nextChar(currentChar, arq) == '/') return identify(nextChar(currentChar, arq), arq, BEGIN);
      return identify(currentChar, arq, COMMENT_BLOCK);
    }

    case ID: {
      char identificador[200];
      int i = 0;
      char atual = currentChar;
      while (isalnum(atual) || atual == '_') {
        identificador[i] = atual;
        atual = nextChar(atual, arq);
        i++;
      }
    }

    default: return createToken(currentChar, ERROR, "ERRO");
  }
}

int main (int argc, char *argv[]) {
  if (argc <= 1) {
    printf("%s", "Número inválido de argumentos");
    exit(1);
  }
  char ch;
  FILE *arq;

  arq=fopen(argv[1], "r");
  if (arq == NULL) {
    printf("Não foi possível ler o arquivo\n");
    exit(1);
  }

  printf("TOKEN@LEXEMA@LINHA@COLUNA\n"); // Printa o Header
  Token token;

  while((ch=fgetc(arq)) != EOF) {
    token = identify(ch, arq, BEGIN);
    printToken(token);
    printf("%s -> %d (%s)\n", token.tokenText, token.tokenType, token.tokenTypeText);
    col++;
  }

  fclose(arq);
  return 0;
}
