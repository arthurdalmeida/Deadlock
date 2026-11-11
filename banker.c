#include <stdio.h>
#include <stdlib.h>

int qtdRecursos;
int *avaiable;

int main(int argc, char *argv[]){
    qtdRecursos = argc-1;

    if (qtdRecursos <= 0){
        fprintf(stderr, "deve ser: ./banker r1 r2 r3\n");
        return 1;
    }

    avaiable = malloc(qtdRecursos*sizeof(int));

    if (avaiable == NULL){
        fprintf(stderr, "Erro de memória\n");
        return 1;
    }

    for (int i=0; i<qtdRecursos; i++){
        char *fim;
        long valor = strtol(argv[i+1], &fim, 10);

        if (argv[i+1][0] == '\0' || *fim != '\0'){
            fprintf(stderr, "Argumento invalido: %s\n", argv[i+1]);
            free(avaiable);
            return 1;
        }

        if (valor < 0){
            fprintf(stderr, "Valor negativo: %s\n", argv[i+1]);
            free(avaiable);
            return 1;
        }

        avaiable[i] = (int)valor;
    }

    for (int i=0; i<qtdRecursos; i++){
        printf(" %d", avaiable[i]);
    }
    printf("\n");
    free(avaiable);
    return 0;

}