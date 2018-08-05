#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

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

  Token token;

  while((ch=fgetc(arq)) != EOF) {
  	
  }

  fclose(arq);
  return 0;
}
