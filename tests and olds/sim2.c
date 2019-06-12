#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*			Simulador de Memoria Virtual com Paginacao
 *			Sistemas Operacionais - Prof Alex
 *
 *			Andre Klingenfus Antunes
 *			Cassiano Kruchelski Vidal
 */

typedef struct Processo{
	int id;
	char processo[3];
	char leitura[2];
	int tamanho;
	char tipoTamanho[3];
	int posicaoMemoria;
} PROCESSO;

int simuladorMemoriaVirtual(int numPaginas, int numQuadros, int paginas[], int modo);
int leastRecentlyUsed(int *tempo, int n);
int leastRecentlyUsedAproximado(int *tempo, int *bitRef, int n);
int segundaChance(int *tempo, int *bitRef, int n);
void verificarParametros(int argc, char **argumentos);
void carregarProcessos();

int tamanhoPaginaQuadro, tamanhoMemoria;

int main (int argc, char *argv[])
{	
	verificarParametros(argc, argv);

	printf("Tamanho de Pagina / Quadro inserido: %d\n", tamanhoPaginaQuadro);
	printf("Tamanho de Memoria informada:        %d\n", tamanhoMemoria);	

	//carregarProcessos();

	int numPaginas, numQuadros, cont, paginas[30];
	int flag1, flag2;

	printf("Numero de paginas: "); scanf("%d",&numPaginas);
	printf("Numero de quadros: "); scanf("%d",&numQuadros);

	for (cont = 0; cont < numPaginas; cont++)	
		scanf("%d",&paginas[cont]);

	simuladorMemoriaVirtual (numPaginas, numQuadros, paginas, 1);
	simuladorMemoriaVirtual (numPaginas, numQuadros, paginas, 2);
	simuladorMemoriaVirtual (numPaginas, numQuadros, paginas, 3);

	return 0;
}

/* Abre o arquivo de texto na mesma pasta, faz a leitura dos dados e envia para um elemento
*  da Struct Processo - com as informações de ID, Processo, Tipo, Tamanho, TipoTamanho ou
*  posicao na memoria (lido em %x e impresso em %d)
*/

void carregarProcessos()
{
	char linha[15], processo[3], leitura[2], tipoTamanho[3];
	int tamanho, posicaoMemoria;

	FILE *arq;

	arq = fopen("dadosEntrada.txt", "rt");

	if (arq == NULL)
	{
		printf("Problemas ao abrir arquivo\n");
		return ;
	}

	int count = 0, i = 0, id = 0;	

	while(!feof(arq))
	{	
		fscanf(arq,"%s", processo);
		fscanf(arq,"%s", leitura);
		if (strcmp (leitura,"C") == 0)
		{
			fscanf(arq,"%d", &tamanho);
			fscanf(arq,"%s", tipoTamanho);
			posicaoMemoria = 0;
		}
		if (strcmp (leitura,"R") == 0 || strcmp (leitura,"W") == 0)
		{
			fscanf(arq,"%x", &posicaoMemoria);
			tamanho = 0;
			strcpy(tipoTamanho," ");		
		}		

		setbuf(stdin, NULL);

		PROCESSO *p  = (PROCESSO*) malloc(sizeof(PROCESSO));

		p -> id = id;
		strcpy(p -> processo, processo);
		strcpy(p -> leitura, leitura);
		p -> tamanho = tamanho;
		strcpy(p -> tipoTamanho, tipoTamanho);
		p -> posicaoMemoria = posicaoMemoria;

		if (strcmp (leitura,"C") == 0)
			printf("Id: %d String: %s %s %d %s\n", p -> id, p -> processo, p -> leitura, p -> tamanho, p -> tipoTamanho);					

		if (strcmp (leitura,"R") == 0 || strcmp (leitura,"W") == 0)
			printf("Id: %d String: %s %s %d\n", p -> id, p -> processo, p -> leitura, p -> posicaoMemoria);

		id++;
	}		
	fclose(arq);
}

/* Verifica parametros inseridos no console -p para o tamanho de pagina e -m para o tamanho
*  de memoria, a funcao recebe os parametros argv[] que são convertidos para int por meio da
*  funcao atoi e devolvidos a variavel global
*/

void verificarParametros(int argc, char **argumentos)
{
	int i;
	char *PaginaQuadro, *Memoria;

	for (i = 1; i < argc; i++)
	{
		if (strcmp (argumentos[i],"-p") == 0)
		{			
			PaginaQuadro = argumentos[i + 1];
			tamanhoPaginaQuadro = atoi(PaginaQuadro);
		}
		if (strcmp (argumentos[i],"-m") == 0)
		{			
			Memoria = argumentos[i + 1];
			tamanhoMemoria = atoi(Memoria);
		}
	}
}

/* LRU - O algoritmo substitui o quadro que é menos recentemente utilizado pelas paginas,
*  verifica o tempo em que cada página entrou no código - valores armazenados no vetor tempo[]
*  compara o tempo de cada posicao do vetor com o tempo da primeira posicao do vetor
*  caso o tempo seja menor que o minimo, troca-se o valor e retorna a posicao que deve
*  ser trocada pelo algoritmo
*/ 

int leastRecentlyUsed (int *tempo, int n)
{
	int cont, minimo = tempo[0], pos = 0;		

	for (cont = 1; cont < n; ++cont)	
		if (tempo[cont] < minimo)		
		{								
			minimo = tempo[cont];		
			pos = cont;					
		}	
	return pos;							
}

/* LRU Aproximado - Funciona da mesma maneira que o LRU somada da etapa de verificao do bit de 
*  de referencia, se o bitRef for 1 ele e reduzido a 0 e o algoritmo busca a proxima posicao
*  caso o valor de tempo seja menor e o bitRef for 0 o algoritmo retorna a poiscao que deve ser
*  trocada
*/ 

