#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int qtdRecursos;
int *available;
int qtdClientes;
int **maximum;
int **allocation;
int **need;

void leituraDeClientes(){
    FILE *arquivo = fopen("customer.txt", "r");

    if (arquivo == NULL){
        fprintf(stderr, "Não foi possível abrir o arquivo\n");
        free(available);
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
        free(available);
        exit(1);
    }

    qtdClientes = cont;

    maximum = malloc(qtdClientes*sizeof(int*));

    for (int i = 0; i<qtdClientes; i++){
        maximum[i] = malloc(qtdRecursos*sizeof(int));
    }

    rewind(arquivo);

    int i = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL){
        linha[strcspn(linha, "\r\n")] = '\0';
        
        if (linha[0] == '\0'){
            continue;
        }

        int j = 0;
        char *token = strtok(linha, ",");

        while (token != NULL){
            if (j >= qtdRecursos){
                fprintf(stderr, "cloente tem mais recursos que o esperado\n");
                fclose(arquivo);
                free(available);
                exit(1);
            }

            char *fim;
            long valor = strtol(token, &fim, 10);

            if (fim == token || *fim != '\0'){
                fprintf(stderr, "cliente tem um valor inválido\n");
                fclose(arquivo);
                free(available);
                exit(1);
            }

            if (valor < 0){
                fprintf(stderr, "cliente tem um valor neagtivo\n");
                fclose(arquivo);
                free(available);
                exit(1);
            }

            maximum[i][j] = (int)valor;
            j++;
            token = strtok(NULL, ",");
        } 

        if (j != qtdRecursos){
            fprintf(stderr, "cliente tem menos recursos que o esperado\n");
            fclose(arquivo);
            free(available);
            exit(1);
        }
        i++;
    }

    fclose(arquivo);
}

