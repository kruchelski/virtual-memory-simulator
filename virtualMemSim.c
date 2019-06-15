/*	Simulador de memória virtual
*	
*	Autores: André Klingenfus Antunes & Cassiano Kruchelski Vidal
*
*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define ARQUIVO "process.txt" // nome do arquivo com as instruções

typedef char texto[4];
typedef struct _PROCESSO {
	texto id;
	int *pageTable;
	unsigned long int procSize;
	int pageQnt;
	int pageAloc;
} PROCESSO;

typedef struct _MEMORIA {
	int *frames;
	texto *frameProcId;
	int *time;
	int *timeUnchange;
	int *bitRef;
} MEMORIA;

void verifyParameters(int argc, char **args, int *fSize, int *memSize);
int processCounter();
unsigned long int toByte(texto sizeUnit, int procSize);
void processScale(PROCESSO *p);
void initTables(PROCESSO *p, int fSize, int processCont);
void initMemory(MEMORIA *m, int framesQnt);
int simuladorMemoriaVirtual(PROCESSO *process, int framesQnt, int processCont, int modo, int fSize);
int leastRecentlyUsed(int *tempo, int n);
int leastRecentlyUsedAproximado(int *tempo, int *bitRef, int n);
int segundaChance(int *tempo, int *bitRef, int n);
void liberaMem(PROCESSO *p, int processCont); 

int main (int argc, char* argv[]) {
	int fSize, memSize, processCont, framesQnt;
	PROCESSO *process; // vetor que irá armazenar as tabelas das páginas
	
	verifyParameters(argc, argv, &fSize, &memSize);
	
	processCont = processCounter(); // quantidade de processos que serão criados
	printf("\n>> Quantidade de processos a serem criados: %d\n\n", processCont);
	framesQnt = memSize / fSize; // quantos quadros na memória principal
	printf("\n>> Memoria: %d\tTamanho dos quadros: %d\tQuantidade de quadros: %d\n\n",memSize, fSize, framesQnt);
	process = (PROCESSO*)malloc(sizeof(PROCESSO) * processCont);
	processScale(process); // ler os tamanhos dos processos
	initTables(process, fSize, processCont); // inicializar memória principal
	
	printf("\n--------LRU-------\n");
	simuladorMemoriaVirtual(process, framesQnt, processCont, 1, fSize);
	printf("\nPressione ENTER para ver o LRU-BitRef\n");
	getchar();
	printf("\n----LRU-BitRef----\n");
	simuladorMemoriaVirtual(process, framesQnt, processCont, 2, fSize);
	printf("\nPressione ENTER para ver o 2nd-Chance\n");
	getchar();
	printf("\n----2nd-Chance----\n");
	simuladorMemoriaVirtual(process, framesQnt, processCont, 3, fSize);
	printf("\nPressione ENTER para FINALIZAR\n");
	getchar();

	liberaMem(process, processCont);
	return 0;
}
/*  Verifica parametros inseridos no console -p para o tamanho de pagina e -m para o tamanho
*   de memoria, a funcao recebe os parametros argv[] que são convertidos para int por meio da
*   funcao atoi e devolvidos a variavel global. 
*	Caso não seja, inseridos argumentos, ou os mesmos serem inválidos ou então o tamanho da
*	memória não ser múltiplo do tamanho do quadro, será atribuído um valor default de 1000 B
*	para o tamanho do quadro e 4000B para o tamanho da memória. 
*/
void verifyParameters(int argc, char **args, int *fSize, int *memSize) {
	int i;
	char *pageFrame, *memory;

	for (i = 1; i < argc; i++) {
		if (strcmp (args[i],"-p") == 0) {			
			pageFrame = args[i + 1];
			(*fSize) = atoi(pageFrame);
		}
		if (strcmp (args[i],"-m") == 0) {			
			memory= args[i + 1];
			(*memSize) = atoi(memory);
		}
	}
	if (((*fSize) <= 0) ||((*memSize) <= 0)) {
		printf("\nValores passados como argumentos sao invalidos.\nAtribuindo valores padroes para o tamanho do quadro e tamanho da memoria.\n");
		(*fSize) = 1000;
		(*memSize) = 4000;	
	} else if (((*memSize) % (*fSize)) != 0) {
		printf("\nValores passados como argumentos nao sao multiplos entre si.\nAtribuindo valores padroes para o tamanho do quadro e tamanho da memoria.\n");
		(*fSize) = 1000;
		(*memSize) = 4000;
	} else printf("\nValores de tamanho de quadro e tamanho de memoria passados com sucesso\n");
	printf("Tamanho do quadro/pagina: %dB\nTamanho da memoria: %dB\n", (*fSize), (*memSize));
}

