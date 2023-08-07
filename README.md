# B-Tree com arquivos não voláteis - Aplicação com E-Commerce de grande Volume

| UFSCAR (Universidade Federal de São Carlos) |

Projeto de ORI (Organização e Recuperação de Informação) realizado pelos alunos:
- Cauã Benini da Silva - RA 801046
- Lucas Pereira Quadros - RA 800981
- Maurício Gallera de Almeida - RA 800366

## B-Tree (Árvores-B)
As B-Trees são estruturas de dados advindas do conceito de árvore binária amplamente utilizadas quando há um grande volume de informação - 
tão grande que não se pode guarda-la em memória primária. Nela, cada nó da árvore representa uma página, a qual é armazenada em disco; ao longo da busca,
carrega-se a página de interesse em memória primária, se realiza a busca sequencial (tendo em vista a velocidade da memória primária) e se encontrar, finaliza a operação de busca;
senão, continua a carregar as páginas subsequentes de acordo com as regras do algoritmo a seguir.
- A operação de busca começa pelo nó raiz e a partir disso, temos quatro opções, que devem ser verificadas em sequência:
1. Verifica se esse é o valor encontrado, se sim, encerra a operação;
2. Verifica se esse é um valor NULL, se sim, encerra a operação, pois não achou;
3. Se o valor procurado for menor que o valor do nó, parte para o nó filho da esquerda e retorna ao passo “1”;
4. Se o valor procurado for maior que o valor do nó, parte para o nó filho da direita e retorna ao passo “1”.

- Exemplo:
  
![image](https://github.com/mau0414/arvoreB/assets/99350970/2fb81da6-01b8-419c-b40c-267a9da8bb7d)

Nesse caso, estamos trabalhando com índices, e dentro de cada página, há um vetor de índices que apontam para o registro do produto do E-Commerce de grande volume.
Isso resulta em uma busca eficiente, garantindo, no pior caso, O(log(n)) acessos a disco e, da mesma forma, uma manutenção menos custosa que um arquivo de índices linear.
