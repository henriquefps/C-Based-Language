#include <stdio.h>
#include <stdlib.h>

//COMBINADORES
#define ARROBA -64
#define K -75
#define S -83
#define I -73
#define B -66
#define C -67
#define D -68
#define E -69
#define F -70
#define Y -89
#define SOMA -43
#define SUBTRACAO -45
#define MULTIPLICACAO -42
#define DIVISAO -47
#define TRUE -15
#define FALSE -16
#define MENORQUE -60
#define MAIORQUE -62
#define IGUALDADE -61
#define ROOT -100 //Usado como Root do grafo, representa o espaco vazio ' '

//GARBAGE COLLECTION
#define CELULA_LIVRE   '0'
#define CELULA_OCUPADA '1'

//TAMANHO DA STRING A SER REDUZIDA
#define N 10000000

//TAMANHO DA HEAP
#define H 1000000

typedef struct reg {
    int tipo;
    struct reg *dir;
    struct reg *esq;
} noh;
typedef noh* node;

static node tokens[128];//Otimização que reutiliza combinadores

static noh heap1[H/2];      //Ponteiro para a HEAP1
static noh heap2[H/2];      //Ponteiro para a HEAP2
static noh* rootGrafo;      //Ponteiro a cabeça do grafo
static int heapPointer = 0; //Variavel que indica a proxima celula livre de heap atual
static int copyPointer = 0; //Variavel que indica a celula que está sendo copiada atualmente
static int currentHeap = 1; //Indica qual heap deve ser usada atualmente. (1) -> heap1, (2) -> heap2
static int callsGC = 0;     //Conta a quantidade de chamadas ao garbage collector

//Retorna um node da heap em uso atual
//recebe o tipo de dado a ser armazenado no
//campo tipo do node
node alocarNode(int tipo) {
    if(heapPointer >= H/2) {
        printf("\nErro: HeapPointer maior que Heap/2\n");
        exit(0);
    }
    node celulaAlocada = (currentHeap == 1) ? &heap1[heapPointer++] : &heap2[heapPointer++];
    celulaAlocada->tipo = tipo;
    celulaAlocada->esq = celulaAlocada->dir = NULL;
    return celulaAlocada;
}


//Aloca os tokens que sao utilizados
//na construcao e reducao do grafo
//So eh necessaria uma unidade de cada,
//pois eles sao reutilizados, reduzindo
//o gasto de memoria da heap
void alocarTokens() {
    int i;
    int constante[18] = {K, S, I, B, C, D, E, F, Y, SOMA, SUBTRACAO, MULTIPLICACAO,
                         DIVISAO, TRUE, FALSE, MENORQUE, MAIORQUE, IGUALDADE};
    for(i = 0 ; i < 18 ; i++) {
        tokens[-1 * constante[i]] = alocarNode( constante[i] );
    }
}

void copiaTokens() {
    int i;
    int constante[18] = {K, S, I, B, C, D, E, F, Y, SOMA, SUBTRACAO, MULTIPLICACAO,
                         DIVISAO, TRUE, FALSE, MENORQUE, MAIORQUE, IGUALDADE};
    for(i = 0 ; i < 18 ; i++) {
        int index = heapPointer++;
        node nodeCopia = (currentHeap == 1) ? &heap1[index] : &heap2[index];

        //Copiando conteudo
        nodeCopia->tipo = tokens[-1 * constante[i]]->tipo;
        nodeCopia->esq = tokens[-1 * constante[i]]->esq;
        nodeCopia->dir = tokens[-1 * constante[i]]->dir;

        //Criando foward pointer
        tokens[-1 * constante[i]]->esq = nodeCopia;
        tokens[-1 * constante[i]]->dir = NULL;

        //Atribuindo nova celula no vetor
        tokens[-1 * constante[i]] = nodeCopia;
    }
}

//Copia um node e transforma a celula original num forward pointer
node copiarNode(node celula) {
    node celulaCopia = alocarNode(celula->tipo);
    celulaCopia->esq = celula->esq;
    celulaCopia->dir = celula->dir;
    celula->esq = celulaCopia;
    celula->dir = NULL;
    return celulaCopia;
}

