#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "graph.h"
#include "binheap_pq.h"
#include "dijkstra.h"
#include "prim.h"

#include "fibheap_pq.h"
#include "pairing_pq.h"

static void run_and_print_dijkstra(const char *label,
                                   const Graph *g,
                                   int source,
                                   const PQOps *ops) {
    DijkstraResult d = dijkstra_run(g, source, ops);

    printf("\n%s\n", label);
    printf("Dijkstra dist from %d:\n", source);

    for (int i = 0; i < g->n; i++) {
        printf("  %d: %" PRId64 "\n", i, (int64_t)d.dist[i]);
    }

    dijkstra_free(d);
}

static void run_and_print_prim(const char *label,
                               const Graph *g,
                               int root,
                               const PQOps *ops) {
    PrimResult p = prim_run(g, root, ops);

    printf("\n%s\n", label);
    printf("Prim MST total weight (root=%d): %" PRId64 "\n",
           root, (int64_t)p.mst_weight);

    printf("Parent array:\n");
    for (int i = 0; i < g->n; i++) {
        printf("  parent[%d] = %d\n", i, p.parent[i]);
    }

    prim_free(p);
}

int main(void) {
    Graph *g = graph_create(5, 0);

    graph_add_edge(g, 0, 1, 4);
    graph_add_edge(g, 0, 2, 1);
    graph_add_edge(g, 2, 1, 2);
    graph_add_edge(g, 1, 3, 1);
    graph_add_edge(g, 2, 3, 5);
    graph_add_edge(g, 3, 4, 3);

    // Dijkstra tests
    run_and_print_dijkstra("=== Dijkstra using Binary Heap ===", g, 0, &BINHEAP_OPS);
    run_and_print_dijkstra("=== Dijkstra using Fibonacci Heap ===", g, 0, &fibheap_pq_ops);
    run_and_print_dijkstra("=== Dijkstra using Pairing Heap ===", g, 0, &pairing_pq_ops);

    // Prim tests
    run_and_print_prim("=== Prim using Binary Heap ===", g, 0, &BINHEAP_OPS);
    run_and_print_prim("=== Prim using Fibonacci Heap ===", g, 0, &fibheap_pq_ops);
    run_and_print_prim("=== Prim using Pairing Heap ===", g, 0, &pairing_pq_ops);

    graph_destroy(g);
    return 0;
}
