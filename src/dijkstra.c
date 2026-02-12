// src/dijkstra.c
#include "dijkstra.h"
#include <stdlib.h>
#include <limits.h>

#define INF (LLONG_MAX/4)

DijkstraResult dijkstra_run(const Graph* g, int s, const PQOps* ops) {
    int n = g->n;
    long long* dist = (long long*)malloc(sizeof(long long) * n);
    int* parent = (int*)malloc(sizeof(int) * n);
    PQHandle** handle = (PQHandle**)calloc((size_t)n, sizeof(PQHandle*)); // NULL init

    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[s] = 0;

    PQ* pq = ops->create(n);

    // Insert ONLY the source to start (lazy insertion)
    handle[s] = ops->insert(pq, dist[s], s);

    while (!ops->is_empty(pq)) {
        PQItem it = ops->extract_min(pq);
        int u = it.value;
        long long du = it.key;

        // Skip stale entries (if any)
        if (du != dist[u]) continue;

        for (Edge* e = g->adj[u]; e; e = e->next) {
            int v = e->to;
            long long nd = du + e->w;

            if (nd < dist[v]) {
                dist[v] = nd;
                parent[v] = u;

                if (handle[v] == NULL) {
                    handle[v] = ops->insert(pq, nd, v);
                } else {
                    ops->decrease_key(pq, handle[v], nd);
                }
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
