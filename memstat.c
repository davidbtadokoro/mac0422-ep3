#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib.h>
#include <unistd.h>
#include <math.h>

/* Funcao auxiliar do Bubble Sort */
void swap(unsigned int * x, unsigned int * y)
{
  int temp = *x;
  *x = *y;
  *y = temp;
}


/* Algoritmo de ordenacao simples */
void bubbleSort(unsigned int * holes_lengths, int nr_holes)
{
  int i, j;

  for (i = 0; i < nr_holes - 1; i++)
    for (j = 0; j < nr_holes - i - 1; j++)
      if (holes_lengths[j] > holes_lengths[j+1])
        swap(&holes_lengths[j], &holes_lengths[j+1]);
}


/* Funcao que extrai mediana de lista com tamanhos dos buracos */
double getmedian(unsigned int * holes_lengths, int nr_holes)
{
  int med1, med2;
  double median;

  /* Decidindo qual valor de mediana pegamos */
  if (nr_holes % 2 == 0) {
    med1 = nr_holes/2 - 1;
    med2 = nr_holes/2;
    median = (holes_lengths[med1] + holes_lengths[med2])/2.0;
  }
  else
    median = (double) holes_lengths[nr_holes/2];

  return median;
}


/* Faz dump dos tamanhos dos buracos */
void dump_holes(unsigned int * holes_lengths, int nr_holes)
{
  static dump_nr = 0;
  int i;

  /* Atualiza numero do dump */
  dump_nr++;

  /* Limpa tela */
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  
  /* Imprime tamanho dos buracos */
  printf("\t\t\t\033[0;33mHOLES_LENGTHS DUMP (NUMBER %d)\033[0m\n\n", dump_nr);
  for (i = 0; i < nr_holes; i++)
    printf("\t\t\t\t%.4fMB\n" , holes_lengths[i]/1048576.0);
  printf("\n");
}


int main(int argc, char ** argv)
{
  /* Valores a serem impressos */
  int nr_holes;
  double length_avg;
  double length_stdev;
  double length_median;
  /* Struct que guarda infos sobre os buracos da memoria */
  static struct pm_mem_info pmi;
  /* Lista que guarda somente os tamanhos dos buracos */
  unsigned int * holes_lengths;
  /* Variaveis auxiliares */
  unsigned int length_sum;
  double dev_sum;
  int i, j;
  /* Booleano p/ dump de holes_length */
  int dump_mode = 0;
  /* Tempo entre medicoes em segundos */
  int sleep_seconds = 1;

  /* Tratamentos de erros de input do usuario */
  if (argc > 2) {
    printf("Erro: numero de argumentos incorreto\n");
    printf("Uso: %s [--dump-mode]\n", argv[0]);
    return 1;
  }

  if (argc == 2) {
    if (strcmp(argv[1], "--dump-mode") == 0) {
      dump_mode = 1;
      sleep_seconds = 5;
    }      
    else {
      printf("Erro: argumento '%s' invalido.\n", argv[1]);
      printf("Uso: %s [--dump-mode]\n", argv[0]);
      return 1;
    }
  }

  for (;;) {

    /* Copia lista de holes + free_slots em pmi */
    getsysinfo(MM, SI_MEM_ALLOC, (void *) &pmi);

    /* Reseta as variaveis antes de cada medicao */
    nr_holes = 0;
    length_sum = 0;
    dev_sum = 0;

    /* Conta o numero de holes e faz o somatorio de seus tamanhos */
    for (i = 0; i < _NR_HOLES; i++) {
      if(pmi.pmi_holes[i].h_base && pmi.pmi_holes[i].h_len) {
        nr_holes++;
        length_sum += (pmi.pmi_holes[i].h_len << CLICK_SHIFT);
      }
    }

    /* Calcula a media dos tamanhos */
    length_avg = length_sum/(double) nr_holes;

    /* Cria vetor que guarda somente os holes */
    holes_lengths = malloc(sizeof(unsigned int *) * nr_holes);

    /* Preenche vetor e faz o somatorio do desvio padrao */
    for (i = 0, j = 0; i < _NR_HOLES; i++) {
      if(pmi.pmi_holes[i].h_base && pmi.pmi_holes[i].h_len) {
        holes_lengths[j] = (pmi.pmi_holes[i].h_len << CLICK_SHIFT);
      	dev_sum += pow(((double) holes_lengths[j] - length_avg), 2.0);
        j++;
      }
    }
    
    /* Calcula o desvio padrao */
    length_stdev = sqrt(dev_sum/(double) nr_holes);

    /* Ordena lista com tamanhos dos buracos e extrai a mediana */
    bubbleSort(holes_lengths, nr_holes);
    length_median = getmedian(holes_lengths, nr_holes);
    
    /* dump de holes_lengths ordenado */
    if (dump_mode) { dump_holes(holes_lengths, nr_holes); }
  
    /* Imprime estatisticas */
    printf("HOLES: %d\tAVERAGE: %.4fMB\tSTDDEV: %.4fMB\tMEDIAN: %.4fMB\n",
            nr_holes,
	    length_avg/1048576.0,
	    length_stdev/1048576.0,
	    length_median/1048576.0);

    /* Espera sleep_seconds segundos ate nova medicao */
    sleep(sleep_seconds);
  }


  return 1; /* Nao deveria chegar aqui */
}
