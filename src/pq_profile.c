#include "pq_profile.h"
#include <string.h>

#ifdef _WIN32
#include <windows.h>
static uint64_t now_ns(void) {
    static LARGE_INTEGER freq;
    static int init = 0;
    if (!init) { QueryPerformanceFrequency(&freq); init = 1; }
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    // convert ticks to ns
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

typedef struct {
    PQProfile *prof;
    PQ *inner;
} ProfilePQ;

static PQ *p_create(int cap);
static void p_destroy(PQ *pq);
static PQHandle *p_insert(PQ *pq, long long key, int value);
static PQItem p_extract_min(PQ *pq);
static void p_decrease_key(PQ *pq, PQHandle *h, long long new_key);
static int p_is_empty(PQ *pq);

static PQOps WRAP_OPS = {
    .create = p_create,
    .destroy = p_destroy,
    .insert = p_insert,
    .extract_min = p_extract_min,
    .decrease_key = p_decrease_key,
    .is_empty = p_is_empty
};

static const PQOps *G_BASE = NULL;
static PQProfile *G_PROF = NULL;

const PQOps *pq_profile_ops(const PQOps *base_ops, PQProfile *prof) {
    G_BASE = base_ops;
    G_PROF = prof;
    prof->base = base_ops;
    return &WRAP_OPS;
}

void pq_profile_reset(PQProfile *prof) {
    memset(prof, 0, sizeof(*prof));
}

static PQ *p_create(int cap) {
    ProfilePQ *ppq = (ProfilePQ*)malloc(sizeof(ProfilePQ));
    ppq->prof = G_PROF;
    ppq->inner = G_BASE->create(cap);
    return (PQ*)ppq;
}

static void p_destroy(PQ *pq) {
    ProfilePQ *ppq = (ProfilePQ*)pq;
    G_BASE->destroy(ppq->inner);
    free(ppq);
}

static PQHandle *p_insert(PQ *pq, long long key, int value) {
    ProfilePQ *ppq = (ProfilePQ*)pq;
    uint64_t t0 = now_ns();
    PQHandle *h = G_BASE->insert(ppq->inner, key, value);
    uint64_t t1 = now_ns();
    ppq->prof->insert_count++;
    ppq->prof->insert_ns += (t1 - t0);
    return h;
}

static PQItem p_extract_min(PQ *pq) {
    ProfilePQ *ppq = (ProfilePQ*)pq;
    uint64_t t0 = now_ns();
    PQItem it = G_BASE->extract_min(ppq->inner);
    uint64_t t1 = now_ns();
    ppq->prof->extract_count++;
    ppq->prof->extract_ns += (t1 - t0);
    return it;
}

static void p_decrease_key(PQ *pq, PQHandle *h, long long new_key) {
    ProfilePQ *ppq = (ProfilePQ*)pq;
    uint64_t t0 = now_ns();
    G_BASE->decrease_key(ppq->inner, h, new_key);
    uint64_t t1 = now_ns();
    ppq->prof->decrease_count++;
    ppq->prof->decrease_ns += (t1 - t0);
}

static int p_is_empty(PQ *pq) {
    ProfilePQ *ppq = (ProfilePQ*)pq;
    return G_BASE->is_empty(ppq->inner);
}
