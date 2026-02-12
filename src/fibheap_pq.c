// src/fibheap_pq.c
#include "pq.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

typedef struct FibNode {
    long long key;
    int value;

    int degree;
    int mark;

    struct FibNode *parent;
    struct FibNode *child;
    struct FibNode *left;
    struct FibNode *right;
} FibNode;

struct PQ {
    FibNode *min;
    int n;
};

static FibNode *fib_node_create(long long key, int value) {
    FibNode *x = (FibNode *)malloc(sizeof(FibNode));
    if (!x) return NULL;

    x->key = key;
    x->value = value;
    x->degree = 0;
    x->mark = 0;
    x->parent = NULL;
    x->child = NULL;
    x->left = x;
    x->right = x;

    return x;
}

static void fib_isolate(FibNode *x) {
    x->left->right = x->right;
    x->right->left = x->left;
    x->left = x->right = x;
}

static void fib_splice_into_root_list(PQ *pq, FibNode *x) {
    x->right = pq->min->right;
    x->left  = pq->min;
    pq->min->right->left = x;
    pq->min->right = x;
}

static void fib_add_root(PQ *pq, FibNode *x) {
    fib_isolate(x);
    x->parent = NULL;

    if (!pq->min) {
        pq->min = x;
        x->left = x->right = x;
    } else {
        fib_splice_into_root_list(pq, x);
        if (x->key < pq->min->key)
            pq->min = x;
    }
}

static void fib_link(PQ *pq, FibNode *y, FibNode *x) {
    (void)pq;
    fib_isolate(y);
    y->parent = x;
    y->mark = 0;

    if (!x->child) {
        x->child = y;
        y->left = y->right = y;
    } else {
        y->right = x->child->right;
        y->left  = x->child;
        x->child->right->left = y;
        x->child->right = y;
    }
    x->degree++;
}

static void fib_consolidate(PQ *pq) {
    if (!pq->min) return;

    // Count roots with a guard
    int root_count = 0;
    FibNode *start = pq->min;
    FibNode *w = start;

    int guard = 0;
    int guard_limit = pq->n + 10;
    do {
        root_count++;
        w = w->right;
        if (++guard > guard_limit) {
            fprintf(stderr, "FIB ERROR: root list loop during count (n=%d)\n", pq->n);
            abort();
        }
    } while (w != start);

    FibNode **roots = (FibNode **)malloc(sizeof(FibNode*) * (size_t)root_count);
    if (!roots) return;

    w = start;
    for (int i = 0; i < root_count; i++) {
        roots[i] = w;
        w = w->right;
    }

    int D = 64;
    FibNode **A = (FibNode **)calloc((size_t)D, sizeof(FibNode*));
    if (!A) { free(roots); return; }

    pq->min = NULL;

    for (int i = 0; i < root_count; i++) {
        FibNode *x = roots[i];

        x->left = x->right = x;
        x->parent = NULL;

        int d = x->degree;
        int local_guard = 0;
        while (A[d]) {
            FibNode *y = A[d];
            if (x->key > y->key) {
                FibNode *tmp = x;
                x = y;
                y = tmp;
            }
            fib_link(pq, y, x);
            A[d] = NULL;
            d++;

            if (++local_guard > pq->n + 10) {
                fprintf(stderr, "FIB ERROR: consolidate link loop (n=%d)\n", pq->n);
                abort();
            }
            if (d >= D) {
                fprintf(stderr, "FIB ERROR: degree overflow in consolidate (d=%d)\n", d);
                abort();
            }
        }
        A[d] = x;
    }

    for (int i = 0; i < D; i++) {
        if (A[i]) {
            FibNode *x = A[i];
            x->left = x->right = x;
            x->parent = NULL;

            if (!pq->min) {
                pq->min = x;
                x->left = x->right = x;
            } else {
                fib_splice_into_root_list(pq, x);
                if (x->key < pq->min->key) pq->min = x;
            }
        }
    }

    free(roots);
    free(A);
}

