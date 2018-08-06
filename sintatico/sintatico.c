#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include "sintatico.h"
enum states
{
  Q_FUNC,
  Q_PARAM,
  Q_INCR,
  Q_INSTRUCAO,
  Q_ENDLINE,
  Q_COND,
  Q_IF,
  Q_RETURN
} ESTADOS;

char *line = NULL;   // Linha que está sendo lida no momento
size_t line_len = 0; // Não faço ideia do que seja, mas é parâmetro do getLine
ssize_t read_line;   // Também não faço ideia do que seja, mas é parâmetro do getLine
int hasError = 0;

Token createTokenFromCurrentLine()
{
  Token currentToken;                                    // Cria a estrutura do token
  strcpy(currentToken.tokenTypeText, strtok(line, "@")); // Preenche o tipo do token
  strcpy(currentToken.tokenText, strtok(NULL, "@"));     // Preenche o texto referente ao token
  currentToken.tokenLine = atoi(strtok(NULL, "@"));      // Preenche a linha
  currentToken.tokenCol = atoi(strtok(NULL, "@"));       // Preenche a coluna
  return currentToken;
}

Token nextToken(FILE *arq)
{
  read_line = getline(&line, &line_len, arq); // Vai ler a primeira linha
  return createTokenFromCurrentLine();
}

void printError(Token t, char *message)
{
  hasError = 1;
  printf("Erro ao compilar \"%s\": %s (Lin: %d | Col: %d)\n", t.tokenText, message, t.tokenLine, t.tokenCol);
}

int A_PARENTESES(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "ABRE_PARENTESES") == 0;
}

int F_PARENTESES(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "FECHA_PARENTESES") == 0;
}

int PR_VOID(Token currentToken)
{
  return strcmp(currentToken.tokenText, "void") == 0;
}

int PR_BOOL(Token currentToken)
{
  return strcmp(currentToken.tokenText, "boolean") == 0;
}

int PR_FLOAT(Token currentToken)
{
  return strcmp(currentToken.tokenText, "double") == 0;
}

int PR_INT(Token currentToken)
{
  return strcmp(currentToken.tokenText, "int") == 0;
}

int P_VIRGULA(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "PONTO_VIRGULA") == 0;
}

int A_CHAVES(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "ABRE_CHAVES") == 0;
}

int F_CHAVES(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "FECHA_CHAVES") == 0;
}

int VIRGULA(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "VIRGULA") == 0;
}

int ATTR(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "ATRIBUICAO") == 0;
}

int OP_BOOL(Token currentToken)
{
  return strcmp(currentToken.tokenText, "||") == 0 || strcmp(currentToken.tokenText, "&&") == 0 || strcmp(currentToken.tokenText, "!") == 0;
}

int OP_NUM(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "OPERADOR") == 0;
}

int OP_REL_NUM(Token currentToken)
{
  return strcmp(currentToken.tokenText, ">") == 0 || strcmp(currentToken.tokenText, ">=") == 0 || strcmp(currentToken.tokenText, "<") == 0 || strcmp(currentToken.tokenText, "<=") == 0 || strcmp(currentToken.tokenText, "==") == 0;
}

int INCR_ID(Token currentToken)
{
  return strcmp(currentToken.tokenText, "++") == 0 || strcmp(currentToken.tokenText, "--") == 0;
}

int PR_WHILE(Token currentToken)
{
  return strcmp(currentToken.tokenText, "while") == 0;
}

int PR_IF(Token currentToken)
{
  return strcmp(currentToken.tokenText, "if") == 0;
}

int PR_ELSE(Token currentToken)
{
  return strcmp(currentToken.tokenText, "else") == 0;
}

int PR_RETURN(Token currentToken)
{
  return strcmp(currentToken.tokenText, "return") == 0;
}

int INT(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "INTEIRO") == 0;
}

int FLOAT(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "PONTO_FLUTUANTE") == 0;
}

int T_BOOL(Token currentToken)
{
  return strcmp(currentToken.tokenTypeText, "true") == 0 || strcmp(currentToken.tokenTypeText, "false") == 0;
}

void COND(Token currentToken, int state, FILE *arq)
{
  Token newToken;

  if (INT(currentToken) || T_BOOL(currentToken))
  {
    newToken = nextToken(arq);
    if (OP_REL_NUM(newToken) || OP_BOOL(newToken))
    {
      COND(newToken, state, arq);
    }
  }
  else if (INCR_ID(currentToken))
  {
    newToken = nextToken(arq);
    IDENTIFICADOR(newToken, Q_ENDLINE, arq);
  }

  if (state == Q_IF)
  {
    if (F_PARENTESES(currentToken))
    {
      newToken = nextToken(arq);
      if (A_CHAVES(newToken))
      {
        newToken = nextToken(arq);
        INSTRUCAO(newToken, arq);
      }
      else
      {
        printError(newToken, "Esperado '{'");
      }
    }
  }
  else
  {
    IDENTIFICADOR(newToken, Q_COND, arq); // tratar casos id, id OP_BOOL id COND2
  }
}