//Recebe um grafo e o copia para a heap vazia
node copiarGrafo(node grafo) {
    copyPointer = heapPointer;

    //Copiando a raiz do grafo
    copiarNode(grafo);

    //Copiando demais celulas em LARGURA
    while(copyPointer < heapPointer) {
        if(copyPointer > (H/2)) {
            printf("Erro: Memoria cheia");
            exit(0);
        }

        //nodeCopia armazena a celula que copyPointer referencia
        node nodeCopia = (currentHeap == 1) ? &heap1[copyPointer] : &heap2[copyPointer];
        node nodeEsq = nodeCopia->esq;
        node nodeDir = nodeCopia->dir;

        if(nodeEsq != NULL) {
            if (nodeEsq->dir == NULL && nodeEsq->esq != NULL) {
                nodeCopia->esq = nodeEsq->esq;
            } else {
                nodeCopia->esq = copiarNode(nodeEsq);
            }
        }

        if(nodeDir != NULL) {
            if (nodeDir->dir == NULL && nodeDir->esq != NULL) {
                nodeCopia->dir = nodeDir->esq;
            } else {
                nodeCopia->dir = copiarNode(nodeDir);
            }
        }

        copyPointer++;
    }

    //O index 18 armazena a proxima posicao livre apos os tokens
    //Portanto, sera garantidamente a raiz do grafo copiado
    return (currentHeap == 1) ? &heap1[18] : &heap2[18];
}


void cheney() {
    currentHeap = (currentHeap == 1) ? 2 : 1;
    heapPointer = 0;
    copiaTokens();
    node grafo = copiarGrafo(rootGrafo->esq);
    rootGrafo = alocarNode(ROOT);
    rootGrafo->esq = grafo;
}

//só pode ser usada na geração do grafo
node atribuirToken(char tipo) {
    if(tipo < '0' || tipo > '9')
        return tokens[tipo];
    else
        return alocarNode(tipo-'0');
}

void printGrafoInfixo(node no){
    static int deph = 0;
    static int lista = 0;
    if(deph == 0 && no->tipo == -25) lista = 1;
    if(lista && no->tipo == -25) printf("[");
    if(no->esq) {
        deph++;
        printGrafoInfixo(no->esq);
    }
    switch (no->tipo){
        case -1: break;
        case ARROBA: printf("("); break;
        case S: printf("S"); break;
        case K: printf("K"); break;
        case I: printf("I"); break;
        case B: printf("B"); break;
        case C: printf("C"); break;
        case D: printf("D"); break;
        case E: printf("E"); break;
        case F: printf("F"); break;
        case TRUE: printf("TRUE"); break;
        case FALSE: printf("FALSE"); break;
        case MAIORQUE: printf(">"); break;
        case MENORQUE: printf("<"); break;
        case IGUALDADE: printf("=="); break;
        case SOMA: printf("+"); break;
        case SUBTRACAO: printf("-"); break;
        case MULTIPLICACAO: printf("*"); break;
        case DIVISAO: printf("/"); break;
        case Y: printf("Y"); break;
        default: printf("%u", no->tipo); break;
    }
    if(no->dir) {
        deph++;
        if(no->dir != no) {
            printGrafoInfixo(no->dir);
            if (lista)
                printf("]");
            else
                printf(")");
        }else{
            printf("Y(");
            printGrafoInfixo(no->esq);
            printf(")");
            return;
        }
    }
    if(deph == 0) lista = 0;
}

