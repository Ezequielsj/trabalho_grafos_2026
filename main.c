#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "biblioteca_grafos.h"

// Função para criar a pasta de resultados se ela não existir
void criar_pasta_resultados() {
    struct stat st = {0};
    if (stat("resultados", &st) == -1) {
        // A função mkdir em sistemas Linux (como o WSL) espera 2 argumentos
        mkdir("resultados", 0700);
    }
}

// Função para salvar informações de busca (pai e nível) em um arquivo
void salvar_info_busca(Grafo *grafo, int *pais, int *niveis, int vertice_inicial, const char *nome_busca) {
    char nome_arquivo[256];
    snprintf(nome_arquivo, sizeof(nome_arquivo), "resultados/%s_%d.txt", nome_busca, vertice_inicial);

    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo de saida para a busca.\n");
        return;
    }

    fprintf(arquivo, "Arvore de Busca em %s (iniciando do vertice %d)\n", nome_busca, vertice_inicial);
    fprintf(arquivo, "Vertice\tPai\tNivel\n");

    for (int i = 0; i < grafo->num_vertices; i++) {
        fprintf(arquivo, "%d\t%d\t%d\n", i + 1, pais[i] + 1, niveis[i]);
    }

    fclose(arquivo);
}

// Função para salvar a quantidade de memória utilizada por diferentes representações de grafos
void salvar_memoria_grafo(Grafo *grafo, const char *nome_arquivo) {
    char caminho_arquivo[256];
    snprintf(caminho_arquivo, sizeof(caminho_arquivo), "resultados/%s", nome_arquivo);

    FILE *arquivo_saida = fopen(caminho_arquivo, "w");
    if (!arquivo_saida) {
        printf("Erro ao abrir o arquivo de saida: %s\n", caminho_arquivo);
        return;
    }

    long long memoria_lista_adj = (long long)sizeof(NoListaAdj *) * grafo->num_vertices + (long long)sizeof(NoListaAdj) * (long long)grafo->num_arestas * 2;
    long long memoria_matriz_adj = (long long)sizeof(int *) * grafo->num_vertices + (long long)sizeof(int) * (long long)grafo->num_vertices * grafo->num_vertices;

    fprintf(arquivo_saida, "Memoria utilizada (Lista de Adjacencia): %lld bytes\n", memoria_lista_adj);
    fprintf(arquivo_saida, "Memoria utilizada (Matriz de Adjacencia): %lld bytes\n", memoria_matriz_adj);

    fclose(arquivo_saida);
}

// Função para salvar o resultado das distâncias entre pares de vértices específicos
void salvar_distancias(Grafo *grafo, const char *nome_arquivo) {
    char caminho_arquivo[256];
    snprintf(caminho_arquivo, sizeof(caminho_arquivo), "resultados/%s", nome_arquivo);

    FILE *arquivo_saida = fopen(caminho_arquivo, "w");
    if (!arquivo_saida) {
        printf("Erro ao abrir o arquivo de saida: %s\n", caminho_arquivo);
        return;
    }

    fprintf(arquivo_saida, "Distancias entre Pares de Vertices\n");
    int pares[3][2] = {{10, 20}, {10, 30}, {20, 30}};

    for (int i = 0; i < 3; i++) {
        int origem = pares[i][0];
        int destino = pares[i][1];
        int distancia = calcular_distancia(grafo, origem, destino);
        
        if (distancia != -1) {
            fprintf(arquivo_saida, "Distancia entre %d e %d: %d\n", origem, destino, distancia);
        } else {
            fprintf(arquivo_saida, "Nao ha caminho entre %d e %d.\n", origem, destino);
        }
    }

    fclose(arquivo_saida);
}


