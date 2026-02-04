// graph.c
#include "graph.h"
#include <stdlib.h>

Graph* graph_create(int n, int directed) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->n = n;
    g->directed = directed;
    g->adj = (Edge**)calloc(n, sizeof(Edge*));
    return g;
}

static void add_one(Graph* g, int u, int v, int w) {
    Edge* e = (Edge*)malloc(sizeof(Edge));
    e->to = v;
    e->w = w;
    e->next = g->adj[u];
    g->adj[u] = e;
}

void graph_add_edge(Graph* g, int u, int v, int w) {
    add_one(g, u, v, w);
    if (!g->directed) add_one(g, v, u, w);
}

void graph_destroy(Graph* g) {
    for (int i = 0; i < g->n; i++) {
        Edge* cur = g->adj[i];
        while (cur) {
            Edge* nxt = cur->next;
            free(cur);
            cur = nxt;
        }
    }
    free(g->adj);
    free(g);
}
