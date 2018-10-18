/*
Corretor Ortografico
Utiliza o algoritmo de Levenshtein Distance para defirnir a proximidade da palavra correta
Utiliza um heap binário para organizar as melhores soluções
Autor: Leonardo Pedrozo
*/

#include <iostream>
#include <fstream>
#include <time.h>
#include <locale.h>

#define MAX_LEN 40
#define QUANT_SUG 5

using namespace std;

/* Representa uma sugestão de solução do problema */
typedef struct {
	string palavra;
	int distancia;
} Solucao;

/* Tabela utilizada pelo Levenshtein Distance */
int tabela[MAX_LEN][MAX_LEN];

/* Devolve o menor entre 3 valores */
int minimo(int a, int b, int c) {

	if(a > b)
		a = b;

	if(a > c)
		a = c;

	return a;
}

/* Levenshtein Distance */
int similarity(string a, string b) {

	int len_a = a.length();
	int len_b = b.length();

	for(int i = 1; i <= len_a; i++) {
		for(int j = 1; j <= len_b; j++) {

			int d;

			if(a[i - 1] == b[j - 1])
				d = 0;
			else
				d = 1;

			tabela[i][j] = minimo(tabela[i - 1][j] + 1, tabela[i][j - 1] + 1, tabela[i - 1][j - 1] + d);
		}
	}

	return tabela[len_a][len_b];
}


/* Metodos do HEAP */
int parent(int i) {
	return (i - 1) / 2;
}

int left(int i) {
	return i * 2 + 1;
}

int right(int i) {
	return i * 2 + 2;
}

void heap_insert(Solucao heap[], int *size, Solucao s) {

	int pos = *size;
	heap[pos] = s;
	(*size)++;

	while(pos) {

		if(heap[pos].distancia > heap[parent(pos)].distancia) {
			Solucao aux = heap[pos];
			heap[pos] = heap[parent(pos)];
			heap[parent(pos)] = aux;
		} else {
			break;
		}

		pos = parent(pos);
	}
}

void heap_remove(Solucao heap[], int *size) {

	int pos = 0;

	heap[0] = heap[(*size) - 1];

	(*size)--;

	/* heapfy */
	while(left(pos) < *size) {

		int max;

		if(right(pos) < *size)
			max = heap[left(pos)].distancia > heap[right(pos)].distancia ? left(pos) : right(pos);
		else
			max = left(pos);

		if(heap[pos].distancia < heap[max].distancia) {
			Solucao aux = heap[pos];
			heap[pos] = heap[max];
			heap[max] = aux;
		} else {
			break;
		}

		pos = max;
	}
}

bool matches(string palavra, Solucao melhores[], int *size, ifstream &entrada) {

	string str;

	/* Dessa forma ele vai ler todas as palavras do arquivo até o End Of File */
	while(entrada >> str) {

		/* O quanto similar a palavra atual é da palavra que esta sendo procurada (quanto maior o valor, menos similar é) */
		int s = similarity(palavra, str);

		/* Similaridade 0 quer dizer que as palavras são idênticas */
		if(s == 0)
			return true;

		/* melhores[0] = raiz do heap */
		if(*size == 0 || s < melhores[0].distancia) {

			Solucao nova;
			nova.distancia = s;
			nova.palavra = str;

			if(*size == QUANT_SUG)
				heap_remove(melhores, size);

			heap_insert(melhores, size, nova);
		}
	}

	return false;
}

void toLowerCase(string &str) {

	int len = str.length();

	for(int i = 0; i < len; i++)
		if(str[i] >= 'A' && str[i] <= 'Z')
			str[i] = str[i] - 'A' + 'a';
}

int main() {

	for(int i = 0; i < MAX_LEN; i++)
		tabela[i][0] = tabela[0][i] = i;

	string caminho = "dicionario/  ";
	int indice = 0;

	ifstream entradas[MAX_LEN];

	for(char c = '0'; c <= '2'; c++) {

		caminho[11] = c;

		for(char d = '0'; d <= '9'; d++) {

			caminho[12] = d;
			entradas[indice++].open(caminho);
		}
	}

	/* Palavra que será corrigida */
	string palavra;

	cout << "Palavra: ";
	cin >> palavra;

	toLowerCase(palavra);

	/* Arquivo de dicionário */
	ifstream entrada;
	entrada.open("dicionario.txt");

	string str;

	Solucao melhores[QUANT_SUG];

	int size = 0;

	int len = palavra.length();

	if(len > 29) {
		cout << "Palavra muito grande!" << endl;
		return 0;
	}


	bool correto = false;
	
	/* Procurar apenas nos arquivos que possuam de 2 letras a menos até 2 letras a mais */
	for(int i = len - 2; i <= len + 2; i++) {

		if(i > 0 && i <= MAX_LEN)
			correto = matches(palavra, melhores, &size, entradas[i]);

		if(correto)
			break;
	}


	if(correto) {
		cout << "A palavra esta correta!" << endl;
	} else {

		cout << "A palavra esta incorreta!" << endl;
 
		cout << "Sugestões: ";

		for(int i = 0; i < QUANT_SUG; i++)
			cout << melhores[i].palavra << " ";

		cout << endl;
	}

	return 0;
}