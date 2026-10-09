#include <stdio.h>
#include <string.h>

int main()
{
    FILE *entrada;
    FILE *saida;
    char linha[100];
    char mnem[20];
    int x = 0;
    int y = 0;
    char codigo;
    int numeroLinha = 0;

    entrada = fopen("testeula.ula", "r");
    saida = fopen("testeula.hex", "w");

    // Lê o arquivo ate encontrar fim.
    while (fgets(linha, sizeof(linha), entrada) != NULL)
    {
        numeroLinha++;

        // Verifica se a linha está vazia
        if (linha[0] == '\n' || linha[0] == '\0')
        {
            printf("Erro na linha %d: linha em branco\n", numeroLinha);
            return 0;
        }
        else
        {

            // Guarda o valor de X
            if (sscanf(linha, "X=%d;", &x) == 1)
            {
            }

            // Guarda o valor de Y
            if (sscanf(linha, "Y=%d;", &y) == 1)
            {
            }

            // Quando encontra W, utiliza os últimos valores de X e Y
            if (sscanf(linha, "W=%19[^;];", mnem) == 1)
            {

                // Traduz o mnemônico para o código hexadecimal
                if (strcmp(mnem, "nA") == 0)
                {
                    codigo = '0';
                }
                else if (strcmp(mnem, "AoBn") == 0)
                {
                    codigo = '1';
                }
                else if (strcmp(mnem, "nAeB") == 0)
                {
                    codigo = '2';
                }
                else if (strcmp(mnem, "zeroL") == 0)
                {
                    codigo = '3';
                }
                else if (strcmp(mnem, "AeBn") == 0)
                {
                    codigo = '4';
                }
                else if (strcmp(mnem, "nB") == 0)
                {
                    codigo = '5';
                }
                else if (strcmp(mnem, "AxB") == 0)
                {
                    codigo = '6';
                }
                else if (strcmp(mnem, "AenB") == 0)
                {
                    codigo = '7';
                }
                else if (strcmp(mnem, "nAoB") == 0)
                {
                    codigo = '8';
                }
                else if (strcmp(mnem, "AxBn") == 0)
                {
                    codigo = '9';
                }
                else if (strcmp(mnem, "copiaB") == 0)
                {
                    codigo = 'A';
                }
                else if (strcmp(mnem, "AeB") == 0)
                {
                    codigo = 'B';
                }
                else if (strcmp(mnem, "umL") == 0)
                {
                    codigo = 'C';
                }
                else if (strcmp(mnem, "AonB") == 0)
                {
                    codigo = 'D';
                }
                else if (strcmp(mnem, "AoB") == 0)
                {
                    codigo = 'E';
                }
                else if (strcmp(mnem, "copiaA") == 0)
                {
                    codigo = 'F';
                }
                else
                {
                    printf("Erro na linha %d: instrucao inexistente\n", numeroLinha);
                    return 0;
                }

                // Junta X, Y e o código da operação
                fprintf(saida, "%X%X%c\n", x, y, codigo);
            }
        }
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}