void inicializarEstado(){
    allocation = malloc(qtdClientes*sizeof(int*));
    need = malloc(qtdClientes*sizeof(int*));

    for (int i=0; i<qtdClientes; i++){
        allocation[i] = calloc(qtdRecursos, sizeof(int));
        need[i] = malloc(qtdRecursos*sizeof(int));

        for (int j=0; j<qtdRecursos; j++){
            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }
}

void imprimirEstado(FILE *saida){
    fprintf(saida, "MAXIMUM | ALLOCATION | NEED\n");

    for (int i=0; i<qtdClientes; i++){
        for (int j=0; j<qtdRecursos; j++){
            if (j>0){
                fprintf(saida, " ");    
            }
            fprintf(saida, "%d", maximum[i][j]);
        }

        fprintf(saida, " | ");

        for (int j=0; j<qtdRecursos; j++){
            if (j>0){
                fprintf(saida, " ");
            }
            fprintf(saida, "%d", allocation[i][j]);
        }

        fprintf(saida, " | ");

        for (int j=0; j<qtdRecursos; j++){
            if (j>0){
                fprintf(saida, " ");
            }
            fprintf(saida, "%d", need[i][j]);
        }

        fprintf(saida, "\n");
    }

    fprintf(saida, "AVAILABLE");

    for (int i=0; i<qtdRecursos; i++){
        fprintf(saida, " %d", available[i]);
    }

    fprintf(saida, "\n");
}

int validarLinha(char *linha, int numeroDaLinha, int *cliente, int *pedido){
    linha[strcspn(linha, "\r\n")] = '\0';

    if (linha[0] == '\0'){
        return 0;
    }

    char *token = strtok(linha, " ");
    char *fim;
    long valor;
    int tipoToken;

    if (token == NULL){
        return 0;
    }

    if (strcmp(token, "*") == 0){
        if (strtok(NULL, " ") != NULL){
            fprintf(stderr, "commands.txt, linha %d: * nao aceita parametros\n", numeroDaLinha);
            exit(1);
        }
        return 3;
    }

    if (strcmp(token, "RQ") == 0){
        tipoToken = 1;
    }
    else if (strcmp(token, "RL") == 0){
        tipoToken = 2;
    }
    else{
        fprintf(stderr, "Comando desconhecido na linha %d: %s\n", numeroDaLinha, token);
        exit(1);
    }

    token = strtok(NULL, " ");
    if (token == NULL){
        fprintf(stderr, "Falta o numero de clientes da linha %d\n", numeroDaLinha);
        exit(1);
    }

    valor = strtol(token, &fim, 10);
    if (valor >= qtdClientes || valor < 0){
        fprintf(stderr, "Cliente %s da linha %d nao existe\n", token, numeroDaLinha);
        exit(1);
    }

    valor = strtol(token, &fim, 10);
    if (*fim != '\0' || fim == token){
        fprintf(stderr, "O cliente %s da linha %d é inválido\n", token, numeroDaLinha);
        exit(1);
    }

    *cliente = (int)valor;

    for (int i=0; i<qtdRecursos; i++){
        token = strtok(NULL, " ");
        if (token == NULL){
            fprintf(stderr, "A linha %d tem menos recursos que o esperado\n", numeroDaLinha);
            exit(1);
        }

        valor = strtol(token, &fim, 10);

        if (valor < 0){
            fprintf(stderr, "Valor negativo na linha %d\n", numeroDaLinha);
            exit(1);
        }

        if (*fim != '\0' || fim == token){
            fprintf(stderr, "Valor invalido na linha %d\n", numeroDaLinha);
            exit(1);
        }

        pedido[i] = (int)valor;
    }

    if (strtok(NULL, " ") != NULL){
        fprintf(stderr, "Mais recursos que o esperado na linha %d\n", numeroDaLinha);
        exit(1);
    }
    return tipoToken;
}

void liberacaoDeRecursos(FILE* saida, int cliente, int *valores){
    int liberador = 1;

    for (int i= 0; i<qtdRecursos; i++){
        if (valores[i] > allocation[cliente][i]){
            liberador = 0;
        }
    }
    
    if (liberador == 1){
        for (int i=0; i<qtdRecursos; i++){
            available[i] += valores[i];
            allocation[cliente][i] -= valores[i];
            need[cliente][i] += valores[i];
        }

        fprintf(saida, "Release from customer %d the resources", cliente);
        for (int i=0; i<qtdRecursos; i++){
            fprintf(saida, " %d", valores[i]);
        }
        fprintf(saida, "\n");
    }

    else{
        fprintf(saida, "The customer %d cannot release more resources than it has allocated\n", cliente);
    }
    
}

void processarComandos(){
    FILE* arq = fopen("commands.txt", "r");

    if (arq == NULL){
        fprintf(stderr, "Não foi possível abrir o arquivo commands.txt\n");
        exit(1);
    }

    int cliente;
    char linha[256];
    int numDaLinha = 0;
    int *valores = malloc(qtdRecursos*sizeof(int));

    while (fgets(linha, sizeof(linha), arq) != NULL){
        numDaLinha++;
        validarLinha(linha, numDaLinha, &cliente, valores);
    }

    rewind(arq);
    numDaLinha = 0;

    while (fgets(linha, sizeof(linha), arq) != NULL){
        numDaLinha++;
        int tipoComando = validarLinha(linha, numDaLinha, &cliente, valores);

        if (tipoComando == 1){
            printf("RQ %d", cliente);
            for (int i=0; i<qtdRecursos; i++){
                printf(" %d", valores[i]);
            }
            printf("\n");
        }

        else if(tipoComando == 2){
            liberacaoDeRecursos(stdout, cliente, valores);
        }

        else if(tipoComando == 3){
            imprimirEstado(stdout);
        }
    }

    fclose(arq);
    free(valores);
    
}


int main(int argc, char *argv[]){
    qtdRecursos = argc-1;

    if (qtdRecursos <= 0){
        fprintf(stderr, "deve ser: ./banker r1 r2 r3\n");
        return 1;
    }

    available = malloc(qtdRecursos*sizeof(int));

    if (available == NULL){
        fprintf(stderr, "Erro de memória\n");
        return 1;
    }

    for (int i=0; i<qtdRecursos; i++){
        char *fim;
        long valor = strtol(argv[i+1], &fim, 10);

        if (argv[i+1][0] == '\0' || *fim != '\0'){
            fprintf(stderr, "Argumento invalido: %s\n", argv[i+1]);
            free(available);
            return 1;
        }

        if (valor < 0){
            fprintf(stderr, "Valor negativo: %s\n", argv[i+1]);
            free(available);
            return 1;
        }

        available[i] = (int)valor;
    }

    leituraDeClientes();
    inicializarEstado();
    processarComandos();

    free(available);
    return 0;
}