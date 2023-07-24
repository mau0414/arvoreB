import csv
import pickle
import bintrees # biblioteca pronta que usa RBTree (só para testar)

# Criar uma B-tree vazia
btree = bintrees.RBTree()

# Carregar os registros do arquivo CSV na B-tree
with open('flipkart_sample.csv', newline="", encoding="utf-8") as arquivo_csv:
    leitor_csv = csv.DictReader(arquivo_csv)
    
    for registro in leitor_csv:
        # Obter o valor da chave primária (uniq_id) e criar uma chave binária para a B-tree
        chave_primaria = registro['uniq_id']
        chave_binaria = chave_primaria.encode("utf-8")  

        # Inserir o registro na B-tree usando a chave binária como chave e o registro completo como valor
        btree.insert(chave_binaria, registro)

# Salvar a B-tree em um arquivo usando pickle
with open('database.pkl', 'wb') as arquivo:
    pickle.dump(btree, arquivo)