//Exibe o grafo no console, escrito em preordem
void printGrafoPosfixo(node r) {
    if (r != NULL) {
        int numero = 0;
        char token;
        switch (r->tipo) {
            case ARROBA: token = '@'; break;
            case K: token = 'K'; break;
            case S: token = 'S'; break;
            case I: token = 'I'; break;
            case B: token = 'B'; break;
            case C: token = 'C'; break;
            case D: token = 'D'; break;
            case E: token = 'E'; break;
            case F: token = 'F'; break;
            case Y: token = 'Y'; break;
            case SOMA: token = '+'; break;
            case SUBTRACAO: token = '-'; break;
            case MULTIPLICACAO: token = '*'; break;
            case DIVISAO: token = '/'; break;
            case MENORQUE: token = '<'; break;
            case MAIORQUE: token = '>'; break;
            case IGUALDADE: token = '='; break;
            case ROOT: token = 'R'; break;
            default: numero = 1; break;
        }
        if(numero == 1) printf ("(%i)", r->tipo);
        else printf ("%c", token);
        printGrafoPosfixo(r->esq);
        printGrafoPosfixo(r->dir);
    }
}

//Procedimento que recebe duas variaveis, uma que aponta para
//um array e outra que aponta para um inteiro com a posicao do
//array a ser usada. O procedimento termina quando atinge a
//primeira posicao depois dos parenteses.
void casaParenteses(char* string, int* p) {
    int paren = 1;
    int c = *p;
    while (paren != 0) {
        char x = string[c];
        if(x == '(') {
            paren++;
            c++;
        }
        else if(x == ')') {
            paren--;
            c++;
        }
        else {
            c++;
        }
    }
    *p = c;
}

//Recebe uma string e um inteiro que armazena
//a posicao do ( parenteses a ser deletado.
//Reescreve a string, sem o abre parenteses
//do inteiro e seu fecha parenteses correspondente.
void removeParenteses(char* string, int index) {
    int i = ++index;
    casaParenteses(string, &index);
    for(; i < index-1 ; i++) {
        string[i-1] = string[i];
    }
    for(i = index ; string[i] != '\0' ; i++) {
        string[i-2] = string[i];
    }
    string[i-2] = '\0';
}

//Procedimento que remove redundancias (parenteses
//duplos) da string a ser convertida em grafo
void formatarString(char* string) {
    int i = 0, redundancia = 0;
    while(string[i] != '\0') {
        if(string[i] == '(' && string[i+1] == '(') redundancia = 1;
        if(redundancia) {
            removeParenteses(string, i+1);
            redundancia=0;
            i--;
        }
        i++;
    }
}

//Recebe um valor inteiro, que será adicionado
//ao grafo (rootGRafo) como parametro
void adicionarParametro(int param) {
    node arroba = alocarNode(ARROBA);
    arroba->dir = alocarNode(param);;
    arroba->esq = rootGrafo->esq;
    rootGrafo->esq = arroba;
}


//Recebe a String a ser convertida, o index de inicio
//da string, e o index de onde a string termina.
//Converte esta string para grafo e
//retorna o node raiz do grafo gerado
node gerarGrafoAux(char* string, int indexAtual, int indexFinal) {
    node root = alocarNode(ROOT);
    root->dir = NULL;

    // Folha a esquerda
    node token = atribuirToken(string[indexAtual++]);
    root->esq = token;

    // Folhas a direita
    while(indexAtual < indexFinal) {
        if(string[indexAtual] != '(') {
            node token = atribuirToken(string[indexAtual++]);
            node arroba = alocarNode(ARROBA);
            arroba->dir = token;
            arroba->esq = root->esq;
            root->esq = arroba;
        }
        else {
            indexAtual++;
            int inicio = indexAtual;
            casaParenteses(string, &indexAtual);
            int fim = indexAtual-1;
            node subgrafo = gerarGrafoAux(string, inicio, fim);
            node arroba = alocarNode(ARROBA);
            arroba->dir = subgrafo->esq;
            arroba->esq = root->esq;
            root->esq = arroba;
        }
    }
    return root;
}

//Recebe a string a ser convertida em grafo
//e retorna o node raiz do grafo gerado
node gerarGrafo(char* string) {
    alocarTokens();
    int lenght = 0;
    while(string[lenght] != '\0')
        lenght++;
    node grafo = gerarGrafoAux(string, 0, lenght);
    return grafo;
}

//Cabecalho para permitir que os combinadores chamem a funcao reduzirGrafo
node reduzirGrafo(int chamadaRecursiva);

