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
  printf("chegou em split\n");
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
  printf("copiou tudo\n");
  for (int i = 0; i < MAXIMOCHAVES; i++) {
    printf("aux.chaves[%d] = %d\n", i, aux.chaves[i]);
    printf("aux.filhos[%d] = %d\n", i, aux.filhos[i]);
    if (i==MAXIMOCHAVES-1) printf("aux.filhos[%d] = %d\n", i+1, aux.filhos[i+1]);
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
  printf("dps do loop testeado posInsercao = %d\n", posInsercao);


  // transposicao elementos
  for (int i = MAXIMOCHAVES - 1; i >= posInsercao; i--) {
    aux.chaves[i + 1] = aux.chaves[i];
    aux.filhos[i + 2] = aux.filhos[i + 1];
  } 

  
  printf("nova chave inserida %d\n", *newKey);
  aux.contador_chaves++;
  aux.chaves[posInsercao] = *newKey;
  aux.filhos[posInsercao+1] = *filhoDNewKey;
  aux.ponteirosDados[posInsercao] = *newKey;
    
  printf("dps de todos ajuteste\n");

  for (int i = 0; i < MAXIMOCHAVES + 1; i++) {
    printf("chave[%d] = %d\n", i, aux.chaves[i]);
    printf("aux.filhos[%d] = %d\n", i, aux.filhos[i]);
    if (i==MAXIMOCHAVES) printf("aux.filhos[%d] = %d\n", i+1, aux.filhos[i+1]);
  }
  

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
  printf("aqui?\n");
  int posMedia = (MAXIMOCHAVES+1) / 2;
  //printf("posicao media %d\n", posMedia);
  printf("elemento na posicao Media[%d]: %d\n", posMedia, aux.chaves[posMedia]);
  (*keyPromovida) = aux.chaves[posMedia];
  //printf("por aqui mano?\n");
  
  /*set filhoDPromovida para RRN de newPage*/
  *filhoDPromovida = newPage->RRN;
  
  printf("filho da direita = %d\n", *filhoDPromovida);
  /*copiar todas chaves e ponteiros de filhos antes de promo_key da pag aux para page*/
  for (int i = 0; i < MAXIMOCHAVES; i++) {
    if (i >= posMedia) {
        page->chaves[i] = -1;
        page->ponteirosDados[i] = -1;
        if (i > posMedia) page->filhos[i] = -1;    
    } else {
        page->chaves[i] = aux.chaves[i];
        page->ponteirosDados[i] = aux.ponteirosDados[i];
        page->filhos[i] = aux.filhos[i];
        page->contador_chaves = posMedia;
        if (i == posMedia - 1) page->filhos[i+1] = aux.filhos[i+1];
    }
  }

  printf("============================\n");
  printf("como ficou page (%d) depois de ajustes:\n", page->RRN);
  for (int i = 0; i < MAXIMOCHAVES; i++) {
    printf("chave[%d] = %d\n", i, page->chaves[i]);
  }
  printf("============================\n");
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

  printf("============================\n");
  printf("como ficou page (%d) depois de ajustes:\n", newPage->RRN);
  for (int i = 0; i < MAXIMOCHAVES; i++) {
    printf("chave[%d] = %d\n", i, newPage->chaves[i]);
  }
  printf("============================\n");
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
  printf("insert entrou\n");
  printf("curRRN: %d\n", curRRN);
  int pos;
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
  // printf("chegou aqui tambem\n");
  printf("olha aqui na posicao %d o numero: %d\n", pos, curPage.filhos[pos]);
  printf("curPage.filhos[%d] = %d", pos, curPage.filhos[pos]);
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
    //printf("key promovida %d\n", keyPromovidaAux);

    // printf("pagina:\n");
    // for (int i = 0; i < MAXIMOCHAVES; i++) {
    //   printf("chave[%d] = %d\n", i, curPage.chaves[i]);
    // }
    printf("salvando chave %d na posicao %d da pagina %d\n", keyPromovidaAux, posInsercao, curRRN);
    int numPages;
    f = fopen(NOMEHEADER, "r");
    fseek(f, sizeof(int), SEEK_SET);
    fread(&numPages, sizeof(int), 1, f);
    fclose(f);

    // if (numPages == 1) { f = fopen(NOMEARQUIVO, "wb"); printf("teste fail"); }
    // else f = fopen(NOMEARQUIVO, "ab");
    // passo 4
    f = fopen(NOMEARQUIVO, "r+b");
    fseek(f, curRRN*sizeof(Page), SEEK_SET);
    fwrite(&curPage, sizeof(Page), 1, f);
    fclose(f);

    f = fopen(NOMEARQUIVO, "rb");
    Page teste;
    fseek(f, 0 * sizeof(Page), SEEK_SET);
    fread(&teste, sizeof(Page), 1, f);
    printf("aquiiii %d\n", teste.contador_chaves);
    return 0;
  } else {
    Page newPage;
    split(&keyPromovidaAux, &filhoDPromovidaAux, &curPage, keyPromovida, filhoDPromovida, &newPage);
    f = fopen(NOMEARQUIVO, "r+b");
    // escrever curPage no arquivo em currRRN (?)
    fseek(f, curPage.RRN*sizeof(Page), SEEK_SET);
    fwrite(&curPage, sizeof(Page), 1, f);
    // printf("posicao insercao aqui: %d\n", curPage.RRN);
    // lePagina(curPage);
    // inserir newpageaux no arquivo em promo_r_child
    fseek(f, newPage.RRN*sizeof(Page), SEEK_SET);
    fwrite(&newPage, sizeof(Page), 1, f);
    //printf("posicao insercao aqui: %d\n", newPage.RRN);
    //lePagina(newPage);
    fclose(f);

    // printf("\n\n teste \n\n");
    // f = fopen(NOMEARQUIVO, "rb");
    // Page teste;
    // fread(&teste, sizeof(Page), 1, f);
    // printf("result teste : \n");
    // lePagina(teste);
    // fread(&teste, sizeof(Page), 1, f);
    // printf("result teste : \n");
    // lePagina(teste);
    // fclose(f);
    return 1;
  }
  return -1;
}

