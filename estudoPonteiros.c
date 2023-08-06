#include <stdio.h> 

// px eh ponteiro pra int que recebe endereco de x ou xp diretamente
void mudaValor(int* px) {
    *px = 2;
    printf("valor dentro da funcao: %d\n", *px);
}

typedef struct {
    int idade;
    int id;
} Pessoa;

int main() {
    int* xp;
    int x = 1;
    xp = &x;
    printf("valor de x antes da funcao: %d\n", x);
    //mudaValor(&x);
    mudaValor(xp);
    printf("valor de x depois da funcao: %d\n", x);

    int intTeste = 2;
    Pessoa pessoaTeste;
    pessoaTeste.idade = 12;
    pessoaTeste.id = 1;
    FILE* f = fopen("arqruivoTeste.bin", "wb");
    fwrite(&intTeste, sizeof(int), 1, f);
    fwrite(&pessoaTeste, sizeof(Pessoa), 1, f);
    fclose(f);

    int leitorInt;
    Pessoa leitorPessoa;
    f = fopen("arqruivoTeste.bin", "rb");
    fread(&leitorInt, sizeof(int), 1, f);
    fread(&leitorPessoa, sizeof(Pessoa), 1, f);
    fclose(f);

    printf("%d\n", leitorInt);
    printf("idade %d id  %d\n", leitorPessoa.idade, leitorPessoa.id);

    printf("resultado: %d", 5 /2);
    return 0;
}