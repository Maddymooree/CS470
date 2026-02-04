// pq.h
#ifndef PQ_H
#define PQ_H

#include <stddef.h>

typedef struct PQ PQ;           // opaque
typedef struct PQHandle PQHandle; // opaque handle type

typedef struct {
    long long key;  // priority (dist for Dijkstra, edge weight key for Prim)
    int value;      // vertex id
} PQItem;

// Function table (like an interface in C)
typedef struct {
    PQ*      (*create)(int capacity);
    void     (*destroy)(PQ* pq);

    PQHandle*(*insert)(PQ* pq, long long key, int value); // returns handle for decrease-key
    PQItem   (*extract_min)(PQ* pq);                      // removes/returns min
    void     (*decrease_key)(PQ* pq, PQHandle* h, long long new_key);

    int      (*is_empty)(PQ* pq);
} PQOps;

#endif
