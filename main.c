#include <stdio.h>
#include <stdlib.h>
#define MAXIMOCHAVES 3
#define NOMEARQUIVO "btree.bin"

typedef struct {
  int folha;
  int contador_chaves;
  int chaves[MAXIMOCHAVES];
  int ponteirosDados[MAXIMOCHAVES];
  int filhos[MAXIMOCHAVES + 1];
} Page;

typedef struct {
  int folha;
  int contador_chaves;
  int chaves[MAXIMOCHAVES + 1];
  int ponteirosDados[MAXIMOCHAVES + 1];
  int filhos[MAXIMOCHAVES + 2];
} AuxiliarPage;


Page init_page() {
  Page page;
  for (int i = 0; i < MAXIMOCHAVES; i++) {
    page.chaves[i] = -1;
    page.contador_chaves = 0;
    page.ponteirosDados[i] = -1;
    page.folha = 1;
  }

  for (int i = 0; i < MAXIMOCHAVES + 1; i++)
    page.filhos[i] = -1;
  // for (int i = 0; i < MAXIMOCHAVES; i++) {
  //   page->chaves[i] = -1;
  //   page->contador_chaves = 0;
  //   page->ponteirosDados[i] = -1;
  //   page->folha = 1;
  // }

  return page;

  // for (int i = 0; i < MAXIMOCHAVES + 1; i++)
  //   page->filhos[i] = -1;
}

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

