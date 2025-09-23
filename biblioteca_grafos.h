#ifndef BIBLIOTECA_GRAFOS_H
#define BIBLIOTECA_GRAFOS_H

#include <stdbool.h>

typedef enum { LISTA_ADJACENCIA, MATRIZ_ADJACENCIA } RepresentacaoGrafo;

typedef struct NoListaAdj {
    int vertice;
    struct NoListaAdj *proximo;
} NoListaAdj;

typedef struct {
    int num_vertices;
    int num_arestas;
    int *graus;
    NoListaAdj **lista_adj;
    int **matriz_adj;
    RepresentacaoGrafo tipo_representacao;
} Grafo;

Grafo* criar_grafo_lista_adj(int num_vertices);
Grafo* criar_grafo_matriz_adj(int num_vertices);
void adicionar_aresta(Grafo *grafo, int v1, int v2);
void liberar_grafo(Grafo *grafo);

void busca_em_largura(Grafo *grafo, int vertice_inicial, int *pais, int *niveis);
void busca_em_profundidade(Grafo *grafo, int vertice_inicial, int *pais, int *niveis);

int calcular_distancia(Grafo *grafo, int origem, int destino);
int calcular_diametro(Grafo *grafo);

void encontrar_componentes_conexas(Grafo *grafo, const char *nome_arquivo_saida);

Grafo* ler_grafo_do_arquivo(const char* nome_arquivo, RepresentacaoGrafo tipo_representacao);
void salvar_informacoes_grafo(Grafo *grafo, const char *nome_arquivo_saida);

#endif