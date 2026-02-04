// prim.h
#ifndef PRIM_H
#define PRIM_H

#include "graph.h"
#include "pq.h"

typedef struct {
    long long mst_weight;
    int* parent; // parent[v] in MST, parent[root] = -1
} PrimResult;

PrimResult prim_run(const Graph* g, int root, const PQOps* ops);
void prim_free(PrimResult r);

#endif
