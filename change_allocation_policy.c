#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char ** argv)
{
  int ret;

  /* Tratamento de erro no input do usuario */
  if (argc != 2) {
    printf("Erro: numero invalido de argumentos\n");
    printf("Uso: %s [first_fit | best_fit | worst_fit | random_fit]\n",argv[0]);
    return(1);
  }

  /* Definimos qual politica o usuario deseja */
  if (strcmp(argv[1], "first_fit") == 0)
    ret = setallocpol(FIRST_FIT);  
  else if (strcmp(argv[1], "best_fit") == 0)
    ret = setallocpol(BEST_FIT);
  else if (strcmp(argv[1], "worst_fit") == 0)
    ret = setallocpol(WORST_FIT);
  else if (strcmp(argv[1], "random_fit") == 0)
    ret = setallocpol(RANDOM_FIT);
  else {
    printf("Erro: argumento '%s' invalido\n", argv[1]);
    printf("Uso: %s [first_fit | best_fit | worst_fit | random_fit]\n",argv[0]);
    return(1);
  }
  
  /* Caso a chamada de sistema tenha sido bem sucedida */
  if (ret == 0) {
    printf("\033[0;35mPolitica '%s' definida com sucesso!\033[0m\n", argv[1]);
    return(0);
  }

  printf("Erro: setallocpol retornou com erro...\n");
  return(1);
}
