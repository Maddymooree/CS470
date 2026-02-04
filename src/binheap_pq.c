// binheap_pq.c
#include "binheap_pq.h"
#include <stdlib.h>
#include <stdio.h>

struct PQHandle {
    long long key;
    int value;
    int idx; // position in heap array
};

struct PQ {
    int size;
    int cap;
    PQHandle** a; // heap array of pointers to handles
};

static void swap_nodes(PQ* pq, int i, int j) {
    PQHandle* tmp = pq->a[i];
    pq->a[i] = pq->a[j];
    pq->a[j] = tmp;
    pq->a[i]->idx = i;
    pq->a[j]->idx = j;
}

static void sift_up(PQ* pq, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (pq->a[p]->key <= pq->a[i]->key) break;
        swap_nodes(pq, p, i);
        i = p;
    }
}

static void sift_down(PQ* pq, int i) {
    while (1) {
        int l = 2*i + 1;
        int r = 2*i + 2;
        int m = i;
        if (l < pq->size && pq->a[l]->key < pq->a[m]->key) m = l;
        if (r < pq->size && pq->a[r]->key < pq->a[m]->key) m = r;
        if (m == i) break;
        swap_nodes(pq, i, m);
        i = m;
    }
}

static PQ* bh_create(int capacity) {
    PQ* pq = (PQ*)malloc(sizeof(PQ));
    pq->size = 0;
    pq->cap = capacity > 0 ? capacity : 1;
    pq->a = (PQHandle**)malloc(sizeof(PQHandle*) * pq->cap);
    return pq;
}

static void bh_destroy(PQ* pq) {
    // handles were allocated in insert, free them
    for (int i = 0; i < pq->size; i++) free(pq->a[i]);
    free(pq->a);
    free(pq);
}

static PQHandle* bh_insert(PQ* pq, long long key, int value) {
    if (pq->size == pq->cap) {
        pq->cap *= 2;
        pq->a = (PQHandle**)realloc(pq->a, sizeof(PQHandle*) * pq->cap);
    }
    PQHandle* h = (PQHandle*)malloc(sizeof(PQHandle));
    h->key = key;
    h->value = value;
    h->idx = pq->size;
    pq->a[pq->size++] = h;
    sift_up(pq, h->idx);
    return h;
}

static PQItem bh_extract_min(PQ* pq) {
    if (pq->size == 0) {
        fprintf(stderr, "extract_min on empty heap\n");
        exit(1);
    }
    PQHandle* root = pq->a[0];
    PQItem out = { root->key, root->value };

    pq->size--;
    if (pq->size > 0) {
        pq->a[0] = pq->a[pq->size];
        pq->a[0]->idx = 0;
        sift_down(pq, 0);
    }
    free(root);
    return out;
}

static void bh_decrease_key(PQ* pq, PQHandle* h, long long new_key) {
    if (new_key > h->key) {
        fprintf(stderr, "decrease_key called with larger key\n");
        exit(1);
    }
    h->key = new_key;
    sift_up(pq, h->idx);
}

static int bh_is_empty(PQ* pq) {
    return pq->size == 0;
}

const PQOps BINHEAP_OPS = {
    .create = bh_create,
    .destroy = bh_destroy,
    .insert = bh_insert,
    .extract_min = bh_extract_min,
    .decrease_key = bh_decrease_key,
    .is_empty = bh_is_empty
};