int tamanhoGrafo(node grafo) {
    int size = -1; //desconsiderando o nó raiz
    node aux = grafo;
    while(aux != NULL) {
        size++;
        aux = aux->esq;
    }
    return size;
}

int buscaCombinador(node grafo) {
    node aux = grafo;
    while(aux->esq != NULL)
        aux = aux->esq;
    return aux->tipo;
}


//Retorna um ponteiro, cujos elementos abaixo dele
//são todos os argumentos da respectiva redução
//Recebe o grafo atual e a quantidade de elementos
//desta redução
node posicionarPonteiro(node grafo, int numArgs) {
    int size = tamanhoGrafo(grafo);
    node aux = grafo;
    while(size > numArgs+1) {
        aux = aux->esq;
        size--;
    }
    return aux;
}

//K a b = a
void reduzK(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);
    aux->esq = aux->esq->esq->dir; //argumento A
}

//S a b c = a c ( b c )
void reduzS(node grafo) {
    node aux = posicionarPonteiro(grafo, 3);
    node arroba1 = alocarNode(ARROBA);
    node arroba2 = alocarNode(ARROBA);
    node arroba3 = alocarNode(ARROBA);

    arroba1->esq = arroba2;
    arroba1->dir = arroba3;

    arroba2->esq = aux->esq->esq->esq->dir; //argumento A
    arroba2->dir = aux->esq->dir; //argumento C

    arroba3->esq = aux->esq->esq->dir; //argumento B
    arroba3->dir = aux->esq->dir; //argumento C

    aux->esq = arroba1;
}

//I a = a
void reduzI(node grafo) {
    node aux = posicionarPonteiro(grafo, 1);
    aux->esq = aux->esq->dir; //argumento A
}

//B a b c = a ( b c )
void reduzB(node grafo) {
    node aux = posicionarPonteiro(grafo, 3);
    node arroba1 = alocarNode(ARROBA);
    node arroba2 = alocarNode(ARROBA);

    arroba1->dir = arroba2;

    arroba1->esq = aux->esq->esq->esq->dir; //argumento A
    arroba2->esq = aux->esq->esq->dir; //argumento B
    arroba2->dir = aux->esq->dir; //argumenro C

    aux->esq = arroba1;
}

//C a b c = a c b
void reduzC(node grafo) {
    node aux = posicionarPonteiro(grafo, 3);
    node arroba1 = alocarNode(ARROBA);
    node arroba2 = alocarNode(ARROBA);

    arroba1->esq = arroba2;

    arroba1->dir = aux->esq->esq->dir; //argumento B
    arroba2->dir = aux->esq->dir; //argumento C
    arroba2->esq = aux->esq->esq->esq->dir; //argumento A

    aux->esq = arroba1;
}

//D a b c d = a ( b d ) ( c d )
void reduzD(node grafo) {
    node aux = posicionarPonteiro(grafo, 4);
    node arroba1 = alocarNode(ARROBA);
    node arroba2 = alocarNode(ARROBA);
    node arroba3 = alocarNode(ARROBA);
    node arroba4 = alocarNode(ARROBA);

    arroba1->esq = arroba2;
    arroba1->dir = arroba3;

    arroba2->dir = arroba4;
    arroba2->esq = aux->esq->esq->esq->esq->dir; //argumento A

    arroba4->esq = aux->esq->esq->esq->dir; //argumento B
    arroba4->dir = aux->esq->dir; //argumento D

    arroba3->esq = aux->esq->esq->dir; //argumento C
    arroba3->dir = aux->esq->dir; //argumento D

    aux->esq = arroba1;
}

//E a b c d = a b ( c d )
void reduzE(node grafo) {
    node aux = posicionarPonteiro(grafo, 4);
    node arroba1 = alocarNode(ARROBA);
    node arroba2 = alocarNode(ARROBA);
    node arroba3 = alocarNode(ARROBA);

    arroba1->esq = arroba2;
    arroba1->dir = arroba3;

    arroba2->esq = aux->esq->esq->esq->esq->dir; //argumento A
    arroba2->dir = aux->esq->esq->esq->dir; //argumento B

    arroba3->esq = aux->esq->esq->dir; //argumento C
    arroba3->dir = aux->esq->dir; //argumento D

    aux->esq = arroba1;
}

