#include <stdio.h> 

// px eh ponteiro pra int que recebe endereco de x ou xp diretamente
void mudaValor(int* px) {
    *px = 2;
    printf("valor dentro da funcao: %d\n", *px);
}

int main() {
    int* xp;
    int x = 1;
    xp = &x;
    printf("valor de x antes da funcao: %d\n", x);
    //mudaValor(&x);
    mudaValor(xp);
    printf("valor de x depois da funcao: %d\n", x);

    return 0;
}