/*	Percorre o arquivo analisando as entradas de processo que tenham o marcador 'C'
*	que indicar que o mesmo está sendo criado. Verificando tal marcador, um contador
*	é incrementado em 1. Quando o arquivo for lido completamente, a função retorna 
*	a quantidade de 'C' encontrada no arquivo e isso vai representar quantos processos
*	tem na simulação e isso vai servir de parâmetro para alocar memória para o vetor
*	que controla os quadros da memória.
*/
int processCounter(){
	int cont = 0, tamanho, posicaoMemoria;
	texto pName, pOper, tipoTamanho;
	FILE *arq;
	if (!(arq = fopen(ARQUIVO, "rt"))) {
	 	printf("\nProblemas na leitura do arquivo. Saindo\n");
	 	exit(-1);
	}
	while (!feof(arq)) {
		fscanf(arq,"%s", pName);
		fscanf(arq,"%s", pOper);
		if (strcmp (pOper,"C") == 0){
			cont++;
			fscanf(arq,"%d", &tamanho);
			fscanf(arq,"%s", tipoTamanho);
		}
		if (strcmp (pOper,"R") == 0 || strcmp (pOper,"W") == 0){
			fscanf(arq,"%x", &posicaoMemoria);		
		}
	}
	fclose(arq);
	return cont;
}

/*	A função que converte as unidades em Bytes verifica qual a unidade de
*	medida passada junto com a criação do processo. Conforme o tipo, ela
*	retorna o unitário do tamanho multiplicado por um fator correspondente
*	ao tipo de unidade 
*/
unsigned long int toByte(texto sizeUnit, int procSize) {
	if (strcmp(sizeUnit, "KB") == 0) return procSize * 1000;
	else if (strcmp(sizeUnit, "MB") == 0) return procSize * 1000000;
	else if (strcmp(sizeUnit, "GB") == 0) return procSize * 1000000000;
	else if (strcmp(sizeUnit, "TB") == 0) return procSize * 1000000000000;
	else if (strcmp(sizeUnit, "B") == 0) return procSize;
	else printf("\nTamanho não aceito. Finalizando o programa\n");
	exit(-1);
}

/*	Essa função armazena o tamanho de cada processo convertido em bytes para
*	a variável dentro da struct dos processos. Isso é feito para que depois
*	sejam alocadas as tabelas de memória baseadas no tamanho de cada processo
*	e no tamanho dos quadros/páginas.
*/
void processScale(PROCESSO *p) {
	int cont = 0, tamanho, posicaoMemoria;
	texto pName, pOper, tipoTamanho;
	FILE *arq;
	if (!(arq = fopen(ARQUIVO, "rt"))) {
	 	printf("\nProblemas na leitura do arquivo. Saindo\n");
	 	exit(-1);
	}
	while (!feof(arq)) {
		fscanf(arq,"%s", pName);
		fscanf(arq,"%s", pOper);
		if (strcmp (pOper,"C") == 0){
			fscanf(arq,"%d", &tamanho);
			fscanf(arq,"%s", tipoTamanho);
			strcpy(p[cont].id, pName);
			p[cont].procSize = toByte(tipoTamanho, tamanho);
			cont++;
		}
		if (strcmp (pOper,"R") == 0 || strcmp (pOper,"W") == 0){
			fscanf(arq,"%x", &posicaoMemoria);		
		}
	}
	fclose(arq);
}

/*	Passa por cada struct do vetor de structs que corresponde à quantidade
*	de processos que tem o arquivo de texto. A cada passada, faz a alocaçao
*	do vetor pageTable conforme a quantidade de páginas que cada processo
*	vai ter.
*/
void initTables(PROCESSO *p, int fSize, int processCont){
	int i, j;
	for (i = 0; i < processCont; i++) {
		p[i].pageQnt = p[i].procSize / fSize;
		if ((p[i].procSize % fSize) > 0) p[i].pageQnt += 1;
		p[i].pageTable = (int*)malloc(sizeof(int) * p[i].pageQnt);	
	}
}

/*	Inicializa os vetores dentro da struct MEMORIA. Esses vetores são usados
*	para fazer o controle da memória principal. A inicialização é feita
*	com o valor do framesQnt que indica quantos quadros terá a memória
*	principal.
*/
void initMemory(MEMORIA *m, int framesQnt) {
	int i;
	m->frames = (int*)malloc(sizeof(int) * framesQnt);
	m->frameProcId = (texto*)malloc(sizeof(texto) * framesQnt);
	m->time = (int*)malloc(sizeof(int) * framesQnt);
	m->timeUnchange = (int*)malloc(sizeof(int) * framesQnt);
	m->bitRef = (int*)malloc(sizeof(int) * framesQnt);
	for (i = 0; i < framesQnt; i++) {
		m->frames[i] = -1;
		strcpy(m->frameProcId[i], "");
		m->time[i] = 0;
		m->timeUnchange[i] = 0;
		m->bitRef[i] = 0;
	}
}

