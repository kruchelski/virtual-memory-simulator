#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef char texto[4];




//TODO adaptar as funções e/ou structs para serem compatíveis.

typedef struct _PROCESSO {
	texto id;
	int *pageTable;
	unsigned long int procSize;
	int pageQnt;
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
int simuladorMemoriaVirtual(int numPaginas, int numQuadros, int modo, unsigned int *mem, int *P1, int *P2, int *P3, int P1Qnt, int P2Qnt, int P3Qnt, texto *procSeq, texto *memProc, int fSize, int physQnt, int *physicMem);
int leastRecentlyUsed(int *tempo, int n);
int leastRecentlyUsedAproximado(int *tempo, int *bitRef, int n);
int segundaChance(int *tempo, int *bitRef, int n);
void liberaMem(PROCESSO *p, int processCont); 

int main (int argc, char* argv[]) {
	int fSize = 1000, mSize = 4000, processCont;
	int *P1, *P2, *P3, *physicMem;
	unsigned int *mem;
	int P1Size = 500000;
	int P2Size = 300000;
	int P3Size = 200050;
	int P1Qnt, P2Qnt, P3Qnt, physQnt;
	texto *procSeq, *memProc;
	procSeq = (texto*)malloc(sizeof(char) * 10);
	mem = (unsigned int*)malloc(sizeof(unsigned int) * 10);
	physQnt = mSize / fSize;
	physicMem = (int*)malloc(sizeof(int) * physQnt);
	memProc = (texto*)malloc(sizeof(char) * physQnt);
	strcpy(procSeq[0], "P1");
	strcpy(procSeq[1], "P1");
	strcpy(procSeq[2], "P2");
	strcpy(procSeq[3], "P3");
	strcpy(procSeq[4], "P2");
	strcpy(procSeq[5], "P2");
	strcpy(procSeq[6], "P1");
	strcpy(procSeq[7], "P3");
	strcpy(procSeq[8], "P3");
	strcpy(procSeq[9], "P2");

	mem[0] = 0x400;
	mem[1] = 0x011;
	mem[2] = 0x001;
	mem[3] = 0x003;
	mem[4] = 1280;
	mem[5] = 0x510;
	mem[6] = 0x0f0;
	mem[7] = 0x0a1;
	mem[8] = 0x002;
	mem[9] = 0x20B;
	processCont = processCounter();
	printf("\nCONTADOR DE PROCESSOS A SEREM CRIADOS: %d\n\n", processCont);
	for (int i = 0; i < physQnt; i++) {
		physicMem[i] = -1;
	}

	P1Qnt = P1Size / fSize;
	if (P1Size % fSize > 0) P1Qnt += 1;

	P2Qnt = P2Size / fSize;
	if (P2Size % fSize > 0) P2Qnt += 1;

	P3Qnt = P3Size / fSize;
	if (P3Size % fSize > 0) P3Qnt += 1;

	P1 = (int*)malloc(sizeof(int) * P1Qnt);
	P2 = (int*)malloc(sizeof(int) * P2Qnt);
	P3 = (int*)malloc(sizeof(int) * P3Qnt);
	for (int i = 0; i < P1Qnt; i++) {
		P1[i] = -1;
	}
	for (int i = 0; i < P2Qnt; i++) {
		P2[i] = -1;
	}
	for (int i = 0; i < P3Qnt; i++) {
		P3[i] = -1;
	}
	printf("oiiii\n");
	printf("Tamanho do quadro: %d\n", fSize);
	printf("Tamanho da memoria: %d\n", mSize);
	printf("Tamanho do processo P1: %d\n", P1Size);
	printf("Quant paginas P1: %d\n", P1Qnt);
	printf("Tamanho do processo P2: %d\n", P2Size);
	printf("Quant paginas P2: %d\n", P2Qnt);
	printf("Tamanho do processo P3: %d\n", P3Size);
	printf("Quant paginas P3: %d\n", P3Qnt);

	printf("SEQUENCIA DE PROCESSOS\n");
	for (int i = 0; i < 10; i++) {
		printf("%d: %s\tEndereco: %X | %d \n", i, procSeq[i], mem[i], mem[i]);
	}
	printf("TABELA P1\n");
	for (int i = 0; i < P1Qnt; i++) {
		printf("Pagina %d: %d\n", i, P1[i]);
	}
	printf("TABELA P2\n");
	for (int i = 0; i < P2Qnt; i++) {
		printf("Pagina %d: %d\n", i, P2[i]);
	}
	printf("TABELA P3\n");
	for (int i = 0; i < P3Qnt; i++) {
		printf("Pagina %d: %d\n", i, P3[i]);
	}

	printf("\n\n----------------\n\n");
	
	printf("\n\nLRU\n\n----------------------------\n");
	simuladorMemoriaVirtual (500, 4, 1, mem, P1, P2, P3, P1Qnt, P2Qnt, P3Qnt, procSeq, memProc, fSize, physQnt, physicMem);
	
	
	printf("\n\nLRU_BITREF\n\n----------------------------\n");
	simuladorMemoriaVirtual (500, 4, 2, mem, P1, P2, P3, P1Qnt, P2Qnt, P3Qnt, procSeq, memProc, fSize, physQnt, physicMem);
	
	printf("\n\n2nd_CHANCE\n\n----------------------------\n");
	simuladorMemoriaVirtual (500, 4, 3, mem, P1, P2, P3, P1Qnt, P2Qnt, P3Qnt, procSeq, memProc, fSize, physQnt, physicMem);

	//int simuladorMemoriaVirtual (int numPaginas, int numQuadros, int modo, int *mem, int *P1, int *P2, int *P3, int P1Qnt, int P2Qnt, int P3Qnt, texto *procSeq, texto *memProc, int fSize, int physQnt, int *physicMem)





	free(procSeq);
	free(mem);
	free(physicMem);
	free(memProc);
	free(P1);
	free(P2);
	free(P3);

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
	} else if (((*memSize) % (*fSize)) == 0) {
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
	if (!(arq = fopen("process.txt", "rt"))) {
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
	if (!(arq = fopen("process.txt", "rt"))) {
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
		p[i].pageTable = (int *)malloc(sizeof(int) * p[i].pageQnt);
		for (j = 0; j < p[i].pageQnt; j++) {
			p[i].pageTable[j] = -1;
		}
	}
}



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

int simuladorMemoriaVirtual (int numPaginas, int numQuadros, int modo, unsigned int *mem, int *P1, int *P2, int *P3, int P1Qnt, int P2Qnt, int P3Qnt, texto *procSeq, texto *memProc, int fSize, int physQnt, int *physicMem)
{
	int flag1, flag2, cont, contQuadros, pos, numPage, numFrame, contProcess;						// Tempo 			- Tempo corrido a cada etapa
	int contador = 0, contadorSemTrocas = 0, pageFaults = 0;		// Tempo Sem Troca  - Nao adiciona tempo se a pagina ja estiver carregada p/ Second Chance
	int quadros[4], tempo[4], tempoSemTrocas[4], bitRef[4];


	for (int i = 0; i < physQnt; i++) {
		physicMem[i] = -1;
		strcpy(memProc[i], "");
	}

	for (int i = 0; i < P1Qnt; i++) {
		P1[i] = -1;
	}
	for (int i = 0; i < P2Qnt; i++) {
		P2[i] = -1;
	}
	for (int i = 0; i < P3Qnt; i++) {
		P3[i] = -1;
	}




	for (cont = 0; cont < physQnt; cont++)					// Zerar vetores
	{
		//physicMem[cont] = -1;

		bitRef[cont] = 0; 				
		tempo[cont] = 0;		
		tempoSemTrocas[cont] = 0;
	}		
		
	
	for (cont = 0; cont < 10; cont++)		
	{
		flag1 = flag2 = 0;		// Flags zerados = pagina nao encontrada nos quadros
		printf("\nmem: %d, fSize: %d\n", mem[cont], fSize);
		numPage = mem[cont] / fSize;
		printf("\nChamada %d\t Pagina: %d do processo: %s", cont, numPage, procSeq[cont]);


		/* Inicialmente, pega a primeira pagina e percorre o vetor de quadros, verificando se a
		*  pagina ja esta carregda no vetor, caso esteja os flags se tornam 1 (valor encontrado)
		*  e o tempo e incrementado
		*/


		if ((strcmp("P1", procSeq[cont])) == 0){
			for (contQuadros = 0; contQuadros < P1Qnt; contQuadros++) {
				if (P1[numPage] != -1) {
					printf("Blinpa P1\n");								
					tempo[P1[numPage]] = ++contador;				
					bitRef[P1[numPage]] = 1;	
					flag1 = flag2 = 1;	
					break;
				}
			}
		} else
		if ((strcmp("P2", procSeq[cont])) == 0){
			for (contQuadros = 0; contQuadros < P2Qnt; contQuadros++) {
				if (P2[numPage] != -1) {	
					printf("Blimpa P2\n");							
					tempo[P2[numPage]] = ++contador;				
					bitRef[P2[numPage]] = 1;	
					flag1 = flag2 = 1;	
					break;
				}
			}
		} else
		if ((strcmp("P3", procSeq[cont])) == 0){
			for (contQuadros = 0; contQuadros < P1Qnt; contQuadros++) {
				if (P3[numPage] != -1) {	
					printf("Blimpa P3\n");							
					tempo[P3[numPage]] = ++contador;				
					bitRef[P3[numPage]] = 1;	
					flag1 = flag2 = 1;	
					break;
				}
			}
		}
		


		//for (contQuadros = 0; contQuadros < physQnt; contQuadros++)		
		/*	if (physicMem[contQuadros] == numPage)						
			{								
				tempo[contQuadros] = ++contador;				
				bitRef[contQuadros] = 1;	
				flag1 = flag2 = 1;	
				break;
			}	*/		

		/* Flag 1 - Verifica se os quadros estao vazios primeiramente, carrega a pagina solicitada
		*  no quadro, incrementa o valor do tempo e o numero de Page Faults - Como a pagina é
		*  carregada nos quadros o Flag 2 é pulado.
		*/

		if (flag1 == 0)	
			for (contQuadros = 0; contQuadros < physQnt; contQuadros++){													
			
				if (physicMem[contQuadros] == -1)								
				{	
					printf("\nCATOMBA");				
					physicMem[contQuadros] = numPage;
					strcpy(memProc[contQuadros], procSeq[cont]);
					if ((strcmp("P1", procSeq[cont])) == 0) P1[numPage] = contQuadros;
					else if ((strcmp("P2", procSeq[cont])) == 0) P2[numPage] = contQuadros;
					else if ((strcmp("P3", procSeq[cont])) == 0) P3[numPage] = contQuadros;					
					tempoSemTrocas[contQuadros] = ++contadorSemTrocas;
					tempo[contQuadros] = ++contador;						
					pageFaults++;											
					flag2 = 1;												
					break;
				}	
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
					pos = leastRecentlyUsed(tempo, physQnt);		
					physicMem[pos] = numPage;
					strcpy(memProc[pos], procSeq[cont]);
					if ((strcmp("P1", procSeq[cont])) == 0) P1[numPage] = pos;
					else if ((strcmp("P2", procSeq[cont])) == 0) P2[numPage] = pos;
					else if ((strcmp("P3", procSeq[cont])) == 0) P3[numPage] = pos;							
					tempo[pos] = ++contador;							
					pageFaults++;										
					break;				
				case 2:			// LRUAproximado				
					pos = leastRecentlyUsed_bitRef(tempo, bitRef, physQnt);	
					physicMem[pos] = numPage;
					strcpy(memProc[pos], procSeq[cont]);
					if ((strcmp("P1", procSeq[cont])) == 0) P1[numPage] = pos;
					else if ((strcmp("P2", procSeq[cont])) == 0) P2[numPage] = pos;
					else if ((strcmp("P3", procSeq[cont])) == 0) P3[numPage] = pos;								
					tempo[pos] = ++contador;									
					pageFaults++;												
					break;				
				case 3:			// SecondChance				
					pos = SecondChance(tempoSemTrocas, bitRef, physQnt);	
					physicMem[pos] = numPage;
					strcpy(memProc[pos], procSeq[cont]);	
					if ((strcmp("P1", procSeq[cont])) == 0) P1[numPage] = pos;
					else if ((strcmp("P2", procSeq[cont])) == 0) P2[numPage] = pos;
					else if ((strcmp("P3", procSeq[cont])) == 0) P3[numPage] = pos;						
					tempoSemTrocas[pos] = ++contadorSemTrocas;												
					pageFaults++;																
					break;				
			}
		
		}	




		printf("\n");

		if (modo == 1){			
			for (contQuadros = 0; contQuadros < physQnt; ++contQuadros)
				printf("Quadro %d: %d \t  Tempo de Entrada: %d \t Processo: %s \n",contQuadros, physicMem[contQuadros],tempo[contQuadros], memProc[contQuadros]);
				printf("PAGE FAULTS: %d\n", pageFaults);		
		}
		if (modo == 2){
			for (contQuadros = 0; contQuadros < physQnt; ++contQuadros)
				printf("Quadro %d: %d \t BitReferencia: %d \t Tempo de Entrada: %d \t Processo: %s \n",contQuadros, physicMem[contQuadros],bitRef[contQuadros],tempo[contQuadros], memProc[contQuadros]);
				printf("PAGE FAULTS: %d\n", pageFaults);
		}
		if (modo == 3){
			for (contQuadros = 0; contQuadros < physQnt; ++contQuadros)
				printf("Quadro %d: %d \t BitReferencia: %d \t Tempo de Entrada: %d \t Processo: %s \n",contQuadros, physicMem[contQuadros],bitRef[contQuadros],tempoSemTrocas[contQuadros], memProc[contQuadros]);
				printf("PAGE FAULTS: %d\n", pageFaults);
		}

	}

	printf("Numero de Page Faults: %d\n", pageFaults );		// Mostra o total de Page Faults

	return 1;
}

void liberaMem(PROCESSO *p, int processCont) {
	int i;
	for (i = 0; i < processCont; i++) {
		free(p[i].pageTable);
	}
	free(p);
}


