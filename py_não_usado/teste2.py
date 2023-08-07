import pickle

# Definir uma classe
class Pessoa:
    def __init__(self, nome, idade):
        self.nome = nome
        self.idade = idade


# Ler os dados binários de um arquivo
with open('arquivo.bin', 'rb') as arquivo:
    dados_binarios = arquivo.read()

# Desserializar os dados binários para recriar o objeto
pessoa = pickle.loads(dados_binarios)

# Acessar os atributos do objeto
print(pessoa.nome)   # Saída: João
print(pessoa.idade)  # Saída: 30