int main() {
    criar_pasta_resultados();

    int escolha_rep;
    printf("Escolha a representacao do grafo:\n");
    printf("1. Lista de Adjacencia\n");
    printf("2. Matriz de Adjacencia\n");
    printf("Escolha: ");
    scanf("%d", &escolha_rep);

    RepresentacaoGrafo tipo_representacao;
    if (escolha_rep == 1) {
        tipo_representacao = LISTA_ADJACENCIA;
    } else if (escolha_rep == 2) {
        tipo_representacao = MATRIZ_ADJACENCIA;
    } else {
        printf("Opcao invalida. Usando lista de adjacencia como padrao.\n");
        tipo_representacao = LISTA_ADJACENCIA;
    }

    Grafo *grafo = ler_grafo_do_arquivo("grafo_3.txt", tipo_representacao);
    if (!grafo) {
        return 1;
    }

    printf("Numero de vertices: %d\n", grafo->num_vertices);

    int escolha;
    printf("\nSelecione o teste a ser executado:\n");
    printf("1. Teste de Memoria\n");
    printf("2. Salvar Informacoes Gerais do Grafo\n");
    printf("3. Teste de Tempo de Execucao BFS\n");
    printf("4. Teste de Tempo de Execucao DFS\n");
    printf("5. Teste de Pais e Niveis da Busca (BFS e DFS)\n");
    printf("6. Teste de Distancias entre Pares de Vertices\n");
    printf("7. Teste de Componentes Conexas\n");
    printf("8. Teste de Diametro do Grafo\n");
    printf("9. Sair\n");
    printf("Escolha: ");
    scanf("%d", &escolha);

    switch (escolha) {
        case 1:
            salvar_memoria_grafo(grafo, "1.memoria.txt");
            printf("Resultados do teste de memoria salvos em 'resultados/1.memoria.txt'\n");
            break;
        case 2:
            salvar_informacoes_grafo(grafo, "2.informacoes_gerais.txt");
            printf("Informacoes gerais salvas em 'resultados/2.informacoes_gerais.txt'\n");
            break;
        case 3: {
            double tempo_total = 0.0;
            int *pais = (int *)malloc(grafo->num_vertices * sizeof(int));
            int *niveis = (int *)malloc(grafo->num_vertices * sizeof(int));
            for (int i = 0; i < 100; i++) {
                int vertice_inicial = (i % grafo->num_vertices) + 1;
                clock_t inicio = clock();
                busca_em_largura(grafo, vertice_inicial, pais, niveis);
                clock_t fim = clock();
                tempo_total += (double)(fim - inicio) / CLOCKS_PER_SEC;
            }
            double tempo_medio = tempo_total / 100.0;
            FILE *arquivo_tempo = fopen("resultados/3.tempo_medio_bfs.txt", "w");
            if (arquivo_tempo) {
                fprintf(arquivo_tempo, "Tempo medio de execucao da BFS: %.6f segundos\n", tempo_medio);
                fclose(arquivo_tempo);
            }
            free(pais);
            free(niveis);
            printf("Tempo medio da BFS salvo em 'resultados/3.tempo_medio_bfs.txt'\n");
            break;
        }
        case 4: {
            double tempo_total = 0.0;
            int *pais = (int *)malloc(grafo->num_vertices * sizeof(int));
            int *niveis = (int *)malloc(grafo->num_vertices * sizeof(int));
            for (int i = 0; i < 100; i++) {
                int vertice_inicial = (i % grafo->num_vertices) + 1;
                clock_t inicio = clock();
                busca_em_profundidade(grafo, vertice_inicial, pais, niveis);
                clock_t fim = clock();
                tempo_total += (double)(fim - inicio) / CLOCKS_PER_SEC;
            }
            double tempo_medio = tempo_total / 100.0;
            FILE *arquivo_tempo = fopen("resultados/4.tempo_medio_dfs.txt", "w");
            if (arquivo_tempo) {
                fprintf(arquivo_tempo, "Tempo medio de execucao da DFS: %.6f segundos\n", tempo_medio);
                fclose(arquivo_tempo);
            }
            free(pais);
            free(niveis);
            printf("Tempo medio da DFS salvo em 'resultados/4.tempo_medio_dfs.txt'\n");
            break;
        }
        case 5: {
            int vertices_iniciais[] = {1, 2, 3};
            for (int i = 0; i < 3; i++) {
                int vertice_inicial = vertices_iniciais[i];
                if (vertice_inicial > grafo->num_vertices) {
                    printf("Vertice inicial %d nao existe no grafo. Pulando...\n", vertice_inicial);
                    continue;
                }
                
                int *pais_bfs = (int *)malloc(grafo->num_vertices * sizeof(int));
                int *niveis_bfs = (int *)malloc(grafo->num_vertices * sizeof(int));
                busca_em_largura(grafo, vertice_inicial, pais_bfs, niveis_bfs);
                salvar_info_busca(grafo, pais_bfs, niveis_bfs, vertice_inicial, "BFS_arvore");
                free(pais_bfs);
                free(niveis_bfs);

                int *pais_dfs = (int *)malloc(grafo->num_vertices * sizeof(int));
                int *niveis_dfs = (int *)malloc(grafo->num_vertices * sizeof(int));
                busca_em_profundidade(grafo, vertice_inicial, pais_dfs, niveis_dfs);
                salvar_info_busca(grafo, pais_dfs, niveis_dfs, vertice_inicial, "DFS_arvore");
                free(pais_dfs);
                free(niveis_dfs);
            }
            printf("Arvores de busca para vertices 1, 2 e 3 salvas em 'resultados/'\n");
            break;
        }
        case 6:
            salvar_distancias(grafo, "6.distancias.txt");
            printf("Distancias salvas em 'resultados/6.distancias.txt'\n");
            break;
        case 7:
            encontrar_componentes_conexas(grafo, "7.componentes_conexas.txt");
            printf("Componentes conexas salvas em 'resultados/7.componentes_conexas.txt'\n");
            break;
        case 8: {
            int diametro = calcular_diametro(grafo);
            FILE *arquivo_diametro = fopen("resultados/8.diametro.txt", "w");
            if (arquivo_diametro) {
                fprintf(arquivo_diametro, "Diametro do grafo: %d\n", diametro);
                fclose(arquivo_diametro);
            }
            printf("Diametro do grafo salvo em 'resultados/8.diametro.txt'\n");
            break;
        }
        case 9:
            printf("Saindo...\n");
            liberar_grafo(grafo);
            return 0;
        default:
            printf("Opcao invalida! Tente novamente.\n");
            break;
    }

    liberar_grafo(grafo);
    return 0;
}