//F a b c d = a ( b d ) c
void reduzF(node grafo) {
    node aux = posicionarPonteiro(grafo, 4);
    node arroba1 = alocarNode(ARROBA);
    node arroba2 = alocarNode(ARROBA);
    node arroba3 = alocarNode(ARROBA);

    arroba1->esq = arroba2;
    arroba1->dir = aux->esq->esq->dir; //argumento C

    arroba2->dir = arroba3;
    arroba2->esq = aux->esq->esq->esq->esq->dir; //argumento A

    arroba3->esq = aux->esq->esq->esq->dir; //argumento B
    arroba3->dir = aux->esq->dir; //argumento D

    aux->esq = arroba1;
}

//Y a = a ( Y a )
void reduzY(node grafo) {
    node aux = posicionarPonteiro(grafo, 1);
    node arroba1 = alocarNode(ARROBA);
    node arroba2 = alocarNode(ARROBA);

    node argumentoA = aux->esq->dir;

    arroba1->dir = arroba2;
    arroba1->esq = argumentoA;

    arroba2->dir = argumentoA;
    arroba2->esq = atribuirToken('Y');

    aux->esq = arroba1;
}

// Outra forma de reduzir o combinador Y
void reduzY2(node grafo) {
    node aux = posicionarPonteiro(grafo, 1);
    node arroba1 = alocarNode(ARROBA);
    arroba1->esq = aux->esq->dir;
    arroba1->dir = arroba1;

    aux->esq = arroba1;
}

/*##########  OPERADORES ARITIMÉTICOS E LÓGICOS #########*/

node avaliarExpressao(node argumento);

//Recebe o ponteiro do grafo, já posiciona acima dos
//argumentos da redução e dois ponteiros para atribuir
//os valores dos argumentos (ja avaliados)
//Retorna 1, caso a atribuição ocorra com sucesso
//Retorna 0, caso ocorra uma chamada ao garbage collection
int atribuirArgumentos(node aux, int* argA, int* argB) {
    node argumentoB = aux->esq->dir;
    if(argumentoB->tipo == ARROBA) {
        argumentoB = avaliarExpressao(argumentoB);
        aux->esq->dir = argumentoB;
    }

    node argumentoA = aux->esq->esq->dir;
    if(argumentoA->tipo == ARROBA) {
        argumentoA = avaliarExpressao(argumentoA);
        aux->esq->esq->dir = argumentoA;
    }

    //Caso verdade, significa que a freelist esta quase vazia
    if(argumentoB->tipo == ARROBA || argumentoA->tipo == ARROBA) {
        return 0;
    }

    //Significa que as avaliações ocorreram com sucesso
    aux->esq->dir = argumentoB;
    aux->esq->esq->dir = argumentoA;

    *argA = argumentoA->tipo;
    *argB = argumentoB->tipo;
    return 1;
}

//SOMA a b = eval(a) + eval(b)
void reduzSoma(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);

    int a, b;
    int chamadaRecursiva = atribuirArgumentos(aux, &a, &b);
    if(chamadaRecursiva == 0) return;

    node resultado = alocarNode(a+b);

    aux->esq = resultado;
}

//SUBTRACAO a b = eval(a) - eval(b)
void reduzSubtracao(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);

    int a, b;
    int chamadaRecursiva = atribuirArgumentos(aux, &a, &b);
    if(chamadaRecursiva == 0) return;

    node resultado = alocarNode(a-b);

    aux->esq = resultado;
}

//MULTIPLICACAO a b = eval(a) * eval(b)
void reduzMultiplicacao(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);

    int a, b;
    int chamadaRecursiva = atribuirArgumentos(aux, &a, &b);
    if(chamadaRecursiva == 0) return;

    node resultado = alocarNode(a*b);

    aux->esq = resultado;
}

