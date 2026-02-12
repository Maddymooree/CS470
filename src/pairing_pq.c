#include "pairing_pq.h"
#include <stdlib.h>

typedef struct PairNode {
    long long key;
    int value;
    struct PairNode *child;
    struct PairNode *sibling;
    struct PairNode *prev;
} PairNode;

struct PQ {
    PairNode *root;
};

/* meld two heaps — smaller key wins and gets the other as child */
static PairNode *meld(PairNode *a, PairNode *b) {
    if (!a) return b;
    if (!b) return a;

    if (a->key > b->key) {
        PairNode *temp = a; a = b; b = temp;
    }

    b->sibling = a->child;
    if (a->child) a->child->prev = b;
    a->child = b;
    b->prev = a;

    return a;
}

/* two-pass pairing of sibling list (used after extract-min) */
static PairNode *two_pass_pair(PairNode *list) {
    if (!list || !list->sibling) return list;

    PairNode *pairs = NULL, *tail = NULL;

    // pass 1: pair adjacent ones
    while (list) {
        PairNode *a = list;
        list = a->sibling;
        a->sibling = NULL;

        if (!list) {
            if (!pairs) return a;
            tail->sibling = a;
            break;
        }

        PairNode *b = list;
        list = b->sibling;
        b->sibling = NULL;

        PairNode *m = meld(a, b);

        if (!pairs) {
            pairs = tail = m;
        } else {
            tail->sibling = m;
            tail = m;
        }
        tail->sibling = NULL;
    }

    // pass 2: merge pairs back together
    PairNode *result = NULL;
    PairNode *curr = pairs;
    while (curr) {
        PairNode *next = curr->sibling;
        curr->sibling = NULL;
        result = meld(curr, result);
        curr = next;
    }

    return result;
}

static PQ *pairing_create(int capacity) {
    (void)capacity;
    PQ *pq = malloc(sizeof(PQ));
    if (pq) pq->root = NULL;
    return pq;
}

static void free_heap(PairNode *node) {
    if (!node) return;
    free_heap(node->child);
    free_heap(node->sibling);
    free(node);
}

static void pairing_destroy(PQ *pq) {
    if (pq) {
        free_heap(pq->root);
        free(pq);
    }
}

static PQHandle *pairing_insert(PQ *pq, long long key, int value) {
    PairNode *n = malloc(sizeof(PairNode));
    if (!n) return NULL;

    n->key = key;
    n->value = value;
    n->child = n->sibling = n->prev = NULL;

    pq->root = meld(pq->root, n);
    return (PQHandle *)n;
}

static PQItem pairing_extract_min(PQ *pq) {
    PQItem item = {0, 0};
    if (!pq->root) return item;

    item.key = pq->root->key;
    item.value = pq->root->value;

    PairNode *kids = pq->root->child;
    if (kids) kids->prev = NULL;

    free(pq->root);
    pq->root = two_pass_pair(kids);

    return item;
}

static void pairing_decrease_key(PQ *pq, PQHandle *h, long long new_key) {
    PairNode *node = (PairNode *)h;
    if (new_key > node->key) return;

    node->key = new_key;

    if (!node->prev || node == pq->root) return;

    // cut node out
    if (node->prev->child == node) {
        node->prev->child = node->sibling;
        if (node->sibling) node->sibling->prev = node->prev;
    } else {
        node->prev->sibling = node->sibling;
        if (node->sibling) node->sibling->prev = node->prev;
    }

    node->sibling = node->prev = NULL;

    pq->root = meld(pq->root, node);
}

static int pairing_is_empty(PQ *pq) {
    return pq->root == NULL;
}

PQOps pairing_pq_ops = {
    .create       = pairing_create,
    .destroy      = pairing_destroy,
    .insert       = pairing_insert,
    .extract_min  = pairing_extract_min,
    .decrease_key = pairing_decrease_key,
    .is_empty     = pairing_is_empty
};