static void fib_cut(PQ *pq, FibNode *x, FibNode *y) {
    if (y->child == x) {
        if (x->right != x) y->child = x->right;
        else y->child = NULL;
    }
    fib_isolate(x);
    y->degree--;

    fib_add_root(pq, x);
    x->parent = NULL;
    x->mark = 0;
}

static void fib_cascading_cut(PQ *pq, FibNode *y) {
    FibNode *z = y->parent;
    if (z) {
        if (!y->mark) y->mark = 1;
        else {
            fib_cut(pq, y, z);
            fib_cascading_cut(pq, z);
        }
    }
}

/* PQOps */

static PQ *fibheap_create(int capacity) {
    (void)capacity;
    PQ *pq = (PQ *)malloc(sizeof(PQ));
    if (!pq) return NULL;
    pq->min = NULL;
    pq->n = 0;
    return pq;
}

static void fibheap_destroy(PQ *pq) {
    free(pq);
}

static PQHandle *fibheap_insert(PQ *pq, long long key, int value) {
    FibNode *x = fib_node_create(key, value);
    if (!x) return NULL;
    fib_add_root(pq, x);
    pq->n++;
    return (PQHandle *)x;
}

static PQItem fibheap_extract_min(PQ *pq) {
    PQItem item = (PQItem){0, 0};
    FibNode *z = pq->min;
    if (!z) return item;

    item.key = z->key;
    item.value = z->value;

    if (z->child) {
        // Count children with guard, then move them safely
        int child_count = 0;
        FibNode *startc = z->child;
        FibNode *c = startc;

        int guard = 0;
        int guard_limit = pq->n + 10;
        do {
            child_count++;
            c = c->right;
            if (++guard > guard_limit) {
                fprintf(stderr, "FIB ERROR: child list loop during count (n=%d)\n", pq->n);
                abort();
            }
        } while (c != startc);

        // Collect children
        FibNode **kids = (FibNode **)malloc(sizeof(FibNode*) * (size_t)child_count);
        if (!kids) abort();

        c = startc;
        for (int i = 0; i < child_count; i++) {
            kids[i] = c;
            c = c->right;
        }

        // Detach and add each child to root list
        for (int i = 0; i < child_count; i++) {
            FibNode *x = kids[i];
            fib_isolate(x);
            x->parent = NULL;
            x->mark = 0;

            if (!pq->min) {
                pq->min = x;
                x->left = x->right = x;
            } else {
                fib_splice_into_root_list(pq, x);
                if (x->key < pq->min->key) pq->min = x;
            }
        }

        free(kids);
        z->child = NULL;
    }

    FibNode *z_next = z->right;

    if (z_next == z) {
        pq->min = NULL;
    } else {
        z->left->right = z->right;
        z->right->left = z->left;
        pq->min = z_next;
        fib_consolidate(pq);
    }

    z->left = z->right = z;
    free(z);
    pq->n--;

    return item;
}

static void fibheap_decrease_key(PQ *pq, PQHandle *h, long long new_key) {
    FibNode *x = (FibNode *)h;
    if (!pq || !x) {
        fprintf(stderr, "fibheap_decrease_key: null pq or handle\n");
        abort();
    }
    if (new_key > x->key) return;

    x->key = new_key;
    FibNode *y = x->parent;

    if (y && x->key < y->key) {
        fib_cut(pq, x, y);
        fib_cascading_cut(pq, y);
    }

    if (!pq->min || x->key < pq->min->key)
        pq->min = x;
}

static int fibheap_is_empty(PQ *pq) {
    return pq->min == NULL;
}

PQOps fibheap_pq_ops = {
    .create       = fibheap_create,
    .destroy      = fibheap_destroy,
    .insert       = fibheap_insert,
    .extract_min  = fibheap_extract_min,
    .decrease_key = fibheap_decrease_key,
    .is_empty     = fibheap_is_empty
};
