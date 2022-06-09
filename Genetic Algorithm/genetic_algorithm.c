#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


//MACROS
//Printa na tela somente quando o usuario tiver selecionado a opcao verbose
#define printf(param, text) ({verbose ? (printf(param, text)) : 0;})
int verbose = 1;


//CONSTANTES
#define VAR 1                              //Quantidade de variaveis alocadas por individuo
#define POPULATION_N 50                    //Numero de individuos em uma populacao
#define MUTATION_MIN 5                     //Minimo de mutacao
#define MUTATION_MAX 30                    //Maximo de mutacao
#define variable_size (sizeof(int)*8)/VAR  //Tamanho em bits de uma variavel



//GLOBAIS
float a, b, c, d, e, f; //Coeficientes para a equacao de quinto grau
float good_enough = 0.002; //Quao perto o resultado precisa estar


//FUNCOES

//1. AUXILIARES

//Entrada: a quantidade de 1s consecutivos da direita pra esquerda (binario)
//Processamento: shifta 1 para todas as posicoes no intervalo [0,ones[, adicionando a uma variavel
//Saida: decimal do valor inicialmente binario
//Uso: valor retornado sera usado para mascaras binarias
int binary_ones_to_decimal(int ones);

//Entrada: valor de uma variavel de um individuo
//Processamento: separa em digitos de mantissa e digitos de expoente. A mantissa sera os primeiros 3/4 da variavel, ou seja, o resultado da operacao AND com uma mascara (shiftada para esquerda para ignorar o expoente) de tamanho igual 3/4 da variavel (e shiftada de volta no final). O expoente sera o AND com o 1/4 restante da variavel. O primeiro bit da variavel sera usado como sinal ao inves de ser parte da mantissa. Move os digitos significantes para direita com base no expoente (mantissa/(2^expoente))
//Saida: float resultante
//Uso: pegar o valor da conversao de uma variavel guardada no individuo (int) para numero com ponto (float)
float get_float(int variable);


//2. PRINCIPAIS

//Entrada: numero de individuos da populacao
//Processamento: aloca memoria para um vetor[n] e utiliza modulo do rand para gerar numeros semi-aleatorios para cada individuo
//Saida: ponteiro com o vetor da populacao
//Uso: gerar a primeira populacao
int* generate_random_population(int n);

//Entrada: ponteiro do individuo e chance de mutacao em porcentagem
//Processamento: trunca para inteiro a representacao decimal da porcentagem (100/porcentagem). Para cada bit do individuo, dependendo da chance (atraves do uso do rand % chance), pode fazer um xor com 1 para realizar o bit flip
//Saida: void, pois altera diretamente o individuo
//Uso: realizar mutacoes a fim de garantir a chegada de resultados melhores
void mutation(int* individual, float chance);

//Entrada: valor de um individuo
//Processamento: cria um vetor para guardar o valor das variaveis do individuo. Para obte-las, shifta uma mascara binaria para a posicao da variavel dentro do individuo, realiza a operacao AND, e depois shifta de volta. Apos isso, converte esse valor para float. Calcula a equacao de quinto grau com os valores das variaveis e compara a distancia desse valor a -f
//Saida: modulo da distancia
//Uso: aferir o quao proximo da resposta o individuo esta
float evaluate_individual(int individual);

//Entrada: ponteiro com o vetor da populacao e o numero de individuos
//Processamento: cria um vetor[n] para colocar a precisao aferida de cada individuo. Caso encontre uma precisao abaixo de um numero suficientemente pequeno, exibe a resposta e finaliza o programa. Compara cada individuo com o anterior e altera a ordem a fim de fazer um sort do melhor para o pior. Quando o numero de passadas for igual a n-1 (j == n-1), ou tiver uma passada perfeita (clear+j == n-1), significa que o sort esta terminado
//Saida: void, pois altera diretamente o vetor da populacao
//Uso: ordenar do melhor para o pior uma populacao
void select_population(int* population, int n);

//Entrada: valores de dois individuos
//Processamento: aloca memoria para um vetor[2] para os filhos. Foca em metade de uma variavel por vez, adicionando a de cada pai para filhos diferentes ("pai1 -> filho1, pai2 -> filho2"); alterna o pai doador para certo filho a cada iteracao ("pai2 -> filho1, pai1 -> filho2"). Para isso, shifta para esquerda uma mascara binaria, realiza o AND com o pai a fim de obter a metade da variavel desejada, e adiciona essa variavel ao filho. Utiliza a paridade da iteracao para decidir o pai doador
//Saida: ponteiro do vetor com os filhos
//Uso: cruzar dois individuos e obter seus filhos
int* breed_two_individuals(int individual_1, int individual_2);



