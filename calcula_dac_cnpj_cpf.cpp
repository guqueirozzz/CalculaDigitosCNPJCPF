#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <thread>
#include <string>
#include <WIndows.h>


using namespace std;


//variáveis globais
char leitor[17];
double tp_total_proc;
double tp_abertura_arquivo;
double tp_leitura_arquivo;
double tp_calcula_cpf_cnpj;
double tp_total;
double tp_escreve_console;
double tp_orquestrador;
int id_cpf = 0;
int id_cnpj = 0;
int dados_no_arquivo = 0;
int qtd_cpf = 0;
int qtd_cnpj = 0;
int cpfs [1000][12];
int cnpjs [1000][17];

clock_t tick[10];     // ticks para calculo de tempo de execução utilizado pelas threads e funções

// protótipos
void calcula_digito_cpf(int);
void calcula_digito_cnpj(int);
void orquestador();
void relatorio();
void le_arquivo();
bool abre_arquivo();

FILE* base;
FILE* base_output;

bool abre_arquivo()
{

    base = fopen("BASE.txt", "r");
    if ( base == NULL)
    {
        printf("arquivo com erro.\n");
        return false;
    }
    else
        return true;


}

void le_arquivo()
{
    printf("Lendo o arquivo... \n");

    //For para leitura das linhas da base ( limitado em 1000 linhas ) para ler mais linhas basta colocar outro numero
    for(int i=0; i < 1000; i++)
    {
        fscanf(base,"%s", leitor);		//Lendo a linha

        int cont_digitos = 0;		//Contador para decidir se o numero é cpf ou cnpj
        int it=0;		            //iterador para andar digito por digito do vetor char


        for(int k = 0; leitor[it]!= '\0'; k++)
        {
            cont_digitos++;			//somando contador
            it++;		            //somando iterador
        }

        //Se contador > 11 o numero é de um CNPJ
        if(cont_digitos > 11)
        {
            for(int j = 0; j <= 12; j++)
                cnpjs[id_cnpj][j] = leitor[j] - '0';            // conversão de string para int

            id_cnpj++;  // variavel de controle para contabiliar quantos cnpj's foram lidos até agora
        }

        //Se nao o numero é de um CPF
        else
        {
            for(int j = 0; j < 9; j++)
                cpfs[id_cpf][j] = leitor[j] - '0';			   // conversão de string para int

            id_cpf++; // variavel de controle para contabiliar quantos cpf's foram lidos até agora
        }

    }
}



// função onde é implementada a chamada das funções com as threads.
void calcula_cpf_cnpj()
{

	for(int i = 0; i < id_cpf; i++)
    {

		std::thread calculo_cpf(calcula_digito_cpf, i); // abertura da thread passando a função calcula_digito_cpf
        calculo_cpf.join();                             // join


        printf("CPF: ");
        int jj = 0;
		while(jj != 11) // while para printar os cpf's com os dois digitos verificadores calculados
        {
            cout << cpfs[i][jj];
            jj++;
        }


		cout<<endl;
	}


	for(int i = 0; i<id_cnpj;i++)
    {
		std::thread calculo_cnpj(calcula_digito_cnpj, i); // abertura da thread passando a função calcula_digito_cpf
        calculo_cnpj.join();                              // join

        printf("CNPJ: ");
        int j = 0;
        while(j != 14) // while para printar os cnpj's com os dois digitos verificadores calculados
        {
            cout << cnpjs[i][j];
            j++;
        }

		cout<<endl;
	}


	fclose(base); // fechando o arquivo

}

// função onde é efetuado o calculo dos digitos verificadores do cpf
void calcula_digito_cpf(int iterador)
{
	int dig_1 = 0;          // digito verificador 1 (DAC 1)
	int dig_2 = 0;          // digito verificador 2 (DAC 2)

	dig_1 = (cpfs[iterador][0]*10)+(cpfs[iterador][1]*9)+(cpfs[iterador][2]*8)+(cpfs[iterador][3]*7)+(cpfs[iterador][4]*6)+(cpfs[iterador][5]*5)+(cpfs[iterador][6]*4)+(cpfs[iterador][7]*3)+(cpfs[iterador][8]*2);
        dig_1 = dig_1%11;
        dig_1 = 11 - dig_1;

        if(dig_1 >=10)
            dig_1 = 0;

    dig_2 = (cpfs[iterador][0]*11)+(cpfs[iterador][1]*10)+(cpfs[iterador][2]*9)+(cpfs[iterador][3]*8)+(cpfs[iterador][4]*7)+(cpfs[iterador][5]*6)+(cpfs[iterador][6]*5)+(cpfs[iterador][7]*4)+(cpfs[iterador][8]*3)+(dig_1*2);
        dig_2 = dig_2%11;
        dig_2 = 11 - dig_2;

        if(dig_2 >=10)
            dig_2=0;

	cpfs[iterador][9]  = dig_1; // após o calculo do digito 1 e digito 2, eles são colocados no final do vetor de cpfs
	cpfs[iterador][10] = dig_2; // após o calculo do digito 1 e digito 2, eles são colocados no final do vetor de cpfs
	printf("\nDAC1 %d, DAC2 %d \n", dig_1, dig_2);
}

