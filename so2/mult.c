#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <omp.h>

#define NLIN 1000
#define NCOL 1000

#define toSeconds(t) (t.tv_sec + (t.tv_usec/1000000.))

int main(int argc, char **argv){
    // utilizado para contabilidades
    struct timeval inic, fim;
    int i, j, k, soma, nthr, nt; 
    int nlin = NLIN;
    int ncol = NCOL;
    int **a, **b, **c; 
    a = (int **) malloc(nlin * sizeof(int*));
    b = (int **) malloc(nlin * sizeof(int*));
    c = (int **) malloc(nlin * sizeof(int*));
    for (i = 0; i < nlin; i++){
        a[i] = (int*) malloc(ncol * sizeof(int));
        b[i] = (int*) malloc(ncol * sizeof(int));
        c[i] = (int*) malloc(ncol * sizeof(int));
    }

    for(i=0;i<nlin;i++)
			for(j=0;j<ncol;j++) {
				a[i][j]=rand()%10;
				b[i][j]=rand()%10;
			}

    // omp_set_dynamic(0);
    for (nt = 1; nt <= 128; nt*=2){ 
        omp_set_num_threads(nt);
        gettimeofday(&inic, 0);
        #pragma omp parallel for private(soma,j,k) 
        for (i=0; i < nlin; i++){
            for(j=0; j < ncol; j++){
                soma = 0;
                for(k=0; k < ncol; k++){
                    soma = soma + a[i][k]*b[k][j];
                }
                c[i][j] = soma;
            }
                nthr = omp_get_num_threads();
        }
        gettimeofday(&fim, 0);
        printf("\nTempo para multiplicacao (%dx%d /%d threads - for das linhas):\n",nlin,ncol,nthr);
        printf("Elapsed time:%f sec\n",
            toSeconds(fim)-toSeconds(inic));
    }
    for (nt = 1; nt <= 128; nt*=2){ 
    omp_set_num_threads(nt);
    gettimeofday(&inic, 0);
    // o i é privado também, mas o omp já faz ele ser
    for (i=0; i < nlin; i++){
        #pragma omp parallel for private(soma,k)
        for(j=0; j < ncol; j++){
            soma = 0;
            for(k=0; k < ncol; k++){
                soma = soma + a[i][k]*b[k][j];
            }
            c[i][j] = soma;
            nthr = omp_get_num_threads();

        }
    }
    gettimeofday(&fim, 0);
    printf("\nTempo para multiplicacao (%dx%d /%d threads - for das colunas):\n",nlin,ncol,nthr);
	printf("Elapsed time:%f sec\n",
		toSeconds(fim)-toSeconds(inic));
    }

    for (nt = 1; nt <= 128; nt*=2){ 
    omp_set_num_threads(nt);
    gettimeofday(&inic, 0);
    // o i é privado também, mas o omp já faz ele ser
    for (i=0; i < nlin; i++){
        for(j=0; j < ncol; j++){
            soma = 0;
            #pragma omp parallel for reduction(+:soma)
            for(k=0; k < ncol; k++){
                soma = soma + a[i][k]*b[k][j];
                nthr = omp_get_num_threads();

            }
            c[i][j] = soma;
        }
    }
    gettimeofday(&fim, 0);
    printf("\nTempo para multiplicacao (%dx%d /%d threads - for mais interno):\n",nlin,ncol,nthr);
	printf("Elapsed time:%f sec\n",
		toSeconds(fim)-toSeconds(inic));
    }
    

    // for (i=0; i < nlin; i++){
    //     for (j=0; j < ncol; j++){
    //         printf("%d ", c[i][j]);
    //     }
    //     printf("\n");
    // }
}

//#pragma omp parallel no j, 
//#pragma omp parallel for reduction(+:soma)