int main(){
    //Seed do rand com o tempo
    srand(time(NULL));

    //Pega os coeficientes e mostra a equacao na tela
    printf("%s", "Insert the coefficients a, b, c, d, e, and f: ");
    scanf("%f %f %f %f %f %f", &a, &b, &c, &d, &e, &f);
    printf("\nEquation: %.2f*x^5", a);
    printf(" + %.2f*x^4", b);
    printf(" + %.2f*x^3", c);
    printf(" + %.2f*x^2", d);
    printf(" + %.2f*x", e);
    printf(" + %.2f = 0\n\n", f);

    //Pega do usuario a corretude necessaria
    printf("%s", "Correctness (-1 for default (0.002)): ");
    float good_enough_temp;
    scanf("%f", &good_enough_temp);
    if(good_enough_temp != -1) good_enough = good_enough_temp;

    //Opcao verbose
    char verbose_temp;
    printf("%s", "Verbose (y/n): ");
    scanf(" %c", &verbose_temp);
    if(verbose_temp == 'n') verbose = 0;


    //Gera populacao inicial aleatoria de tamanho POPULATION_N e faz um sort do melhor para o pior individuo
    int *population = generate_random_population(POPULATION_N);
    select_population(population, POPULATION_N);

    //Imprime o numero inteiro de todos os individuos da populacao inicial
    for(int i = 1; i <= POPULATION_N; ++i){
	printf("Number %d: ", i);
	printf("%d\n", population[i-1]);
    }

    //Aloca memoria para uma segunda populacao
    int *population2 = malloc(sizeof(int)*POPULATION_N);

    while(1){
	//O melhor individuo da populacao passada sera mantido na atual
	population2[0] = population[0];

	//Os outros individuos serao o melhor filho do cruzamento do melhor da geracao passada com todos os outros
	printf("%s", "* Breeding and mutating population... *\n\n");
	for(int i = 1; i < POPULATION_N; ++i){
	    int *offspring = breed_two_individuals(population[0], population[i]);
	    select_population(offspring, 2);
	    population2[i] = offspring[0];
	    free(offspring);
	}

	//Faz o sort da populacao atual
	select_population(population2, POPULATION_N);

	//Os individuos melhores tem uma menor chance de mutacao (comecando em MUTATION_MIN), que vai ate MUTATION_MAX para o pior
	printf("%s", "* Mutating population... *\n\n");
	for(int i = 0; i < POPULATION_N; ++i){
	    mutation(&population2[i], ( (float) (MUTATION_MAX-MUTATION_MIN)/POPULATION_N)*i + MUTATION_MIN);
	}

	//Realiza novamento o sort da populacao
	select_population(population2, POPULATION_N);

	//A populacao atual substitui a passada
	population = population2;
    }

    free(population2);
    free(population);
}



//FUNCOES
int binary_ones_to_decimal(int ones){
    int decimal = 0;

    //Ex, ones = 4
    //decimal = 1000 + 100 + 10 + 1 = 1111 = 15
    for(; ones > 0; --ones){
	decimal += 1 << (ones - 1);
    }

    return decimal;
}

float get_float(int variable){
    int exponent_size = variable_size/4;
    int significant_size = variable_size*3/4;

    //Mascara pegando os primeiros (3/4 - 1) de digitos. O primeiro sera usado para o sinal
    int significant = variable & (binary_ones_to_decimal(significant_size-1) << exponent_size);
    if ((1 << (variable_size - 1)) & variable){
	significant = -significant;
    }

    int exponent = variable & binary_ones_to_decimal(exponent_size);

    float float_conversion = (float) significant / pow(2, exponent);

    printf("-> Value: %f\n", float_conversion);

    //Retorna como float o significante dividido por 2^expoente
    return float_conversion;
}

int* generate_random_population(int n){
    int *population = malloc(sizeof(int)*n);
    printf("%s", "* Generating random population... *\n\n");

    for(; n > 0; --n){
	//Modulo com o maior int
	population[n-1] = rand()%4294967296;
	printf("%d ", population[n-1]);
    }

    printf("%s", "\n\n");

    return population;
}

void mutation(int* individual, float chance){
    chance = 100/chance;

    for(int n = (sizeof(int)*8)-1; n >= 0; --n){
	if((rand() % (int) chance) == 1){
	    *individual ^= (1 << (n));
	}
    }

    return;
}

float evaluate_individual(int individual){
    float variables[VAR];
    printf("Individual: %d\n", individual);
    for(int n = VAR-1; n >= 0; --n){
	//Shifta o individuo para a direita para isolar a variavel desejada
	variables[n] = get_float(((individual) & (binary_ones_to_decimal(variable_size) << ((n)*variable_size))) >> (n)*variable_size);
    }

    float evaluation = -f - (a*pow(variables[0],5) + b*pow(variables[0],4) + c*pow(variables[0],3) + d*pow(variables[0],2) + e*pow(variables[0],1));
    printf("Difference: %f\n\n", evaluation);
    evaluation = fabs(evaluation);
    return evaluation;
}

void select_population(int* population, int n){
    printf("%s", "* Sorting individuals... *\n\n");

    float n_accuracy[n];

    for(int i = n-1; i >= 0; --i){
	n_accuracy[i] = evaluate_individual(population[i]);
	if(n_accuracy[i] < good_enough){
	    verbose = 1;
	    printf("\n**** Best individual is %d ****\n\n", population[i]);
	    evaluate_individual(population[i]);
	    exit(0);
	}
    }

    int clear = 0;
    int j = 0;
    while(clear+j < n-1){
	clear = 0;
	for(int i = 1; i < n-j; ++i){
	    //Menor eh melhor
	    if(n_accuracy[i] < n_accuracy[i-1]){
		int temp_population = population[i-1];
		float temp_accuracy = n_accuracy[i-1];

		population[i-1] = population[i];
		n_accuracy[i-1] = n_accuracy[i];
		population[i] = temp_population;
		n_accuracy[i] = temp_accuracy;
	    }
	    else{
		++clear;
	    }
	}
	++j;
    }

    return;
}

int* breed_two_individuals(int individual_1, int individual_2){
    int* offspring = malloc(sizeof(int)*2);
    offspring[0] = offspring[1] = 0;

    for(int i = 0; i < VAR*2; ++i){
	if(i % 2 != 0){
	    offspring[0] += ((individual_1) & (binary_ones_to_decimal(variable_size/2) << i*variable_size/2));
	    offspring[1] += ((individual_2) & (binary_ones_to_decimal(variable_size/2) << i*variable_size/2));
	}
	else{
	    offspring[0] += ((individual_2) & (binary_ones_to_decimal(variable_size/2) << i*variable_size/2));
	    offspring[1] += ((individual_1) & (binary_ones_to_decimal(variable_size/2) << i*variable_size/2));
	}
    }

    return offspring;
}