void lePagina(Page page) {
    printf("pagina %d:\n", page.RRN);
    for (int i = 0; i < page.contador_chaves; i++) {
        printf("chave[%d] = %d\n", i, page.chaves[i]);
    }
    printf("********************************************\n");
}

void acessoSequencialDeDebug() {
  FILE *f = fopen(NOMEARQUIVO, "r");
  FILE* fheader = fopen(NOMEHEADER, "r");
  int rootRRN;
  int numPages;
  //fseek(fheader, sizeof(int), SEEK_SET);
  fread(&rootRRN, sizeof(int), 1, fheader);
  fread(&numPages, sizeof(int), 1, fheader);
  Page aux;
  printf("\nraiz: %d\n", rootRRN);
  printf("\n numpages %d\n", numPages);
  for (int i = 0; i < numPages; i++) {
    fread(&aux, sizeof(Page), 1, f);
    printf("pagina %d\n", aux.RRN);
    printf("chaves:\n");
    for (int j = 0; j < aux.contador_chaves; j++) {
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
  create_files();
  return 0;
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
    printf("novo root RRN: %d\n", rootRRN);
    printf("numero paginas novo: %d", numPages);
    numPages = numPages + 1;
    printf("\n\n numPages atualizado %d\n\n", numPages);
    printf("prestes a salvar raiz %d e quantidade paginas %d\n", rootRRN, numPages);
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
        printf("\n\n prestes a salvar em -> %d\n\n", novoRoot.RRN);
        fwrite(&novoRoot, sizeof(Page), 1, f);
        fclose(f);
    } else if (resultado == ERRO) {
        printf("errou ou chave ja inserida\n");
    }
    acessoSequencialDeDebug();
}

int main() {
  int rootRRN = init_btree();
  int opcao = 0;

  // testes
  while (opcao != 4) {
    //system("cls");
    printf("o que deseja? 1 - inserir, 2 - buscar item pelo id, 3 - remover item pelo id, 4 -sair, 5- debugar\n");
    scanf("%d", &opcao);
    switch (opcao)
    {
    case 1:
        inserir(&rootRRN);
        printf("nova root: %d\n",rootRRN);
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        acessoSequencialDeDebug();
        break;
    default:
        printf("opcao invalida, tente novamente\n");
    }
  }

  printf("teste final\n\n");
  FILE* f = fopen(NOMEARQUIVO, "r");
  Page final;
  fseek(f, 1*sizeof(Page), SEEK_SET);
  fread(&final, sizeof(Page), 1, f);
  printf("%d", final.RRN);
  fclose(f);
  return 0;
}