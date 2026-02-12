// src/bench.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "graph.h"
#include "dijkstra.h"
#include "prim.h"

#include "binheap_pq.h"
#include "fibheap_pq.h"
#include "pairing_pq.h"

#include "pq_profile.h"

#ifdef _WIN32
#include <windows.h>
static uint64_t now_ns(void) {
    static LARGE_INTEGER freq;
    static int init = 0;
    if (!init) { QueryPerformanceFrequency(&freq); init = 1; }
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (uint64_t)((1e9 * (double)t.QuadPart) / (double)freq.QuadPart);
}
#else
#include <time.h>
static uint64_t now_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}
#endif

// Simple RNG (repeatable)
static uint32_t rng_state = 123456789;
static uint32_t rng_u32(void) {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return rng_state;
}
static int rand_int(int lo, int hi) {
    return lo + (int)(rng_u32() % (uint32_t)(hi - lo + 1));
}

// Random directed graph with n vertices and m edges, weights in [1,20]
static Graph *make_random_graph(int n, int m, int directed) {
    Graph *g = graph_create(n, directed);
    for (int i = 0; i < m; i++) {
        int u = rand_int(0, n - 1);
        int v = rand_int(0, n - 1);
        if (u == v) { i--; continue; }
        int w = rand_int(1, 20);
        graph_add_edge(g, u, v, w);
        if (!directed) graph_add_edge(g, v, u, w);
    }
    return g;
}

static void run_case(const char *algo,
                     const char *heap,
                     const Graph *g,
                     const PQOps *base_ops,
                     int source_or_root) {
    PQProfile prof;
    pq_profile_reset(&prof);
    const PQOps *ops = pq_profile_ops(base_ops, &prof);

    uint64_t t0 = now_ns();

    if (algo[0] == 'D') {
        DijkstraResult d = dijkstra_run(g, source_or_root, ops);
        dijkstra_free(d);
    } else {
        PrimResult p = prim_run(g, source_or_root, ops);
        prim_free(p);
    }

    uint64_t t1 = now_ns();
    uint64_t total_ns = t1 - t0;

    // CSV columns:
    // algo,heap,n,total_ns,insert_count,extract_count,decrease_count,insert_ns,extract_ns,decrease_ns
    printf("%s,%s,%d,%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 "\n",
           algo, heap, g->n, total_ns,
           prof.insert_count, prof.extract_count, prof.decrease_count,
           prof.insert_ns, prof.extract_ns, prof.decrease_ns);

    fflush(stdout);
}

int main(void) {
    // CSV header
    printf("algo,heap,n,total_ns,insert_count,extract_count,decrease_count,insert_ns,extract_ns,decrease_ns\n");
    fflush(stdout);

    // DEBUG SETTINGS (small so it won’t feel “forever”)
    const int sizes[] = {200, 500, 1000};
    const int trials = 5;

    for (int si = 0; si < (int)(sizeof(sizes) / sizeof(sizes[0])); si++) {
        int n = sizes[si];

        // Sparse and “medium-dense” edge counts
        int m_sparse = 4 * n;    // ~O(n)
        int m_dense = (n * n) / 8; // NOT n^2 yet (avoid huge runs while debugging)

        for (int t = 0; t < trials; t++) {

            // ======================
            // Sparse graph runs
            // ======================
            fprintf(stderr, "\n[progress] building SPARSE graph n=%d m=%d trial=%d\n", n, m_sparse, t);
            fflush(stderr);
            Graph *gs = make_random_graph(n, m_sparse, 1);

            fprintf(stderr, "[progress] Dijkstra Binary (sparse)\n"); fflush(stderr);
            run_case("Dijkstra", "Binary", gs, &BINHEAP_OPS, 0);

            fprintf(stderr, "[progress] Dijkstra Fibonacci (sparse)\n"); fflush(stderr);
            run_case("Dijkstra", "Fibonacci", gs, &fibheap_pq_ops, 0);

            fprintf(stderr, "[progress] Dijkstra Pairing (sparse)\n"); fflush(stderr);
            run_case("Dijkstra", "Pairing", gs, &pairing_pq_ops, 0);

            fprintf(stderr, "[progress] Prim Binary (sparse)\n"); fflush(stderr);
            run_case("Prim", "Binary", gs, &BINHEAP_OPS, 0);

            fprintf(stderr, "[progress] Prim Fibonacci (sparse)\n"); fflush(stderr);
            run_case("Prim", "Fibonacci", gs, &fibheap_pq_ops, 0);

            fprintf(stderr, "[progress] Prim Pairing (sparse)\n"); fflush(stderr);
            run_case("Prim", "Pairing", gs, &pairing_pq_ops, 0);

            fprintf(stderr, "[progress] destroying SPARSE graph\n"); fflush(stderr);
            graph_destroy(gs);

            // ======================
            // Medium-dense graph runs
            // ======================
            fprintf(stderr, "\n[progress] building DENSE graph n=%d m=%d trial=%d\n", n, m_dense, t);
            fflush(stderr);
            Graph *gd = make_random_graph(n, m_dense, 1);

            fprintf(stderr, "[progress] Dijkstra Binary (dense)\n"); fflush(stderr);
            run_case("Dijkstra", "Binary", gd, &BINHEAP_OPS, 0);

            fprintf(stderr, "[progress] Dijkstra Fibonacci (dense)\n"); fflush(stderr);
            run_case("Dijkstra", "Fibonacci", gd, &fibheap_pq_ops, 0);

            fprintf(stderr, "[progress] Dijkstra Pairing (dense)\n"); fflush(stderr);
            run_case("Dijkstra", "Pairing", gd, &pairing_pq_ops, 0);

            fprintf(stderr, "[progress] Prim Binary (dense)\n"); fflush(stderr);
            run_case("Prim", "Binary", gd, &BINHEAP_OPS, 0);

            fprintf(stderr, "[progress] Prim Fibonacci (dense)\n"); fflush(stderr);
            run_case("Prim", "Fibonacci", gd, &fibheap_pq_ops, 0);

            fprintf(stderr, "[progress] Prim Pairing (dense)\n"); fflush(stderr);
            run_case("Prim", "Pairing", gd, &pairing_pq_ops, 0);

            fprintf(stderr, "[progress] destroying DENSE graph\n"); fflush(stderr);
            graph_destroy(gd);
        }
    }

    fprintf(stderr, "\n[progress] bench done\n");
    fflush(stderr);
    return 0;
}
