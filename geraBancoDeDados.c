#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAXIMONOME 300
#define OPCOESPRODUTO 50
#define MAXIMAQUANTIDADE 200
#define NOMEARQUIVODADOS "dados.bin"

typedef struct 
{
    int id;
    int quantidade;
    char preco[10];
    char nome[MAXIMONOME];
} Produto;


char buffer[MAXIMONOME];

void inicializaBuffer() {
    for (int i = 0; i < MAXIMONOME; i++) {
        buffer[i] = '-1';
    }
}

Produto inicializaAux() {
    Produto aux;
    for (int i = 0; i < MAXIMONOME; i++) {
        aux.nome[i] = '@';
    } 
    for (int i = 0; i < 10; i++) {
        aux.preco[i] = '@';
    }

    return aux;
}

int main() {

    srand(time(NULL));
    char caractere;
    int contadorIds = 0;
    Produto aux = inicializaAux();
    FILE* f;
    FILE* fDados;
    fDados = fopen(NOMEARQUIVODADOS, "wb");
    f = fopen("../amazon_processado.csv", "rb");    
    inicializaBuffer();
    

    caractere = fgetc(f);
    while (caractere != EOF) {
        // printf("%c", caractere);
        if (caractere == ',') {
            int i = 0;
            caractere = fgetc(f); // le o primeiro algarismo numerico
            while (caractere != '\n') { // espera por '"' que indica fim
                aux.preco[i] = caractere;
                caractere = fgetc(f);
                i = i + 1;
            }
        } else if (caractere == '\n') {
            aux.id = contadorIds;
            contadorIds = contadorIds + 1;
            aux.quantidade = (rand() % 200) + 1;
            fwrite(&aux, sizeof(Produto), 1, fDados);
            printf("salvou:\n");
            printf("nome: ");
            for (int i = 0; i < MAXIMONOME; i++) {
                printf("%c", aux.nome[i]);
            }
            printf("\npreco: ");
            for (int i = 0; i < 10; i++) {
                printf("%c", aux.preco[i]);
            }
            aux = inicializaAux();
            caractere = fgetc(f);
        } else {
            int i = 0;
            while (caractere != ',') { // espera proximo ' " ' que indica fim do nome
                aux.nome[i] = caractere;
                caractere = fgetc(f);
                i = i + 1;
            }
        }
    }
    fclose(f);
    fclose(fDados);
    return 0;
}