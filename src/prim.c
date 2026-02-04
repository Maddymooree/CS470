// prim.c
#include "prim.h"
#include <stdlib.h>
#include <limits.h>

#define INF (LLONG_MAX/4)

PrimResult prim_run(const Graph* g, int root, const PQOps* ops) {
    int n = g->n;
    long long* key = (long long*)malloc(sizeof(long long)*n);
    int* parent = (int*)malloc(sizeof(int)*n);
    int* in_mst = (int*)calloc(n, sizeof(int));
    PQHandle** handle = (PQHandle**)malloc(sizeof(PQHandle*)*n);

    for (int i=0;i<n;i++){ key[i]=INF; parent[i]=-1; }
    key[root]=0;

    PQ* pq = ops->create(n);
    for (int v=0; v<n; v++) {
        handle[v] = ops->insert(pq, key[v], v);
    }

    long long total = 0;

    while (!ops->is_empty(pq)) {
        PQItem it = ops->extract_min(pq);
        int u = it.value;
        long long ku = it.key;

        if (in_mst[u]) continue;
        in_mst[u] = 1;

        if (ku == INF) break;          // disconnected graph
        total += ku;

        for (Edge* e = g->adj[u]; e; e = e->next) {
            int v = e->to;
            if (!in_mst[v] && e->w < key[v]) {
                key[v] = e->w;
                parent[v] = u;
                ops->decrease_key(pq, handle[v], key[v]);
            }
        }
    }

    ops->destroy(pq);
    free(key);
    free(in_mst);
    free(handle);

    PrimResult r = { total, parent };
    return r;
}

void prim_free(PrimResult r) {
    free(r.parent);
}
