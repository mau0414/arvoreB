#include <stdio.h>
#include <stdlib.h>
#define MAXIMOCHAVES 3
#define NOMEARQUIVO "arvoreb.bin"


typedef struct {
    int folha;
    int contador_chaves;
    int chaves[MAXIMOCHAVES];
    int ponteirosDados[MAXIMOCHAVES+1];
    int filhos[MAXIMOCHAVES+1];
} Page;

/*
resumo
    shahsa
parametros:
    curRRN: ponteiro pra inteiro que representa pagina atual da arvore b em uso
    key: chave a ser inserida
    keyPromovida: ponteiro pra chave a ser promovida pra cima
    filhoDPromovida: ponteiro para o filho direito da chave promovida
variaveis locais
    smams
retorno:
    -1 para erro
    0 para nao promocao
    1 para promocao
*/
int insert(int* curRRN, int key, int* filhoDPromovida, int* keyPromovida) {
    int pos = 0;
    Page* newPageAux;
    Page curPage;
    int* keyPromovidaAux;
    Page* filhoDPromovidaAux;
    FILE *f;

    if (curRRN == NULL) {
        *keyPromovida = key;
        filhoDPromovida = NULL;
        return 1;
    }

    else {
        f = fopen(NOMEARQUIVO, "rb"); // abre arquivo
        fseek(f, (*curRRN)*sizeof(Page), SEEK_SET); // posiciona na pagina desejada
        fread(&curPage, sizeof(Page), 1, f); // le uma pagina
        fclose(f); // fecha arquivo

        for (int i = 0; i < MAXIMOCHAVES; i++) {
            if (key <= curPage.chaves[i]) {
                if (key == curPage.chaves[i]) return -1; // erro pois chave ja existe
                else pos = i;
            }
        }
    }

    int returnValue = insert(curPage.filhos[pos], key, filhoDPromovidaAux, keyPromovidaAux);

    if (returnValue == 0 || returnValue == -1)
        return returnValue;
    else if (curPage.contador_chaves < MAXIMOCHAVES) {
        // inserir keyPromovidaAux e filhoDPromovidaAux em curPage (?)
        // 1. procurar posicao onde inserir
        // 2. transportar elementos uma posicao a frente
        // 3. inserir na posicao
        int posInsercao;
        for (int i = 0; i < curPage.contador_chaves; i++) {
            if (*keyPromovidaAux <= curPage.chaves[i]) {
                if (*keyPromovidaAux == curPage.chaves[i]) return -1; // erro pois chave ja existe
                else posInsercao  = i;
            }
        }
        int aux;
        for (int i = curPage.contador_chaves-1; i > curPage.contador_chaves; i--) {
            curPage.chaves[i+1] = curPage.chaves[i];
        }

        curPage.chaves[posInsercao] = *keyPromovidaAux;

        f  =  fopen(NOMEARQUIVO, "ab");
        // escrever no arquivo        
        fclose(f);
        return 0;
    } else {
        split(keyPromovidaAux, filhoDPromovidaAux, curPage, keyPromovida, filhoDPromovida, newPageAux);
        f  =  fopen(NOMEARQUIVO, "ab");
        // escrever curPage no arquivo em currRRN (?)
        // inserir newpageaux no arquivo em promo_r_child
        fclose(f);
        return 1;
    }
    return -1;
}

int split(int* p_B_Key, int* p_B_RRN, Page page, int* keyPagePromovida, int* filhoDPromovida, Page* newPage) {
    return;
}

int main() {

    return 0;
}