int insert(int curRRN, int key, int *filhoDPromovida, int *keyPromovida) {
  // printf("insert entrou\n");
  int pos = 0;
  Page *newPageAux = NULL;
  Page curPage;
  //init_page(&curPage);
  int keyPromovidaAux;
  int filhoDPromovidaAux;
  FILE *f;

  if (curRRN == -1) {
    // printf("era pagina nula\n");
    *keyPromovida = key;
    *filhoDPromovida = -1;
    return 1;
  } else {
    // printf("nao era pagina nula\n");
    f = fopen(NOMEARQUIVO, "rb");              // abre arquivo
    fseek(f, curRRN * sizeof(Page), SEEK_SET); // posiciona na pagina desejada
    fread(&curPage, sizeof(Page), 1, f);       // le uma pagina
    fclose(f);                                 // fecha arquivo
    // printf("chegou aqui\n");
    for (int i = 0; i < MAXIMOCHAVES; i++) {
      if (key <= curPage.chaves[i]) {
        if (key == curPage.chaves[i])
          return -1; // erro pois chave ja existe
        else
          pos = i;
      }
    }
  }
  // printf("chegou aqui tambem\n");
  // printf("olha aqui na posicao %d o numero: %d\n", pos, curPage.filhos[pos]);

  int returnValue = insert(curPage.filhos[pos], key, &filhoDPromovidaAux, &keyPromovidaAux);

  printf("retorno %d\n", returnValue);
  // sem promocao ou erro
  if (returnValue == 0 || returnValue == -1) {
    printf("erro ou nao houve promocao\n");
    return returnValue;
  } else if (curPage.contador_chaves < MAXIMOCHAVES) {
    //printf("tinha espaço na pagina %d para insercao\n", curRRN);
    // inserir keyPromovidaAux e filhoDPromovidaAux em curPage (?)
    // 1. procurar posicao onde inserir
    // 2. transportar elementos uma posicao a frente
    // 3. inserir na posicao
    // 4. salvar no arquivo

    //printf("AQUIIII\n");
    //printf("aqui: %d\n", curPage.chaves[0]);
    // printf("aqui: %d\n", curPage.contador_chaves);
    
    // passo 1
    int posInsercao;
    for (posInsercao = 0; posInsercao < curPage.contador_chaves; posInsercao++) {
      if (keyPromovidaAux <= curPage.chaves[posInsercao]) {
        if (keyPromovidaAux == curPage.chaves[posInsercao])
          return -1; // erro pois chave ja existe
        else break;
      }
    }

    printf("Posicao insercao final %d\n", posInsercao);
    printf("numero chaves %d\n", curPage.contador_chaves);

    // passo 2
    int aux;
    for (int i = curPage.contador_chaves - 1; i >= posInsercao;
         i--) {
      curPage.chaves[i + 1] = curPage.chaves[i];
    }  

    // passo 3
    curPage.chaves[posInsercao] = keyPromovidaAux;
    curPage.ponteirosDados[posInsercao] = keyPromovidaAux;
    curPage.contador_chaves++;

    //printf("key promovida %d\n", keyPromovidaAux);

    // printf("pagina:\n");
    // for (int i = 0; i < MAXIMOCHAVES; i++) {
    //   printf("chave[%d] = %d\n", i, curPage.chaves[i]);
    // }
    
    f = fopen(NOMEARQUIVO, "wb");
    // passo 4
    fseek(f, curRRN * sizeof(Page), SEEK_SET);
    fwrite(&curPage, sizeof(Page), 1, f);
    fclose(f);
    return 0;
  } else {
    // split(keyPromovidaAux, filhoDPromovidaAux, &curPage, keyPromovida,
    // filhoDPromovida, newPageAux);
    f = fopen(NOMEARQUIVO, "ab");
    // escrever curPage no arquivo em currRRN (?)
    // inserir newpageaux no arquivo em promo_r_child
    fclose(f);
    return 1;
  }
  return -1;
}
/*
resumo
    shahsa
parametros:
    newKey: ponteiro pra inteiro que representa nova chave a ser inserida
    filhoDNewKey: ponteiro para inteiro que representa filho a direita da nova
chave a ser inserida page: pagina de disco atual keyPromovida: chave promovida
    filhoDPromovida: ponteiro para o filho direito da chave promovida
    newPage: nova pagina do disco
variaveis locais
    smams
retorno:
    -1 para erro
    0 para nao promocao
    1 para promocao
*/
int split(int newKey, int filhoDNewKey, Page *page, int *keyPromovida, int *filhoDPromovida, int *newPage) {  
  // copiar todas chaves e ponteiros de page para uma pagina aux com capacidade  MAXKEYS+1 de chaves e MAXKEYS + 2 de filhos
  AuxiliarPage aux; 
  for (int i = 0; i < MAXIMOCHAVES; i++) {
    aux.chaves[i] = page->chaves[i];
    aux.filhos[i] = page->ponteirosDados[i];
    aux.ponteirosDados[i] = page->ponteirosDados[i];

    if (i == MAXIMOCHAVES - 1) {
        aux.filhos[i+1] = page->filhos[i];
    }
  }

  /*
    inserir newKey e filhoDNewKey no lugar certo deles na pag aux
  */
  aux.contador_chaves = MAXIMOCHAVES;
  int posInsercao ;
  for (posInsercao=0; posInsercao < MAXIMOCHAVES; posInsercao++) {
    if (newKey > aux.chaves[posInsercao]) {
        break;
    }
  }

  for (int i = aux.contador_chaves - 1; i >= posInsercao; i--) {
    aux.chaves[i + 1] = aux.chaves[i];
  } 

  

  aux.contador_chaves++;
  aux.chaves[posInsercao] = newKey;
  aux.filhos[posInsercao+1] = filhoDNewKey;
  aux.ponteirosDados[posInsercao] = newKey;
  


 /*alocar e inializar nova pagina no arquivo Btree para conter NEWPAGE*/
  FILE* f = fopen(NOMEARQUIVO, "wb");
  
  fclose(f);
  
/*set keyPromovida para o valor medio que sera promovida apos split*/
  int posMedia = (int) MAXIMOCHAVES+1 / 2;
  *keyPromovida = aux.chaves[posMedia];
  
  /*set filhoDPromovida para RRN de newPage*/
  *filhoDPromovida = *newPage;
  
  /*copiar todas chaves e ponteiros de filhos antes de promo_key da pag aux para page*/
  
  /* copia todas chaves e ponteiros para filhos depois de promo_key da pag aux para newpage*/
  return 0;
}

void acessoSequencialDeDebug() {
  FILE *f = fopen(NOMEARQUIVO, "rb");
  int pageCounter = 0;
  Page aux;
  while (!feof(f)) {
    fread(&aux, sizeof(Page), 1, f);
    if (!feof(f)) {
      printf("pagina %d\n", pageCounter);
      for (int i = 0; i < MAXIMOCHAVES; i++) {
        printf("chave[%d] %d\n", i, aux.chaves[i]);
      }
    }
    pageCounter++;
  }
  fclose(f);
}

void init_btree() {
  FILE *f = fopen(NOMEARQUIVO, "wb");
  Page aux  = init_page();
  fwrite(&aux, sizeof(Page), 1, f);
  fclose(f);
}

int main() {

  init_btree();
  int i = 0;

  // testes
  int resultado = insert(0, 31, NULL, NULL);
  resultado = insert(0, 45, NULL, NULL);
  resultado = insert(0, 33, NULL, NULL);
  //resultado = insert(0, 46, NULL, NULL);


  if (resultado == -1) {
    printf("errou ou chave ja inserida\n");
  }

  // debug
  acessoSequencialDeDebug();
  return 0;
}