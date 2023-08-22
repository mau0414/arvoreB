#include <stdio.h>
#include <stdlib.h>
#define MAXIMOCHAVES 2
#define NOMEARQUIVO "btree.bin"
#define NOMEHEADER "header.txt"
#define PROMOCAO 1
#define NAOPROMOCAO 0
#define ERRO -1

typedef struct {
  int RRN;
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
    page.RRN = 0;
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
void split(int* newKey, int* filhoDNewKey, Page *page, int *keyPromovida, int *filhoDPromovida, Page* newPage) {  
  // copiar todas chaves e ponteiros de page para uma pagina aux com capacidade  MAXKEYS+1 de chaves e MAXKEYS + 2 de filhos
  AuxiliarPage aux; 
  for (int i = 0; i < MAXIMOCHAVES; i++) {
    aux.chaves[i] = page->chaves[i];
    aux.filhos[i] = page->filhos[i];
    aux.ponteirosDados[i] = page->ponteirosDados[i];

    if (i == MAXIMOCHAVES-1) {
        aux.filhos[i+1] = page->filhos[i+1];
    }
  }

  /*
    inserir newKey e filhoDNewKey no lugar certo deles na pag aux
  */
 // achar lugar certo
  aux.contador_chaves = MAXIMOCHAVES+1;
  int posInsercao ;
  for (posInsercao=0; posInsercao < MAXIMOCHAVES+1; posInsercao++) {
    if (*newKey < aux.chaves[posInsercao]) {
        break;
    }
  }

  // transposicao elementos
  for (int i = MAXIMOCHAVES - 1; i >= posInsercao; i--) {
    aux.chaves[i + 1] = aux.chaves[i];
    aux.filhos[i + 2] = aux.filhos[i + 1];
  } 

  aux.contador_chaves++;
  aux.chaves[posInsercao] = *newKey;
  aux.filhos[posInsercao+1] = *filhoDNewKey;
  aux.ponteirosDados[posInsercao] = *newKey;
  
  /*
    inicializa newPage preparado seu terreno pro arquivo - aloca seu novo RRN e atualiza no header
  */
  FILE* f = fopen(NOMEHEADER, "r+");
  int numPages;
  fseek(f, sizeof(int), SEEK_SET);
  fread(&numPages, sizeof(int), 1, f);
  newPage->RRN = numPages;
  for (int i = 0; i < MAXIMOCHAVES; i++) {
    newPage->chaves[i] = -1;
    newPage->ponteirosDados[i] = -1;
    newPage->filhos[i] = -1;
    if (i == MAXIMOCHAVES - 1) newPage->filhos[i+1] = -1;
  }
  numPages = numPages + 1;
  rewind(f);
  fseek(f, sizeof(int), SEEK_SET);
  fwrite(&numPages, sizeof(int), 1, f);
  fclose(f);


/*set keyPromovida para o valor medio que sera promovida apos split*/
  int posMedia = (MAXIMOCHAVES+1) / 2;
  (*keyPromovida) = aux.chaves[posMedia];
  
  /*set filhoDPromovida para RRN de newPage*/
  *filhoDPromovida = newPage->RRN;
  
  /*copiar todas chaves e ponteiros de filhos antes de promo_key da pag aux para page*/
  for (int i = 0; i < MAXIMOCHAVES; i++) {
    if (i >= posMedia) {
        page->chaves[i] = -1;
        page->ponteirosDados[i] = -1;
        page->filhos[i+1] = -1;    
    } else {
        page->chaves[i] = aux.chaves[i];
        page->ponteirosDados[i] = aux.ponteirosDados[i];
        page->filhos[i] = aux.filhos[i];
        page->contador_chaves = posMedia;
        if (i == posMedia - 1) page->filhos[i+1] = aux.filhos[i+1];
    }
  }

  /* copia todas chaves e ponteiros para filhos depois de promo_key da pag aux para newpage*/
  int newPageIndex = 0;
  for (int i = posMedia+1; i < MAXIMOCHAVES+1; i++) {
    newPage->chaves[newPageIndex] = aux.chaves[i];
    newPage->ponteirosDados[newPageIndex] = aux.ponteirosDados[i];
    newPage->filhos[newPageIndex] = aux.filhos[i];
    if (i == MAXIMOCHAVES) newPage->filhos[newPageIndex+1] = aux.filhos[i+1];
    newPageIndex++;
  }
  newPage->contador_chaves = MAXIMOCHAVES - posMedia;
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
  int pos;
  Page curPage;
  //init_page(&curPage);
  int keyPromovidaAux;
  int filhoDPromovidaAux;
  FILE *f;

  if (curRRN == -1) {
    *keyPromovida = key;
    *filhoDPromovida = -1;
    return 1;
  } else {
    f = fopen(NOMEARQUIVO, "rb");              // abre arquivo
    fseek(f, curRRN * sizeof(Page), SEEK_SET); // posiciona na pagina desejada
    fread(&curPage, sizeof(Page), 1, f);       // le uma pagina
    fclose(f);                                 // fecha arquivo
    for (pos = 0; pos < MAXIMOCHAVES; pos++) {
      if (key <= curPage.chaves[pos] || curPage.chaves[pos] == -1) {
        if (key == curPage.chaves[pos])
          return -1; // erro pois chave ja existe
        else {
          break;
        }
      }
    }
  }
  int returnValue = insert(curPage.filhos[pos], key, &filhoDPromovidaAux, &keyPromovidaAux);

  // sem promocao ou erro
  if (returnValue == 0 || returnValue == -1) {
    printf("erro ou nao houve promocao\n");
    return returnValue;
  } else if (curPage.contador_chaves < MAXIMOCHAVES) {
    // inserir keyPromovidaAux e filhoDPromovidaAux em curPage (?)
    // 1. procurar posicao onde inserir
    // 2. transportar elementos uma posicao a frente
    // 3. inserir na posicao
    // 4. salvar no arquivo
    
    // passo 1
    int posInsercao;
    for (posInsercao = 0; posInsercao < curPage.contador_chaves; posInsercao++) {
      if (keyPromovidaAux <= curPage.chaves[posInsercao]) {
        if (keyPromovidaAux == curPage.chaves[posInsercao])
          return -1; // erro pois chave ja existe
        else break;
      }
    }

    // passo 2
    for (int i = curPage.contador_chaves - 1; i >= posInsercao;
         i--) {
      curPage.chaves[i + 1] = curPage.chaves[i];
      curPage.filhos[i + 2] = curPage.filhos[i + 1];
      if (i == posInsercao) curPage.filhos[i+1] = curPage.filhos[i];

    }  

    // passo 3
    curPage.chaves[posInsercao] = keyPromovidaAux;
    curPage.filhos[posInsercao+1] = filhoDPromovidaAux;
    curPage.ponteirosDados[posInsercao] = keyPromovidaAux;
    curPage.contador_chaves = curPage.contador_chaves + 1;
    int numPages;
    f = fopen(NOMEHEADER, "r");
    fseek(f, sizeof(int), SEEK_SET);
    fread(&numPages, sizeof(int), 1, f);
    fclose(f);

    // passo 4
    f = fopen(NOMEARQUIVO, "r+b");
    fseek(f, curRRN*sizeof(Page), SEEK_SET);
    fwrite(&curPage, sizeof(Page), 1, f);
    fclose(f);

    f = fopen(NOMEARQUIVO, "rb");
    Page teste;
    fseek(f, 0 * sizeof(Page), SEEK_SET);
    fread(&teste, sizeof(Page), 1, f);
    return 0;
  } else {
    Page newPage;
    split(&keyPromovidaAux, &filhoDPromovidaAux, &curPage, keyPromovida, filhoDPromovida, &newPage);
    f = fopen(NOMEARQUIVO, "r+b");
    // escrever curPage no arquivo em currRRN (?)
    fseek(f, curPage.RRN*sizeof(Page), SEEK_SET);
    fwrite(&curPage, sizeof(Page), 1, f);
    fseek(f, newPage.RRN*sizeof(Page), SEEK_SET);
    fwrite(&newPage, sizeof(Page), 1, f);
    fclose(f);

    return 1;
  }
  return -1;
}

// auxiliar para testes
void lePagina(Page page) {
    printf("pagina %d:\n", page.RRN);
    for (int i = 0; i < page.contador_chaves; i++) {
        printf("chave[%d] = %d\n", i, page.chaves[i]);
    }
    printf("********************************************\n");
}

// auxiliar para testes
void acessoSequencialDeDebug() {
  FILE *f = fopen(NOMEARQUIVO, "r");
  FILE* fheader = fopen(NOMEHEADER, "r");
  int rootRRN;
  int numPages;
  //fseek(fheader, sizeof(int), SEEK_SET);
  fread(&rootRRN, sizeof(int), 1, fheader);
  fread(&numPages, sizeof(int), 1, fheader);
  Page aux;
  for (int i = 0; i < numPages; i++) {
    fread(&aux, sizeof(Page), 1, f);
    printf("pagina %d\n", aux.RRN);
    for (int j = 0; j < MAXIMOCHAVES; j++) {
        printf("chave[%d] = %d\n", j, aux.chaves[j]);
    }
    printf("filhos:\n");
    for (int j = 0; j < MAXIMOCHAVES; j++) {
        printf("filho[%d] = %d\n", j, aux.filhos[j]);
        if (j == MAXIMOCHAVES - 1) printf("filho[%d] = %d\n", j+1, aux.filhos[j+1]);
    }
  }
  fclose(f);
  fclose(fheader);
  printf("\n\n");
}

void create_files() {
    FILE* f = fopen(NOMEARQUIVO, "wb");
    Page root;
    root.contador_chaves = 0;
    root.RRN = 0;
    for (int i = 0; i < MAXIMOCHAVES; i++) {
        root.chaves[i] = -1;
        root.filhos[i] = -1;
        root.ponteirosDados[i] = -1;

        if (i == MAXIMOCHAVES - 1) root.filhos[i+1] = -1;
    }

    fwrite(&root, sizeof(Page), 1, f);
    fclose(f);

    f = fopen(NOMEHEADER, "w");
    int rootRRN = 0;
    int numPages = 1;
    fwrite(&rootRRN, sizeof(int), 1, f);
    fwrite(&numPages, sizeof(int), 1, f);
    fclose(f);
}

// retorna RRN da pagina raiz
int init_btree() {
    // descomentar aqui caso queira que arquivo fique sendo criado toda vez que rodar (para fazer testes)
//   create_files();
//   return 0;
  FILE *f = fopen(NOMEARQUIVO, "r");
  if (f == NULL) {
    printf("arquivo nao existe ainda\n");
    create_files();
    return 0;
  } else {
    f = fopen(NOMEHEADER, "r");
    int rootRRN;
    fread(&rootRRN, sizeof(int), 1, f);
    fclose(f);
    return rootRRN;
  }  
}

int atualizaHeaderNovoRoot() {
    int rootRRN;
    int numPages;
    FILE* f = fopen(NOMEHEADER, "r+");
    fseek(f, sizeof(int), SEEK_SET);
    fread(&numPages, sizeof(int), 1, f);
    fclose(f);
    rootRRN = numPages;
    numPages = numPages + 1;
    f = fopen(NOMEHEADER, "w");
    fwrite(&rootRRN, sizeof(int), 1, f);
    fwrite(&numPages, sizeof(int), 1, f);
    fclose(f);
    return rootRRN; 
}

void inserir(int* rootRRN) {
    FILE* f;
    int insercao;
    int keyPromovida;
    int filhoDPromovida;
    printf("inserir: ");
    scanf("%d", &insercao);
    printf("\n");
    int resultado = insert(*rootRRN, insercao, &filhoDPromovida, &keyPromovida);
    if (resultado == PROMOCAO) {
        // criar nova pagina raiz com chave = ultimo_argumento_insert, left_child = rootRRN e right_child = terceiro_argumento
        int novoRootRRN = atualizaHeaderNovoRoot();
        Page novoRoot;
        novoRoot.chaves[0] = keyPromovida;
        novoRoot.filhos[0] = *rootRRN;
        novoRoot.filhos[1] = filhoDPromovida;
        for (int i = 1; i < MAXIMOCHAVES; i++) {
            novoRoot.chaves[i] = -1;
            novoRoot.filhos[i+1] = -1;
            //if (i == MAXIMOCHAVES-1) novoRoot.filhos[i+1] = -1;
        }
        novoRoot.contador_chaves = 1;
        novoRoot.RRN = novoRootRRN;
        *rootRRN = novoRootRRN;

        f = fopen(NOMEARQUIVO, "r+b");
        fseek(f, novoRoot.RRN*sizeof(Page), SEEK_SET);
        fwrite(&novoRoot, sizeof(Page), 1, f);
        fclose(f);
    } else if (resultado == ERRO) {
        printf("errou ou chave ja inserida\n");
    }
    acessoSequencialDeDebug();
}

// retorna 0 para nao encontrado e 1 para encontrado
int busca(int curRRN, int key, int *foundRRN, int *foundPos, int *parentRRN) {
  if (curRRN == -1) {
    *parentRRN = -1;
    return 0;
  }
  else {
    Page curPage;
    int pos;
    FILE* f = fopen(NOMEARQUIVO, "rb");
    fseek(f, curRRN * sizeof(Page), SEEK_SET);
    fread(&curPage, sizeof(Page), 1, f);
    fclose(f);

    for (pos = 0; pos < MAXIMOCHAVES; pos++) {
      if (key <= curPage.chaves[pos] || curPage.chaves[pos] == -1) {
        break;
      }
    }
    if (key == curPage.chaves[pos]){
      *foundRRN = curRRN;
      *foundPos = pos;
      return 1;
    }
    else {
      printf("busca na pagina %d\n", curPage.filhos[pos]);
      *parentRRN = curRRN;
      return busca(curPage.filhos[pos], key, foundRRN, foundPos, parentRRN);
    }
  }
}

void buscaPorId() {
    int foundPos;
    int foundRRN;
    int parentRRN;
    int key;
    int rootRRN;
    int resultado;

    // Leitura da chave a ser procurada
    printf("Id procurado: \n");
    scanf("%d", &key);

    FILE* fheader = fopen(NOMEHEADER, "r");
    fread(&rootRRN, sizeof(int), 1, fheader);
    fclose(fheader);
    
    resultado = busca(rootRRN, key, &foundRRN, &foundPos, &parentRRN);

    if (resultado == 1) {
      printf("achou na pagina %d na posicao %d\n", foundRRN, foundPos);
    } else {
      printf("Chave nao encontrada na B-tree.\n");
    }
}

void remove_key(Page* curPage, int chave, int *parentRRN) {

  FILE* f;
  int i = 0;
  while (i < curPage->contador_chaves && curPage->chaves[i] < chave) {
      i++;
  }

  if (curPage->chaves[i] == chave) { // chave encontrada na pagina
    
    // a chave está em um nó folha
    if (curPage->filhos[i] == -1) {
      for (int j = i; j < curPage->contador_chaves - 1; j++) {
          curPage->chaves[j] = curPage->chaves[j + 1];
          curPage->ponteirosDados[j] = curPage->ponteirosDados[j + 1];
      }
      curPage->chaves[curPage->contador_chaves - 1] = -1;
      curPage->ponteirosDados[curPage->contador_chaves - 1] = -1;
      curPage->contador_chaves--;
      f = fopen(NOMEARQUIVO, "r+b");
      fseek(f, curPage->RRN*sizeof(Page), SEEK_SET);
      fwrite(curPage, sizeof(Page), 1, f);
      fclose(f);
    }

    // a chave não está em um nó folha
    else if (curPage->filhos[i] != -1) {
      // Encontra o sucessor imediato no nó folha
      Page page_aux;
      //Page page_aux2;
      f = fopen(NOMEARQUIVO, "rb");
      fseek(f, curPage->filhos[i+1] * sizeof(Page), SEEK_SET);
      fread(&page_aux, sizeof(Page), 1, f);
      //fseek(f, curPage->filhos[i] * sizeof(Page), SEEK_SET);
      //fread(&page_aux2, sizeof(Page), 1, f);
      fclose(f);
      Page* sucessor = NULL;
      Page* sucessorDir = &page_aux; // subarvore direita
      //Page* sucessorEsq = &page_aux2; // subarvore esquerda
      while (sucessorDir->filhos[0] != -1 /*&& sucessorEsq->filhos[sucessorEsq->contador_chaves+1] != -1*/) {
        f = fopen(NOMEARQUIVO, "rb");
        fseek(f, sucessorDir->filhos[0] * sizeof(Page), SEEK_SET);
        fread(&page_aux, sizeof(Page), 1, f);
        //fseek(f, sucessorEsq->filhos[sucessorEsq->contador_chaves+1] * sizeof(Page), SEEK_SET);
        //fread(&page_aux2, sizeof(Page), 1, f);
        fclose(f);
        sucessorDir = &page_aux;
        //sucessorEsq = &page_aux2;
      }
      // Remove a chave do nó atual (substituindo pela chave do sucessor)
      int chave_removida = curPage->chaves[i];
      /*if (sucessorEsq->contador_chaves > sucessorDir->contador_chaves) {
        printf("chegou esq\n");
        sucessor = &page_aux2;
        curPage->chaves[i] = sucessor->chaves[sucessor->contador_chaves+1];
        sucessor->chaves[sucessor->contador_chaves+1] = chave_removida;
      }*/
      //else {
        //printf("chegou dir\n");
        sucessor = &page_aux;
        curPage->chaves[i] = sucessor->chaves[0];
        sucessor->chaves[0] = chave_removida;
      //}
      f = fopen(NOMEARQUIVO, "r+b");
      fseek(f, curPage->RRN*sizeof(Page), SEEK_SET);
      fwrite(curPage, sizeof(Page), 1, f);
      fseek(f, sucessor->RRN*sizeof(Page), SEEK_SET);
      fwrite(sucessor, sizeof(Page), 1, f);
      fclose(f);

      *parentRRN = curPage->RRN;
      // Atualiza o nó filho onde foi encontrada a chave sucessora
      remove_key(sucessor, chave_removida, parentRRN);
    }

    // Verifica se o nó ainda possui o número mínimo de chaves
    if (curPage->contador_chaves >= (MAXIMOCHAVES + 1) / 2) {
      return;
    }

    // Verifica os nós irmãos adjacentes para aplicar redistribuição ou concatenação
    int posicao_pai = 0; // posição da página que está sendo removida em relação aos filhos do nó pai
    Page auxpai;
    Page* pai = NULL;
    while (pai == NULL) {
      f = fopen(NOMEARQUIVO, "rb");
      fseek(f, *parentRRN * sizeof(Page), SEEK_SET);
      fread(&auxpai, sizeof(Page), 1, f);
      fclose(f);
      pai = &auxpai;
      for (int j = 0; j <= pai->contador_chaves; j++) {
          if (pai->filhos[j] == curPage->RRN) {
              posicao_pai = j;
              break;
          }
      }
    }
    Page auxesq;
    Page auxdir;
    Page* irmao_esq = NULL;
    Page* irmao_dir = NULL;
    if (posicao_pai > 0) {
        f = fopen(NOMEARQUIVO, "rb");
        fseek(f, pai->filhos[posicao_pai - 1] * sizeof(Page), SEEK_SET);
        fread(&auxesq, sizeof(Page), 1, f);
        fclose(f);
        irmao_esq = &auxesq;
    }
    if (posicao_pai < pai->contador_chaves) {
        f = fopen(NOMEARQUIVO, "rb");
        fseek(f, pai->filhos[posicao_pai + 1] * sizeof(Page), SEEK_SET);
        fread(&auxdir, sizeof(Page), 1, f);
        fclose(f);
        irmao_dir = &auxdir;
    }
    // Redistribuição com o irmão da direita
    if (irmao_dir != NULL && irmao_dir->contador_chaves > (MAXIMOCHAVES + 1) / 2) {
        curPage->chaves[curPage->contador_chaves] = pai->chaves[posicao_pai];
        curPage->ponteirosDados[curPage->contador_chaves] = pai->ponteirosDados[posicao_pai];
        curPage->filhos[curPage->contador_chaves + 1] = irmao_dir->filhos[0];
        pai->chaves[posicao_pai] = irmao_dir->chaves[0];
        pai->ponteirosDados[posicao_pai] = irmao_dir->ponteirosDados[0];
        for (int j = 0; j < irmao_dir->contador_chaves - 1; j++) {
            irmao_dir->chaves[j] = irmao_dir->chaves[j + 1];
            irmao_dir->ponteirosDados[j] = irmao_dir->ponteirosDados[j + 1];
            irmao_dir->filhos[j] = irmao_dir->filhos[j + 1];
        }
        irmao_dir->chaves[irmao_dir->contador_chaves - 1] = -1;
        irmao_dir->ponteirosDados[irmao_dir->contador_chaves - 1] = -1;
        irmao_dir->filhos[irmao_dir->contador_chaves - 1] = irmao_dir->filhos[irmao_dir->contador_chaves];
        irmao_dir->contador_chaves--;
        curPage->contador_chaves++;
        f = fopen(NOMEARQUIVO, "r+b");
        fseek(f, curPage->RRN*sizeof(Page), SEEK_SET);
        fwrite(curPage, sizeof(Page), 1, f);
        fseek(f, pai->RRN*sizeof(Page), SEEK_SET);
        fwrite(pai, sizeof(Page), 1, f);
        fseek(f, irmao_dir->RRN*sizeof(Page), SEEK_SET);
        fwrite(irmao_dir, sizeof(Page), 1, f);
        fclose(f);
        return;
    }

    // Redistribuição com o irmão da esquerda
    else if (irmao_esq != NULL && irmao_esq->contador_chaves > (MAXIMOCHAVES + 1) / 2) {
        for (int j = curPage->contador_chaves; j > 0; j--) {
            curPage->chaves[j] = curPage->chaves[j - 1];
            curPage->ponteirosDados[j] = curPage->ponteirosDados[j - 1];
            curPage->filhos[j + 1] = curPage->filhos[j];
        }
        curPage->filhos[1] = curPage->filhos[0];
        curPage->chaves[0] = pai->chaves[posicao_pai - 1];
        curPage->ponteirosDados[0] = pai->ponteirosDados[posicao_pai - 1];
        curPage->filhos[0] = irmao_esq->filhos[irmao_esq->contador_chaves];
        pai->chaves[posicao_pai - 1] = irmao_esq->chaves[irmao_esq->contador_chaves - 1];
        pai->ponteirosDados[posicao_pai - 1] = irmao_esq->ponteirosDados[irmao_esq->contador_chaves - 1];
        irmao_dir->chaves[irmao_dir->contador_chaves - 1] = -1;
        irmao_dir->ponteirosDados[irmao_dir->contador_chaves - 1] = -1;
        irmao_dir->filhos[irmao_dir->contador_chaves - 1] = irmao_dir->filhos[irmao_dir->contador_chaves];
        curPage->contador_chaves++;
        irmao_esq->contador_chaves--;
        f = fopen(NOMEARQUIVO, "r+b");
        fseek(f, curPage->RRN*sizeof(Page), SEEK_SET);
        fwrite(curPage, sizeof(Page), 1, f);
        fseek(f, pai->RRN*sizeof(Page), SEEK_SET);
        fwrite(pai, sizeof(Page), 1, f);
        fseek(f, irmao_esq->RRN*sizeof(Page), SEEK_SET);
        fwrite(irmao_esq, sizeof(Page), 1, f);
        fclose(f);
        return;
    }
    // A concatenacao raramente acontece para uma B-tree com um grande numero de chaves por pagina
  }
}

void remover() {
  int key;
  int rootRRN, foundRRN, foundPos, parentRRN;
  int resultadoBusca;

  // Leitura da chave a ser removida
  printf("Id da chave a ser removida: \n");
  scanf("%d", &key);

  FILE* fheader = fopen(NOMEHEADER, "r");
  fread(&rootRRN, sizeof(int), 1, fheader);
  fclose(fheader);
  
  resultadoBusca = busca(rootRRN, key, &foundRRN, &foundPos, &parentRRN);

  if (resultadoBusca == 1) {
    Page curPage;
    FILE* f = fopen(NOMEARQUIVO, "rb");
    fseek(f, foundRRN * sizeof(Page), SEEK_SET);
    fread(&curPage, sizeof(Page), 1, f);
    fclose(f);
    
    remove_key(&curPage, key, &parentRRN);
  } else {
    printf("Chave nao encontrada na B-tree.\n");
  }
  acessoSequencialDeDebug();
}

void delBTree(){
  if (remove(NOMEARQUIVO) == 0 && remove(NOMEHEADER) == 0)
  {
    printf("B-Tree excluida.\n");
    create_files();
  } else{
    printf("Exclusao falhou.\n");
  }
}

int main() {
  int rootRRN = init_btree();
  int opcao = 0;

  // testes
  while (opcao != 4) {
    //system("cls");
    printf("o que deseja? 1 - inserir, 2 - buscar item pelo id, 3 - remover item pelo id, 4-sair, 5-debugar, 6-deletar B-tree \n");
    scanf("%d", &opcao);
    switch (opcao)
    {
    case 1:
        inserir(&rootRRN);
        break;
    case 2:
        buscaPorId();
        break;
    case 3:
        remover();
        break;
    case 4:
        break;
    case 5:
        acessoSequencialDeDebug();
        break;
    case 6:
        printf("recomenda-se deletar manualmente\n");
        delBTree();
        break;
    default:
        printf("opcao invalida, tente novamente\n");
    }
  }
  return 0;
} 