
import pickle

# Definir uma classe
class Pessoa:
    def __init__(self, nome, idade):
        self.nome = nome
        self.idade = idade

# Criar uma instância da classe
pessoa = Pessoa("mauricio", 30)

# Serializar o objeto para dados binários
dados_binarios = pickle.dumps(pessoa)

# Escrever os dados binários em um arquivo
with open('arquivo.bin', 'wb') as arquivo:
    arquivo.write(dados_binarios)