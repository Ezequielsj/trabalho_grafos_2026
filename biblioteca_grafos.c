#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "biblioteca_grafos.h"

typedef struct {
    int tamanho;
    int *vertices;
} ComponenteConexa;

int comparar_componentes(const void *a, const void *b) {
    ComponenteConexa *compA = (ComponenteConexa *)a;
    ComponenteConexa *compB = (ComponenteConexa *)b;
    return compB->tamanho - compA->tamanho;
}

Grafo *criar_grafo_lista_adj(int num_vertices) {
    Grafo *grafo = (Grafo *)malloc(sizeof(Grafo));
    if (!grafo) return NULL;
    
    grafo->num_vertices = num_vertices;
    grafo->num_arestas = 0;
    grafo->lista_adj = (NoListaAdj **)malloc(num_vertices * sizeof(NoListaAdj *));
    grafo->graus = (int *)calloc(num_vertices, sizeof(int));
    grafo->matriz_adj = NULL;
    grafo->tipo_representacao = LISTA_ADJACENCIA;
    
    for (int i = 0; i < num_vertices; i++) {
        grafo->lista_adj[i] = NULL;
    }
    return grafo;
}

Grafo *criar_grafo_matriz_adj(int num_vertices) {
    Grafo *grafo = (Grafo *)malloc(sizeof(Grafo));
    if (!grafo) return NULL;
    
    grafo->num_vertices = num_vertices;
    grafo->num_arestas = 0;
    grafo->lista_adj = NULL;
    grafo->graus = (int *)calloc(num_vertices, sizeof(int));
    grafo->matriz_adj = (int **)malloc(num_vertices * sizeof(int *));
    grafo->tipo_representacao = MATRIZ_ADJACENCIA;
    
    for (int i = 0; i < num_vertices; i++) {
        grafo->matriz_adj[i] = (int *)calloc(num_vertices, sizeof(int));
    }
    return grafo;
}

void adicionar_aresta(Grafo *grafo, int v1, int v2) {
    if (grafo->tipo_representacao == LISTA_ADJACENCIA) {
        NoListaAdj *novoNo = (NoListaAdj *)malloc(sizeof(NoListaAdj));
        novoNo->vertice = v2;
        novoNo->proximo = grafo->lista_adj[v1];
        grafo->lista_adj[v1] = novoNo;
        grafo->graus[v1]++;
        
        novoNo = (NoListaAdj *)malloc(sizeof(NoListaAdj));
        novoNo->vertice = v1;
        novoNo->proximo = grafo->lista_adj[v2];
        grafo->lista_adj[v2] = novoNo;
        grafo->graus[v2]++;
    } else {
        grafo->matriz_adj[v1][v2] = 1;
        grafo->matriz_adj[v2][v1] = 1;
        grafo->graus[v1]++;
        grafo->graus[v2]++;
    }
    grafo->num_arestas++;
}

void liberar_grafo(Grafo *grafo) {
    if (grafo->lista_adj) {
        for (int i = 0; i < grafo->num_vertices; i++) {
            NoListaAdj *temp = grafo->lista_adj[i];
            while (temp) {
                NoListaAdj *prox = temp->proximo;
                free(temp);
                temp = prox;
            }
        }
        free(grafo->lista_adj);
    }
    if (grafo->matriz_adj) {
        for (int i = 0; i < grafo->num_vertices; i++) {
            free(grafo->matriz_adj[i]);
        }
        free(grafo->matriz_adj);
    }
    free(grafo->graus);
    free(grafo);
}

// ----------------------------------------------------
// Funções de Leitura e Saída
// ----------------------------------------------------

Grafo *ler_grafo_do_arquivo(const char *nome_arquivo, RepresentacaoGrafo tipo_representacao) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) return NULL;
    
    int num_vertices;
    if (fscanf(arquivo, "%d", &num_vertices) != 1) {
        fclose(arquivo);
        return NULL;
    }
    
    Grafo *grafo;
    if (tipo_representacao == LISTA_ADJACENCIA) {
        grafo = criar_grafo_lista_adj(num_vertices);
    } else {
        grafo = criar_grafo_matriz_adj(num_vertices);
    }
    
    int v1, v2;
    while (fscanf(arquivo, "%d %d", &v1, &v2) == 2) {
        v1--; v2--;
        adicionar_aresta(grafo, v1, v2);
    }
    
    fclose(arquivo);
    return grafo;
}

