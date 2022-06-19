#include <stdio.h>
#include <stdlib.h>
#include <lib.h>
#include <unistd.h>
#include <math.h>

#define SLEEP_SECONDS 1
/*#define MEMSTAT_DEBUG*/

/* Funcao auxiliar do Bubble Sort */
void swap(unsigned int * x, unsigned int * y)
{
  int temp = *x;
  *x = *y;
  *y = temp;
}

/* Funcao que extrai mediana de lista com tamanhos dos buracos */
double getmedian(unsigned int * holes_lengths, int nr_holes)
{
  int i, j;
  double median;

  /* Bubble Sort */
  for (i = 0; i < nr_holes - 1; i++)
    for (j = 0; j < nr_holes - i - 1; j++)
      if (holes_lengths[j] > holes_lengths[j+1])
        swap(&holes_lengths[j], &holes_lengths[j+1]);

  /* Decidindo qual valor de mediana pegamos */
  if (nr_holes % 2 == 0) {
    i = nr_holes/2 - 1;
    j = nr_holes/2;
    median = (holes_lengths[i] + holes_lengths[j])/2.0;
  }
  else
    median = (double) holes_lengths[nr_holes/2];

  return median;
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

    /* Calcula a mediana */
    length_median = getmedian(holes_lengths, nr_holes);


    #ifdef MEMSTAT_DEBUG
    printf("\nholes_lengths\n");
    for (i = 0; i < nr_holes; i++)
      printf("%uB\n" , holes_lengths[i]);
    printf("\n");
    #endif
   
    /* Imprime estatisticas */
    printf("NR_HOLES: %d\tAVERAGE: %.1fB\tSTD_DEV: %.1fB\tMEDIAN: %.0fB\n",
            nr_holes, length_avg, length_stdev, length_median);

    /* Espera SLEEP_SECONDS segundos ate nova medicao */
    sleep(SLEEP_SECONDS);
  }


  return 1; /* Nao deveria chegar aqui */
}