void INSTRUCAO(Token currentToken, FILE *arq)
{
  Token newToken;
  if (F_CHAVES(currentToken))
  {
    return;
  }
  else if (PR_IF(currentToken) || PR_WHILE(currentToken))
  {
    newToken = nextToken(arq);
    if (A_PARENTESES(newToken))
    {
      newToken = nextToken(arq);
      COND(newToken, Q_IF, arq);
    }
    else
    {
      printError(newToken, "Esperado '('");
    }
  }
  else if (INCR_ID(currentToken)) //++id || --id
  {
    newToken = nextToken(arq); // PONTO_VIRGULA
    IDENTIFICADOR(newToken, Q_ENDLINE, arq);
  }
  else if (PR_RETURN(currentToken)) // return
  {
    newToken = nextToken(arq);
    IDENTIFICADOR(newToken, Q_RETURN, arq);
  }
  else
  {
    IDENTIFICADOR(currentToken, Q_INSTRUCAO, arq);
  }
}

void PARAM(Token currentToken, FILE *arq)
{
  Token newToken;
  if (F_PARENTESES(currentToken))
  {
    newToken = nextToken(arq);
    if (A_CHAVES(newToken))
    {
      INSTRUCAO(nextToken(arq), arq);
    }
    else
    {
      printError(newToken, "Esperado '{'");
    }
  }
  else if (PR_INT(currentToken) || PR_FLOAT(currentToken) || PR_BOOL(currentToken))
  {
    newToken = nextToken(arq);
    IDENTIFICADOR(newToken, Q_PARAM, arq);
  }
  else
  {
    printError(currentToken, "Esperado ')', 'int', 'double' ou 'bool'");
  }
}

void IDENTIFICADOR(Token currentToken, int state, FILE *arq)
{
  Token newToken;
  if (strcmp(currentToken.tokenTypeText, "IDENTIFICADOR") == 0)
  {
    newToken = nextToken(arq);
    switch (state)
    {
    case Q_FUNC:
    {
      if (A_PARENTESES(newToken))
      {
        newToken = nextToken(arq);
        PARAM(newToken, arq);
      }
      else
      {
        printError(newToken, "Esperado um '('");
      }
      break;
    }
    case Q_PARAM:
    {
      if (VIRGULA(newToken) || F_PARENTESES(newToken))
      {
        PARAM(newToken, arq);
      }
      else
      {
        printError(newToken, "Esperado ',' ou ')'");
      }
      break;
    }
    case Q_ENDLINE:
    {
      if (P_VIRGULA(newToken))
      {
        return;
      }
      printError(newToken, "Esperado ';'");
      break;
    }
    case Q_RETURN:
    {
      newToken = nextToken(arq);
      if (P_VIRGULA(newToken))
      {

        return;
      }
      else
      {
        IDENTIFICADOR(newToken, state, arq);
      }
    }
    case Q_INSTRUCAO:
    {
    }
    default:
    {
      newToken = nextToken(arq);
      if (INCR_ID(newToken))
      {
        newToken = nextToken(arq);
        if (P_VIRGULA(newToken))
        {
          return;
        }
        else
        {
          printError(newToken, "Esperado ';'");
        }
      }
      return;
    }
    }
  }
  else
  {
    printError(currentToken, "Esperado um identificador");
  }
}

void T_FUNC(Token currentToken, FILE *arq)
{
  if (PR_VOID(currentToken) || PR_BOOL(currentToken) || PR_INT(currentToken) || PR_FLOAT(currentToken))
  {
    Token newToken = nextToken(arq);
    IDENTIFICADOR(newToken, Q_FUNC, arq);
  }
  else
  {
    printError(currentToken, "Esperado 'int', 'void', 'double' ou 'bool'");
  }
}

int main(int argc, char *argv[])
{
  if (argc <= 1)
  {
    printf("%s", "Número inválido de argumentos");
    exit(1);
  }
  FILE *arq;

  arq = fopen(argv[1], "r");
  if (arq == NULL)
  {
    printf("Não foi possível ler o arquivo\n");
    exit(1);
  }

  //Entrada TOKEN_TYPE@@TOKEN_TEXT@@LIN@@COL '\n'
  read_line = getline(&line, &line_len, arq); // Vai ler a primeira linha

  if (read_line != -1) // Arquivo vazio
  {
    Token currentToken = createTokenFromCurrentLine();
    T_FUNC(currentToken, arq); // Início da programa
  }

  if (!hasError)
    printf("OK\n");

  free(line);
  fclose(arq);
  return 0;
}