#pragma once
#include "pq.h"
#include <stdint.h>

typedef struct {
    const PQOps *base;     // underlying ops (bin/fib/pair)
    uint64_t insert_count;
    uint64_t extract_count;
    uint64_t decrease_count;

    uint64_t insert_ns;
    uint64_t extract_ns;
    uint64_t decrease_ns;
} PQProfile;

// Returns an ops table that wraps `base_ops` and records timing into `prof`.
// NOTE: prof must outlive the run.
const PQOps *pq_profile_ops(const PQOps *base_ops, PQProfile *prof);

// Reset counters/timers
void pq_profile_reset(PQProfile *prof);
