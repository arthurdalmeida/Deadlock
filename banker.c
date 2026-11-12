#include <stdio.h>
#include <stdlib.h>

int qtdRecursos;
int *avaiable;
int qtdClientes;

void leituraDeClientes(){
    FILE *arquivo = fopen("customer.txt", "r");

    if (arquivo == NULL){
        fprintf(stderr, "Não foi possível abrir o arquivo\n");
        free(avaiable);
        exit(1);
    }

    char linha[256];
    int cont = 0;

    while (fgets(linha, 256, arquivo) != NULL){
        if (linha[0] != '\n' && linha[0] != '\0' && linha[0] != '\r'){
            cont++;
        }
    }

    if (cont == 0){
        fprintf(stderr, "Arquivo vazio\n");
        fclose(arquivo);
        free(avaiable);
        exit(1);
    }

    qtdClientes = cont;
    fclose(arquivo);
}

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

    leituraDeClientes();
    printf("clientes: %d\n", qtdClientes);

    for (int i=0; i<qtdRecursos; i++){
        printf(" %d", avaiable[i]);
    }
    printf("\n");
    free(avaiable);
    return 0;

}