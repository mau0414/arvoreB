import pickle
import bintrees # biblioteca pronta que usa RBTree (só para testar)

# Carregar a B-tree do arquivo
with open('database.pkl', 'rb') as arquivo:
    loaded_btree = pickle.load(arquivo) # sem paginação por enquanto

# Converter para o tipo RBTree
if isinstance(loaded_btree, bintrees.RBTree):
    loaded_btree = bintrees.RBTree(loaded_btree)
else:
    raise ValueError("A variável não é uma instância de RBTree")

# Exemplo de busca
key = "05324708277dbf2f3e8b55a660e91988" # linha 88 do .csv
print(loaded_btree.get(key.encode("utf-8")))