//DIVISAO a b = eval(a) / eval(b)
void reduzDivisao(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);

    int a, b;
    int chamadaRecursiva = atribuirArgumentos(aux, &a, &b);
    if(chamadaRecursiva == 0)  return;

    if(b == 0) {
        printf("\n# Divisao por zero #\n");
        exit(0);
    }

    node resultado = alocarNode(a/b);

    aux->esq = resultado;
}

//TRUE a b = a
void reduzTrue(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);
    node argumentoA = aux->esq->esq->dir;

    aux->esq = argumentoA;
}

//FALSE a b = B
void reduzFalse(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);
    node argumentoB = aux->esq->dir;

    aux->esq = argumentoB;
}

//MENORQUE a b = (a < b)? TRUE : FALSE
void reduzMenorQue(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);

    int a, b;
    int chamadaRecursiva = atribuirArgumentos(aux, &a, &b);
    if(chamadaRecursiva == 0) return;

    if(a < b) aux->esq = tokens[-1*TRUE];
    else aux->esq = tokens[-1*FALSE];
}

//MAIORQUE a b = (a > b)? TRUE : FALSE
void reduzMaiorQue(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);

    int a, b;
    int chamadaRecursiva = atribuirArgumentos(aux, &a, &b);
    if(chamadaRecursiva == 0) return;

    if(a > b) aux->esq = tokens[-1*TRUE];
    else aux->esq = tokens[-1*FALSE];
}

//IGUALGADE a b = (a == b)? TRUE : FALSE
void reduzIgualdade(node grafo) {
    node aux = posicionarPonteiro(grafo, 2);
    node argumentoB = aux->esq->dir;

    int a, b;
    int chamadaRecursiva = atribuirArgumentos(aux, &a, &b);
    if(chamadaRecursiva == 0) return;

    if(a == b) aux->esq = tokens[-1*TRUE];
    else aux->esq = tokens[-1*FALSE];
}

node avaliarExpressao(node argumento) {
    node subgrafo = alocarNode(ROOT);
    //Avaliando Expressao
    subgrafo->esq = argumento;
    node aux = rootGrafo;
    rootGrafo = subgrafo;
    reduzirGrafo(1);
    node res = rootGrafo->esq;
    rootGrafo = aux;
    return res;
}



//Este procedimento recebe um grafo
//e retorna sua forma irredutível
node reduzirGrafo(int chamadaRecursiva) {
    float porcentagem = 5;
    int euristica = (H/2) * (porcentagem/100);

    int memoriaLivreLastCall = 0;

    while(rootGrafo->esq->tipo == ARROBA) {
        int combinador = buscaCombinador(rootGrafo);
        int memoriaLivre = (H/2)-heapPointer;
        if( memoriaLivre < euristica ) {
            if(chamadaRecursiva) {
                break;
            } else {
                callsGC++;
                cheney();
            }
        }

        switch(combinador) {
            case K: reduzK(rootGrafo); break;
            case S: reduzS(rootGrafo); break;
            case I: reduzI(rootGrafo); break;
            case B: reduzB(rootGrafo); break;
            case C: reduzC(rootGrafo); break;
            case D: reduzD(rootGrafo); break;
            case E: reduzE(rootGrafo); break;
            case F: reduzF(rootGrafo); break;
            case Y: reduzY2(rootGrafo); break;
            case SOMA: reduzSoma(rootGrafo); break;
            case SUBTRACAO: reduzSubtracao(rootGrafo); break;
            case MULTIPLICACAO: reduzMultiplicacao(rootGrafo); break;
            case DIVISAO: reduzDivisao(rootGrafo); break;
            case MENORQUE: reduzMenorQue(rootGrafo); break;
            case MAIORQUE: reduzMaiorQue(rootGrafo); break;
            case IGUALDADE: reduzIgualdade(rootGrafo); break;
            case TRUE: reduzTrue(rootGrafo); break;
            case FALSE: reduzFalse(rootGrafo); break;
            default: break;
        }
    }
    return rootGrafo->esq;
}