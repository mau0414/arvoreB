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

int main() {

    FILE* f = fopen("dados.bin", "r");
    if (f == NULL) printf("deu ruim");

    Produto aux;
    fseek(f, -sizeof(Produto), SEEK_END);
    fread(&aux, sizeof(Produto),1, f);
    printf("nome: ");
    for (int i = 0; i < MAXIMONOME; i++) {
        printf("%c", aux.nome[i]);
    }
    printf("\npreco: ");
    for (int i = 0; i < 10; i++) {
        printf("%c", aux.preco[i]);
    }
    fclose(f);
    return 0;
}