/* LRU - Verifica qual quadro da memoria foi utilizado menos recentemente, percorrendo o
*  numero de quadros total e retornando a posicao do quadro que foi usado menos recentemente,
*  que deve ser trocado
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

/* 
*  Simulador de Memoria Virtual
*/
int simuladorMemoriaVirtual (PROCESSO *process, int framesQnt, int processCont, int modo, int fSize)
{
	MEMORIA mainMemory; // memória principal (não inicializada ainda)
	int flag1, flag2, cont = 0, contQuadros, pos, numPage, numFrame, j, i;	
	int contador = 0, contadorSemTrocas = 0, pageFaults = 0;		
	FILE *arq;
	double percentageMemory, percentageTables; //variáveis que vao armazenar temporariamente os percentuais alocados

	int tamanho, posicaoMemoria;	 // variaveis para leitura do arquivo
	texto pName, pOper, tipoTamanho; // variáveis para leitura do arquivo

	initMemory(&mainMemory, framesQnt); // inicializar a memória principal
	if (!(arq = fopen(ARQUIVO, "rt"))) {
	 	printf("\nProblemas na leitura do arquivo. Saindo\n");
	 	exit(-1);
	}

	// Reinicializar as tabelas de páginas de cada processo
	for (i = 0; i < processCont; i++) {
		for (j = 0; j < process[i].pageQnt; j++) {
			process[i].pageTable[j] = -1;
		}
	}

	// Reinicializar as informações da memória principal
	for (i = 0; i < framesQnt; i++) {
		mainMemory.frames[i] = -1;
		strcpy(mainMemory.frameProcId[i], "");
		mainMemory.time[i] = 0;
		mainMemory.timeUnchange[i] = 0;
		mainMemory.bitRef[i] = 0;
	}
				
	while (!feof(arq)) {
		/* Inicialmente, pega a primeira pagina e percorre o vetor de quadros, verificando se a
		*  pagina ja esta carregda no vetor, caso esteja os flags se tornam 1 (valor encontrado)
		*  e o tempo e incrementado
		*/
		for (i = 0; i < processCont; i++) {
			process[i].pageAloc = 0; //Zerar contador de páginas alocadas
		}
		flag1 = flag2 = 0;		// Flags zerados = pagina nao encontrada nos quadros
		fscanf(arq, "%s", pName);
		fscanf(arq, "%s", pOper);
		if (strcmp(pOper,"C") != 0) { // processo de leitura ou gravação
			fscanf(arq, "%x", &posicaoMemoria);
			numPage = posicaoMemoria / fSize;
			cont++;	
			printf("\nmem: %d, fSize: %d\n", posicaoMemoria, fSize);
			printf("\nChamada %d\t Pagina: %d do processo: %s", cont, numPage, pName);

			//VERIFICAR SE A PÁGINA JÁ FOI REFERENCIADA
			for (i = 0; i < processCont; i++) { // procurar de qual page table é o processo
				if (strcmp(process[i].id, pName) == 0) {

					if (process[i].pageTable[numPage] != -1) {
						mainMemory.time[process[i].pageTable[numPage]] = ++contador;
						mainMemory.bitRef[process[i].pageTable[numPage]] = 1;
						flag1 = flag2 = 1;
						break;
					}
				}
			}
			for (i = 0; i < framesQnt; i++) {

			}
			/* Flag 1 - Verifica se os quadros estao vazios primeiramente, carrega a pagina solicitada
			*  no quadro, incrementa o valor do tempo e o numero de Page Faults - Como a pagina é
			*  carregada nos quadros o Flag 2 é pulado.
			*/
			if (flag1 == 0){
				for (contQuadros = 0; contQuadros < framesQnt; contQuadros++){
					if (mainMemory.frames[contQuadros] == -1) {					
						mainMemory.frames[contQuadros] = numPage;
						strcpy(mainMemory.frameProcId[contQuadros], pName);
						for (i = 0; i < processCont; i++){
							if ((strcmp(process[i].id, pName)) == 0){
								process[i].pageTable[numPage] = contQuadros;
								break;
							}
						}				
						mainMemory.timeUnchange[contQuadros] = ++contadorSemTrocas;
						mainMemory.time[contQuadros] = ++contador;						
						pageFaults++;											
						flag2 = 1;												
						break;
					}	
				}
			}

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
						pos = leastRecentlyUsed(mainMemory.time, framesQnt);
						for (i = 0; i < processCont; i++){ // Apagar a referencia na pagetable do processo cuja página será trocada
							if ((strcmp(process[i].id, mainMemory.frameProcId[pos])) == 0){
								process[i].pageTable[mainMemory.frames[pos]] = -1;
								break;
							}
						}		
						mainMemory.frames[pos] = numPage;
						strcpy(mainMemory.frameProcId[pos], pName);
						for (i = 0; i < processCont; i++){
							if ((strcmp(process[i].id, pName)) == 0){
								process[i].pageTable[numPage] = pos;
								break;
							}
						}
						mainMemory.time[pos] = ++contador;					
						pageFaults++;										
						break;	

					case 2:			// LRUAproximado				
						pos = leastRecentlyUsed_bitRef(mainMemory.time, mainMemory.bitRef, framesQnt);	
						for (i = 0; i < processCont; i++){ // Apagar a referencia na pagetable do processo cuja página será trocada
							if ((strcmp(process[i].id, mainMemory.frameProcId[pos])) == 0){
								process[i].pageTable[mainMemory.frames[pos]] = -1;
								break;
							}
						}		
						mainMemory.frames[pos] = numPage;
						strcpy(mainMemory.frameProcId[pos], pName);
						for (i = 0; i < processCont; i++){
							if ((strcmp(process[i].id, pName)) == 0){
								process[i].pageTable[numPage] = pos;
								break;
							}
						}
						mainMemory.time[pos] = ++contador;					
						pageFaults++;										
						break;

					case 3:			// SecondChance				
						pos = SecondChance(mainMemory.timeUnchange, mainMemory.bitRef, framesQnt);
						for (i = 0; i < processCont; i++){ // Apagar a referencia na pagetable do processo cuja página será trocada
							if ((strcmp(process[i].id, mainMemory.frameProcId[pos])) == 0){
								process[i].pageTable[mainMemory.frames[pos]] = -1;
								break;
							}
						}		
						mainMemory.frames[pos] = numPage;
						strcpy(mainMemory.frameProcId[pos], pName);
						for (i = 0; i < processCont; i++){
							if ((strcmp(process[i].id, pName)) == 0){
								process[i].pageTable[numPage] = pos;
								break;
							}
						}
						mainMemory.timeUnchange[pos] = ++contadorSemTrocas;					
						pageFaults++;										
						break;				
				}
			}
			printf("\n");

			for (i = 0; i < processCont; i++) {
				for (j = 0; j < process[i].pageQnt; j++) {
					if (process[i].pageTable[j] != -1) process[i].pageAloc++;
				}
			}

			if (modo == 1){			
				for (contQuadros = 0; contQuadros < framesQnt; ++contQuadros)
					printf("Quadro %d: Pagina: %d \t Processo: %s \t Tempo de Entrada: %d\n",contQuadros, mainMemory.frames[contQuadros], mainMemory.frameProcId[contQuadros], mainMemory.time[contQuadros]);
					printf("Page faults: %d\n", pageFaults);	
			} else
			if (modo == 2){
				for (contQuadros = 0; contQuadros < framesQnt; ++contQuadros)
					printf("Quadro %d: Pagina: %d \t Processo: %s \t BitReferencia: %d \t Tempo de Entrada: %d\n",contQuadros, mainMemory.frames[contQuadros], mainMemory.frameProcId[contQuadros], mainMemory.bitRef[contQuadros],mainMemory.time[contQuadros]);
					printf("Page faults: %d\n", pageFaults);
			} else
			if (modo == 3){
				for (contQuadros = 0; contQuadros < framesQnt; ++contQuadros)
					printf("Quadro %d: Pagina: %d \t Processo: %s \t BitReferencia: %d \t Tempo de Entrada: %d\n",contQuadros, mainMemory.frames[contQuadros], mainMemory.frameProcId[contQuadros], mainMemory.bitRef[contQuadros],mainMemory.timeUnchange[contQuadros]);
					printf("Page faults: %d\n", pageFaults);
			}
			for (i = 0; i < processCont; i++) {
				percentageMemory = ((double)(process[i].pageAloc * 100) / framesQnt);
				percentageTables = ((double)(process[i].pageAloc * 100) / process[i].pageQnt);
				printf("+---------------------------PROCESSO %s ------------------------------------+\n", process[i].id);
				printf("           Paginas: %d \t|\tAlocadas : %d\n", process[i].pageQnt, process[i].pageAloc);
				printf("Percentual alocado: %.4lf\t|\tPercentual da memoria total: %.4lf\n",percentageTables, percentageMemory);
				printf("+---------------------------------------------------------------------------+\n");
			}
		}else {
			fscanf(arq,"%d", &tamanho);
			fscanf(arq,"%s", tipoTamanho);
		}


	

	}
	free(mainMemory.frames);
	free(mainMemory.frameProcId);
	free(mainMemory.time);
	free(mainMemory.timeUnchange);
	free(mainMemory.bitRef);
	
	fclose(arq);
	return 1;
}

void liberaMem(PROCESSO *p, int processCont) {
	int i;
	for (i = 0; i < processCont; i++) {
		free(p[i].pageTable);
	}
	free(p);
}


