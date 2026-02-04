// dijkstra.h
#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include "pq.h"

typedef struct {
    long long* dist;
    int* parent;
} DijkstraResult;

DijkstraResult dijkstra_run(const Graph* g, int source, const PQOps* ops);
void dijkstra_free(DijkstraResult r);

#endif