int comparar_inteiros(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

void salvar_informacoes_grafo(Grafo *grafo, const char *nome_arquivo_saida) {
    FILE *arquivo_saida = fopen(nome_arquivo_saida, "w");
    if (!arquivo_saida) return;
    
    fprintf(arquivo_saida, "Numero de vertices: %d\n", grafo->num_vertices);
    fprintf(arquivo_saida, "Numero de arestas: %d\n", grafo->num_arestas);
    
    int grau_min = grafo->graus[0], grau_max = grafo->graus[0];
    int soma_graus = 0;
    
    for (int i = 0; i < grafo->num_vertices; i++) {
        if (grafo->graus[i] < grau_min) grau_min = grafo->graus[i];
        if (grafo->graus[i] > grau_max) grau_max = grafo->graus[i];
        soma_graus += grafo->graus[i];
    }
    double grau_medio = (double)soma_graus / grafo->num_vertices;
    
    int *graus_ordenados = (int *)malloc(grafo->num_vertices * sizeof(int));
    for (int i = 0; i < grafo->num_vertices; i++) {
        graus_ordenados[i] = grafo->graus[i];
    }
    qsort(graus_ordenados, grafo->num_vertices, sizeof(int), comparar_inteiros);
    
    double mediana_grau;
    if (grafo->num_vertices % 2 == 0) {
        mediana_grau = (graus_ordenados[grafo->num_vertices / 2 - 1] + graus_ordenados[grafo->num_vertices / 2]) / 2.0;
    } else {
        mediana_grau = graus_ordenados[grafo->num_vertices / 2];
    }
    free(graus_ordenados);
    
    fprintf(arquivo_saida, "Grau minimo: %d\n", grau_min);
    fprintf(arquivo_saida, "Grau maximo: %d\n", grau_max);
    fprintf(arquivo_saida, "Grau medio: %.2f\n", grau_medio);
    fprintf(arquivo_saida, "Mediana do grau: %.2f\n", mediana_grau);
    fclose(arquivo_saida);
}

// ----------------------------------------------------
// Funções de Busca (BFS e DFS) e Cálculos
// ----------------------------------------------------

void busca_em_largura(Grafo *grafo, int vertice_inicial, int *pais, int *niveis) {
    vertice_inicial--;
    bool *visitados = (bool *)calloc(grafo->num_vertices, sizeof(bool));
    int *fila = (int *)malloc(grafo->num_vertices * sizeof(int));
    int inicio = 0, fim = 0;
    
    for (int i = 0; i < grafo->num_vertices; i++) {
        niveis[i] = -1;
        pais[i] = -1;
    }
    
    visitados[vertice_inicial] = true;
    niveis[vertice_inicial] = 0;
    fila[fim++] = vertice_inicial;
    
    // OTIMIZAÇÃO: If removido de dentro do ciclo principal
    if (grafo->tipo_representacao == LISTA_ADJACENCIA) {
        while (inicio < fim) {
            int v = fila[inicio++];
            NoListaAdj *adj = grafo->lista_adj[v];
            while (adj != NULL) {
                if (!visitados[adj->vertice]) {
                    visitados[adj->vertice] = true;
                    pais[adj->vertice] = v;
                    niveis[adj->vertice] = niveis[v] + 1;
                    fila[fim++] = adj->vertice;
                }
                adj = adj->proximo;
            }
        }
    } else {
        while (inicio < fim) {
            int v = fila[inicio++];
            for (int i = 0; i < grafo->num_vertices; i++) {
                if (grafo->matriz_adj[v][i] == 1 && !visitados[i]) {
                    visitados[i] = true;
                    pais[i] = v;
                    niveis[i] = niveis[v] + 1;
                    fila[fim++] = i;
                }
            }
        }
    }
    
    free(visitados);
    free(fila);
}

void busca_em_profundidade(Grafo *grafo, int vertice_inicial, int *pais, int *niveis) {
    vertice_inicial--;
    bool *visitados = (bool *)calloc(grafo->num_vertices, sizeof(bool));
    
    int capacidade_pilha = grafo->num_vertices;
    int *pilha = (int *)malloc(capacidade_pilha * sizeof(int));
    int topo = -1;
    
    for (int i = 0; i < grafo->num_vertices; i++) {
        niveis[i] = -1;
        pais[i] = -1;
    }
    
    pilha[++topo] = vertice_inicial;
    niveis[vertice_inicial] = 0;
    
    // OTIMIZAÇÃO: If removido de dentro do ciclo
    if (grafo->tipo_representacao == LISTA_ADJACENCIA) {
        while (topo >= 0) {
            int u = pilha[topo--];
            if (!visitados[u]) {
                visitados[u] = true;
                NoListaAdj *adj = grafo->lista_adj[u];
                while (adj != NULL) {
                    int v = adj->vertice;
                    if (!visitados[v]) {
                        if (topo + 1 >= capacidade_pilha) {
                            capacidade_pilha *= 2;
                            pilha = (int *)realloc(pilha, capacidade_pilha * sizeof(int));
                        }
                        pilha[++topo] = v;
                        pais[v] = u;
                        niveis[v] = niveis[u] + 1;
                    }
                    adj = adj->proximo;
                }
            }
        }
    } else {
        while (topo >= 0) {
            int u = pilha[topo--];
            if (!visitados[u]) {
                visitados[u] = true;
                for (int v = grafo->num_vertices - 1; v >= 0; v--) {
                    if (grafo->matriz_adj[u][v] == 1 && !visitados[v]) {
                        if (topo + 1 >= capacidade_pilha) {
                            capacidade_pilha *= 2;
                            pilha = (int *)realloc(pilha, capacidade_pilha * sizeof(int));
                        }
                        pilha[++topo] = v;
                        pais[v] = u;
                        niveis[v] = niveis[u] + 1;
                    }
                }
            }
        }
    }
    
    free(visitados);
    free(pilha);
}

int calcular_distancia(Grafo *grafo, int origem, int destino) {
    origem--; destino--;
    if (origem == destino) return 0;
    if (origem < 0 || origem >= grafo->num_vertices || destino < 0 || destino >= grafo->num_vertices) return -1;
    
    bool *visitados = (bool *)calloc(grafo->num_vertices, sizeof(bool));
    int *distancia = (int *)malloc(grafo->num_vertices * sizeof(int));
    int *fila = (int *)malloc(grafo->num_vertices * sizeof(int));
    int inicio = 0, fim = 0;
    
    for (int i = 0; i < grafo->num_vertices; i++) distancia[i] = -1;
    
    visitados[origem] = true;
    distancia[origem] = 0;
    fila[fim++] = origem;
    
    // OTIMIZAÇÃO: If removido de dentro do ciclo
    if (grafo->tipo_representacao == LISTA_ADJACENCIA) {
        while (inicio < fim) {
            int u = fila[inicio++];
            NoListaAdj *adj = grafo->lista_adj[u];
            while (adj) {
                int v = adj->vertice;
                if (!visitados[v]) {
                    visitados[v] = true;
                    distancia[v] = distancia[u] + 1;
                    fila[fim++] = v;
                    if (v == destino) {
                        int dist = distancia[destino];
                        free(visitados); free(distancia); free(fila);
                        return dist;
                    }
                }
                adj = adj->proximo;
            }
        }
    } else {
        while (inicio < fim) {
            int u = fila[inicio++];
            for (int v = 0; v < grafo->num_vertices; v++) {
                if (grafo->matriz_adj[u][v] == 1 && !visitados[v]) {
                    visitados[v] = true;
                    distancia[v] = distancia[u] + 1;
                    fila[fim++] = v;
                    if (v == destino) {
                        int dist = distancia[destino];
                        free(visitados); free(distancia); free(fila);
                        return dist;
                    }
                }
            }
        }
    }
    
    free(visitados); free(distancia); free(fila);
    return -1;
}

int calcular_diametro(Grafo *grafo) {
    if (grafo->num_vertices <= 1000) {
        int max_distancia = 0;
        for (int i = 0; i < grafo->num_vertices; i++) {
            for (int j = i + 1; j < grafo->num_vertices; j++) {
                int dist = calcular_distancia(grafo, i + 1, j + 1);
                if (dist != -1 && dist > max_distancia) {
                    max_distancia = dist;
                }
            }
        }
        return max_distancia;
    } else {
        int max_distancia = 0;
        int amostra_size = 100;
        srand(time(NULL));
        
        bool *visitados = (bool *)malloc(grafo->num_vertices * sizeof(bool));
        int *distancia = (int *)malloc(grafo->num_vertices * sizeof(int));
        int *fila = (int *)malloc(grafo->num_vertices * sizeof(int));
        
        for (int k = 0; k < amostra_size; k++) {
            int vertice_inicial = rand() % grafo->num_vertices;
            int inicio = 0, fim = 0;
            
            for (int i = 0; i < grafo->num_vertices; i++) {
                visitados[i] = false;
                distancia[i] = -1;
            }
            
            visitados[vertice_inicial] = true;
            distancia[vertice_inicial] = 0;
            fila[fim++] = vertice_inicial;
            
            // OTIMIZAÇÃO: If removido de dentro do ciclo
            if (grafo->tipo_representacao == LISTA_ADJACENCIA) {
                while (inicio < fim) {
                    int u = fila[inicio++];
                    NoListaAdj *adj = grafo->lista_adj[u];
                    while (adj) {
                        int v = adj->vertice;
                        if (!visitados[v]) {
                            visitados[v] = true;
                            distancia[v] = distancia[u] + 1;
                            fila[fim++] = v;
                        }
                        adj = adj->proximo;
                    }
                }
            } else {
                while (inicio < fim) {
                    int u = fila[inicio++];
                    for (int v = 0; v < grafo->num_vertices; v++) {
                        if (grafo->matriz_adj[u][v] == 1 && !visitados[v]) {
                            visitados[v] = true;
                            distancia[v] = distancia[u] + 1;
                            fila[fim++] = v;
                        }
                    }
                }
            }
            
            int local_max_dist = 0;
            for (int i = 0; i < grafo->num_vertices; i++) {
                if (distancia[i] > local_max_dist) local_max_dist = distancia[i];
            }
            if (local_max_dist > max_distancia) max_distancia = local_max_dist;
        }
        
        free(visitados);
        free(distancia);
        free(fila);
        
        return max_distancia;
    }
}

void encontrar_componentes_conexas(Grafo *grafo, const char *nome_arquivo_saida) {
    FILE *arquivo_saida = fopen(nome_arquivo_saida, "w"); 
    if (!arquivo_saida) return;
    
    bool *visitados = (bool *)calloc(grafo->num_vertices, sizeof(bool));
    ComponenteConexa *componentes = (ComponenteConexa *)malloc(grafo->num_vertices * sizeof(ComponenteConexa));
    int num_componentes = 0;
    
    for (int i = 0; i < grafo->num_vertices; i++) {
        if (!visitados[i]) {
            int capacidade_vertices = 10;
            componentes[num_componentes].vertices = (int *)malloc(capacidade_vertices * sizeof(int));
            componentes[num_componentes].tamanho = 0;
            
            int *fila = (int *)malloc(grafo->num_vertices * sizeof(int));
            int inicio = 0, fim = 0;
            
            fila[fim++] = i;
            visitados[i] = true;
            
            // OTIMIZAÇÃO: If removido de dentro do ciclo
            if (grafo->tipo_representacao == LISTA_ADJACENCIA) {
                while (inicio < fim) {
                    int vertice = fila[inicio++];
                    
                    if (componentes[num_componentes].tamanho >= capacidade_vertices) {
                        capacidade_vertices *= 2;
                        componentes[num_componentes].vertices = (int *)realloc(componentes[num_componentes].vertices, capacidade_vertices * sizeof(int));
                    }
                    componentes[num_componentes].vertices[componentes[num_componentes].tamanho++] = vertice;
                    
                    NoListaAdj *adj = grafo->lista_adj[vertice];
                    while (adj != NULL) {
                        if (!visitados[adj->vertice]) {
                            visitados[adj->vertice] = true;
                            fila[fim++] = adj->vertice;
                        }
                        adj = adj->proximo;
                    }
                }
            } else {
                while (inicio < fim) {
                    int vertice = fila[inicio++];
                    
                    if (componentes[num_componentes].tamanho >= capacidade_vertices) {
                        capacidade_vertices *= 2;
                        componentes[num_componentes].vertices = (int *)realloc(componentes[num_componentes].vertices, capacidade_vertices * sizeof(int));
                    }
                    componentes[num_componentes].vertices[componentes[num_componentes].tamanho++] = vertice;
                    
                    for (int j = 0; j < grafo->num_vertices; j++) {
                        if (grafo->matriz_adj[vertice][j] == 1 && !visitados[j]) {
                            visitados[j] = true;
                            fila[fim++] = j;
                        }
                    }
                }
            }
            free(fila);
            num_componentes++;
        }
    }
    qsort(componentes, num_componentes, sizeof(ComponenteConexa), comparar_componentes);
    
    fprintf(arquivo_saida, "\nComponentes Conexas:\n");
    for (int i = 0; i < num_componentes; i++) {
        fprintf(arquivo_saida, "Tamanho: %d, Vertices: ", componentes[i].tamanho);
        for (int j = 0; j < componentes[i].tamanho; j++) {
            fprintf(arquivo_saida, "%d ", componentes[i].vertices[j] + 1);
        }
        fprintf(arquivo_saida, "\n");
        free(componentes[i].vertices);
    }
    fprintf(arquivo_saida, "Numero total de componentes conexas: %d\n", num_componentes);
    
    free(visitados); free(componentes); fclose(arquivo_saida);
}