// função onde é efetuado o calculo dos digitos verificadores do cnpj
void calcula_digito_cnpj(int iterador)
{
	int dig_1 = 0;          // digito verificador 1 (DAC 1)
	int dig_2 = 0;          // digito verificador 2 (DAC 2)

	dig_1 = (6*cnpjs[iterador][0])+(7*cnpjs[iterador][1])+(8*cnpjs[iterador][2])+(9*cnpjs[iterador][3]);
	dig_1 = dig_1 + ((2*cnpjs[iterador][4])+(3*cnpjs[iterador][5])+(4*cnpjs[iterador][6])+(5*cnpjs[iterador][7]));
	dig_1 = dig_1 + ((6*cnpjs[iterador][8])+(7*cnpjs[iterador][9])+(8*cnpjs[iterador][10])+(9*cnpjs[iterador][11]));
	dig_1 = dig_1%11;

    if ( dig_1 >= 10 )
        dig_1 = 0;

	dig_2 = (5*cnpjs[iterador][0])+(6*cnpjs[iterador][1])+(7*cnpjs[iterador][2])+(8*cnpjs[iterador][3]);
	dig_2 = dig_2 + ((9*cnpjs[iterador][4])+(2*cnpjs[iterador][5])+(3*cnpjs[iterador][6])+(4*cnpjs[iterador][7]));
	dig_2 = dig_2 + ((5*cnpjs[iterador][8])+(6*cnpjs[iterador][9])+(7*cnpjs[iterador][10])+(8*cnpjs[iterador][11] + (9*dig_1)));
	dig_2 = dig_2%11;

	if ( dig_2 >= 10 )
        dig_2 = 0;

	cnpjs[iterador][12] = dig_1; // após o calculo do digito 1 e digito 2, eles são colocados no final do vetor de cnpj's
	cnpjs[iterador][13] = dig_2; // após o calculo do digito 1 e digito 2, eles são colocados no final do vetor de cnpj's
	printf("\nDAC1 %d, DAC2 %d \n", dig_1,dig_2);
}

// orquestrador para apresentação centralizada das informações
void orquestrador()
{
    int total_linhas_analisadas = id_cpf + id_cnpj;
    tp_abertura_arquivo = ((tick[1] - tick[0]) * 1000.0 / CLOCKS_PER_SEC) * 0.001; // variaveis para calculo de tempo de execuçao.
    tp_leitura_arquivo  = ((tick[3] - tick[2]) * 1000.0 / CLOCKS_PER_SEC) * 0.001; // variaveis para calculo de tempo de execuçao.
    tp_calcula_cpf_cnpj = ((tick[5] - tick[4]) * 1000.0 / CLOCKS_PER_SEC) * 0.001; // variaveis para calculo de tempo de execuçao.
    tp_total            = ((tick[5] - tick[0]) * 1000.0 / CLOCKS_PER_SEC) * 0.001; // variaveis para calculo de tempo de execuçao.

    printf("=======================INFORMACOES DA SIMULACAO========================= \n");
    printf(" Tempo gasto na abertura do arquivo: %g m/s \n", tp_abertura_arquivo);
    printf(" Tempo gasto para a leitura do arquivo: %g m/s \n", tp_leitura_arquivo);
    printf(" Tempo gasto para calcular: %g m/s \n", tp_calcula_cpf_cnpj);
    printf(" Tempo total gasto: %g m/s \n", tp_total);
    printf(" Quantidade total de cpfs lidos = %d\n", id_cpf);
    printf(" Quantidade total de cnpjs lidos = %d\n", id_cnpj);
    printf(" Quantidade total de linhas analisadas = %d\n", total_linhas_analisadas );
    printf("======================================================================== \n");
}


int main()
{
    // chamada das funções e clocks para calculo de tempo de execução.
    tick[0] = clock();
    abre_arquivo();
    tick[1] = clock();

    tick[2] = clock();
    le_arquivo();
    tick[3] = clock();

    tick[4] = clock();
	calcula_cpf_cnpj();
    tick[5] = clock();

    orquestrador();



	return 0;
}
