// dijkstra.c
#include "dijkstra.h"
#include <stdlib.h>
#include <limits.h>

#define INF (LLONG_MAX/4)

DijkstraResult dijkstra_run(const Graph* g, int s, const PQOps* ops) {
    int n = g->n;
    long long* dist = (long long*)malloc(sizeof(long long)*n);
    int* parent = (int*)malloc(sizeof(int)*n);
    PQHandle** handle = (PQHandle**)malloc(sizeof(PQHandle*)*n);

    for (int i=0;i<n;i++){ dist[i]=INF; parent[i]=-1; }
    dist[s]=0;

    PQ* pq = ops->create(n);
    for (int v=0; v<n; v++) {
        handle[v] = ops->insert(pq, dist[v], v);
    }

    while (!ops->is_empty(pq)) {
        PQItem it = ops->extract_min(pq);
        int u = it.value;
        long long du = it.key;

        // if this happens, your PQ implementation is inconsistent with handles.
        if (du != dist[u]) continue;

        for (Edge* e = g->adj[u]; e; e = e->next) {
            int v = e->to;
            long long nd = du + e->w;
            if (nd < dist[v]) {
                dist[v] = nd;
                parent[v] = u;
                ops->decrease_key(pq, handle[v], nd);
            }
        }
    }

    ops->destroy(pq);
    free(handle);

    DijkstraResult r = { dist, parent };
    return r;
}

void dijkstra_free(DijkstraResult r) {
    free(r.dist);
    free(r.parent);
}
