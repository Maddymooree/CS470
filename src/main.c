#include <stdio.h>
#include "graph.h"
#include "binheap_pq.h"
#include "dijkstra.h"
#include "prim.h"

int main() {
    Graph* g = graph_create(5, 0);
    graph_add_edge(g, 0, 1, 4);
    graph_add_edge(g, 0, 2, 1);
    graph_add_edge(g, 2, 1, 2);
    graph_add_edge(g, 1, 3, 1);
    graph_add_edge(g, 2, 3, 5);
    graph_add_edge(g, 3, 4, 3);

    DijkstraResult d = dijkstra_run(g, 0, &BINHEAP_OPS);
    printf("Dijkstra dist from 0:\n");
    for (int i=0;i<g->n;i++) {
        printf("  %d: %lld\n", i, d.dist[i]);
    }

    PrimResult p = prim_run(g, 0, &BINHEAP_OPS);
    printf("\nPrim MST total weight (root=0): %lld\n", p.mst_weight);
    printf("Parent array:\n");
    for (int i=0;i<g->n;i++) {
        printf("  parent[%d] = %d\n", i, p.parent[i]);
    }

    prim_free(p);
    dijkstra_free(d);
    graph_destroy(g);
    return 0;
}
