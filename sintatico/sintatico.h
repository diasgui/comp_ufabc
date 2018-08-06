typedef struct TokenStruct
{
  char tokenText[200];     // x
  char tokenTypeText[200]; // IDENTIFICADOR
  int tokenLine;
  int tokenCol;
} Token;

ssize_t getline(char **lineptr, size_t *n, FILE *stream);
void printError(Token t, char *message);
int A_PARENTESES(Token currentToken);
int F_PARENTESES(Token currentToken);
int PR_VOID(Token currentToken);
int PR_BOOL(Token currentToken);
int PR_FLOAT(Token currentToken);
int PR_INT(Token currentToken);
int P_VIRGULA(Token currentToken);
int A_CHAVES(Token currentToken);
int F_CHAVES(Token currentToken);
int VIRGULA(Token currentToken);
int ATTR(Token currentToken);
int OP_BOOL(Token currentToken);
int OP_NUM(Token currentToken);
int OP_REL_NUM(Token currentToken);
int INCR_ID(Token currentToken);
int PR_WHILE(Token currentToken);
int PR_IF(Token currentToken);
int PR_ELSE(Token currentToken);
int PR_RETURN(Token currentToken);
int INT(Token currentToken);
int FLOAT(Token currentToken);
int T_BOOL(Token currentToken);
void COND(Token currentToken, int state, FILE *arq);
void INSTRUCAO(Token currentToken, FILE *arq);
void PARAM(Token currentToken, FILE *arq);
void IDENTIFICADOR(Token currentToken, int state, FILE *arq);
void T_FUNC(Token currentToken, FILE *arq);