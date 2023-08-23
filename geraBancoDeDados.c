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
    int contadorVirgula = 0;
    FILE* f;
    FILE* fDados;
    fDados = fopen(NOMEARQUIVODADOS, "wb");
    f = fopen("../amazon.csv", "rb");    
    inicializaBuffer();
    

    caractere = fgetc(f);
    // printf("%c", caractere);
    while (caractere != EOF) {
        // printf("%c", caractere);
        if (caractere == ',') {
            if (contadorVirgula == 0) { // formato nome: nome = "blablablabla"
                printf("entrou aqui primeiro\n");
                int i = 0;
                caractere = fgetc(f); // pega ' " '
                caractere = fgetc(f); // pega primeira letra
                while (caractere != '"') { // espera proximo ' " ' que indica fim do nome
                    aux.nome[i] = caractere;
                    char teste = caractere;
                    printf("(aux.nome[i] = %c,  caractere = %c, teste = %c) ", aux.nome[i], caractere, teste);
                    caractere = fgetc(f);
                    i = i + 1;
                }
                // for (int i = 0; i < MAXIMONOME; i++) {
                //     aux.nome[i] = buffer[i];
                // }
                // inicializaBuffer();
            }
            else if (contadorVirgula == 3) { // formato preco = "simbolonumericonumericonumerico"
                printf("entrou aqui2\n");
                int i = 0;
                caractere = fgetc(f); // leu ' " '
                // lendo simbolo do dinheiro
                caractere = fgetc(f); 
                caractere = fgetc(f); 
                caractere = fgetc(f);
                // 
                caractere = fgetc(f); // le o primeiro algarismo numerico
                while (caractere != '"') { // espera por '"' que indica fim
                    aux.preco[i] = caractere;
                    caractere = fgetc(f);
                    i = i + 1;
                }
                // for (int j = 0; j < i; j++) {
                //     aux.preco[j] = buffer[j];
                // }
                // inicializaBuffer();
            }
            contadorVirgula = contadorVirgula + 1;
        } else if (caractere == '\n') {
            printf("entrou aqui3\n");
            aux.id = contadorIds;
            contadorIds = contadorIds + 1;
            aux.quantidade = (rand() % 200) + 1;
            fwrite(&aux, sizeof(Produto), 1, fDados);
            contadorVirgula = 0;
            printf("salvou isso\n");
            printf("nome: ");
            for (int i = 0; i < MAXIMONOME; i++) {
                printf("%c", aux.nome[i]);
            }
            printf("\n");
            printf("preco: ");
            for (int i = 0; i < 10; i++) {
                printf("%c", aux.preco[i]);
            }
            printf("\n");
            printf("id : %d", aux.id);
            printf("quantidade : %d", aux.quantidade);
            aux = inicializaAux();
        }
        caractere = fgetc(f);
    }
    fclose(f);
    fclose(fDados);
    return 0;
}