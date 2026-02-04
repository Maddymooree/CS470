// graph.h
#ifndef GRAPH_H
#define GRAPH_H

typedef struct Edge {
    int to;
    int w;
    struct Edge* next;
} Edge;

typedef struct {
    int n;
    Edge** adj; // array of head pointers
    int directed;
} Graph;

Graph* graph_create(int n, int directed);
void   graph_add_edge(Graph* g, int u, int v, int w);
void   graph_destroy(Graph* g);

#endif