int leastRecentlyUsed_bitRef (int *tempo, int *bitRef, int n)
{
	int cont, minimo = tempo[0], pos = 0;	

	for (cont = 1; cont < n; ++cont)	
		if (tempo[cont] < minimo)		
		{	
			if (bitRef[cont] == 0)		
			{
				minimo = tempo[cont];	
				pos = cont;				
			}
			if (bitRef[cont] == 1)		
				bitRef[cont] = 0;
		}	
	return pos;							
}

/* Second Chance - Funciona em cima da lógica FIFO - First in First out seguida de uma verificao
*  de um bit de referencia, a primeira pagina que entrou no quadro sera a primeira a sair,
*  independente se foi acessada mais recentemente ou não. O tempo que é enviado a essa função
*  é o tempo em que a pagina entrou no quadro.
*/

int SecondChance (int *tempo, int *bitRef, int n)
{
	int cont, minimo = tempo[0], pos = 0;	

	for (cont = 1; cont < n; ++cont)	
		if (tempo[cont] < minimo)		
		{	
			if (bitRef[cont] == 0)		
			{
				minimo = tempo[cont];	
				pos = cont;				
			}
			if (bitRef[cont] == 1)		
				bitRef[cont] = 0;
		}	
	return pos;							
}

int simuladorMemoriaVirtual (int numPaginas, int numQuadros, int *paginas, int modo)
{
	int flag1, flag2, cont, contQuadros, pos;						// Tempo 			- Tempo corrido a cada etapa
	int contador = 0, contadorSemTrocas = 0, pageFaults = 0;		// Tempo Sem Troca  - Nao adiciona tempo se a pagina ja estiver carregada p/ Second Chance
	int quadros[10], tempo[10], tempoSemTrocas[10], bitRef[10];

	for (cont = 0; cont < numQuadros; cont++)					// Zerar vetores
	{
		quadros[cont] = -1;	    bitRef[cont] = 0; 				
		tempo[cont] = 0;		tempoSemTrocas[cont] = 0;
	}		
		
	
	for (cont = 0; cont < numPaginas; cont++)		
	{
		flag1 = flag2 = 0;		// Flags zerados = pagina nao encontrada nos quadros

		/* Inicialmente, pega a primeira pagina e percorre o vetor de quadros, verificando se a
		*  pagina ja esta carregda no vetor, caso esteja os flags se tornam 1 (valor encontrado)
		*  e o tempo e incrementado
		*/

		for (contQuadros = 0; contQuadros < numQuadros; contQuadros++)		
			if (quadros[contQuadros] == paginas[cont])						
			{								
				tempo[contQuadros] = ++contador;				
				bitRef[contQuadros] = 1;	
				flag1 = flag2 = 1;	
				break;
			}			

		/* Flag 1 - Verifica se os quadros estao vazios primeiramente, carrega a pagina solicitada
		*  no quadro, incrementa o valor do tempo e o numero de Page Faults - Como a pagina é
		*  carregada nos quadros o Flag 2 é pulado.
		*/

		if (flag1 == 0)														
			for (contQuadros = 0; contQuadros < numQuadros; contQuadros++)	
				if (quadros[contQuadros] == -1)								
				{					
					quadros[contQuadros] = paginas[cont];					
					tempoSemTrocas[contQuadros] = ++contadorSemTrocas;
					tempo[contQuadros] = ++contador;						
					pageFaults++;											
					flag2 = 1;												
					break;
				}			
		
		// Procurar pagina nos quadros se todos estiverem com paginas carregadas ( != -1 )

		/* Flag 2 - Com todas os quadros cheios, é necessario verificar em qual posicao a nova
		*  pagina devera entrar - modo 1 LRU - modo 2 LRUaproximado - modo 3 SecondChance
		*  as funcoes devolvem a posicao do quadro em que o novo elemento entrara, incrementa
		*  o contador de tempo e o numero de Page Faults
		*/

		if (flag2 == 0)
		{
			switch (modo)
			{
				case 1:			// LRU				
					pos = leastRecentlyUsed(tempo, numQuadros);		
					quadros[pos] = paginas[cont];						
					tempo[pos] = ++contador;							
					pageFaults++;										
					break;				
				case 2:			// LRUAproximado				
					pos = leastRecentlyUsed_bitRef(tempo, bitRef, numQuadros);	
					quadros[pos] = paginas[cont];								
					tempo[pos] = ++contador;									
					pageFaults++;												
					break;				
				case 3:			// SecondChance				
					pos = SecondChance(tempoSemTrocas, bitRef, numQuadros);	
					quadros[pos] = paginas[cont];							
					tempoSemTrocas[pos] = ++contadorSemTrocas;												
					pageFaults++;																
					break;				
			}
		}

		printf("\n");

		if (modo == 1)
			for (contQuadros = 0; contQuadros < numQuadros; ++contQuadros)
				printf("Quadro: %d \t  Tempo de Entrada: %d \n", quadros[contQuadros],tempo[contQuadros]);		
		if (modo == 2)
			for (contQuadros = 0; contQuadros < numQuadros; ++contQuadros)
				printf("Quadro: %d \t BitReferencia: %d \t Tempo de Entrada: %d \n", quadros[contQuadros],bitRef[contQuadros],tempo[contQuadros]);
		if (modo == 3)
			for (contQuadros = 0; contQuadros < numQuadros; ++contQuadros)
				printf("Quadro: %d \t BitReferencia: %d \t Tempo de Entrada: %d \n", quadros[contQuadros],bitRef[contQuadros],tempoSemTrocas[contQuadros]);

	}

	printf("Numero de Page Faults: %d\n", pageFaults );		// Mostra o total de Page Faults

	return 1;
}