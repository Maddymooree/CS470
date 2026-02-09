# Priority Queues in Graph Algorithms

This project implements Dijkstra’s Shortest Path Algorithm and Prim’s Minimum Spanning Tree Algorithm using interchangeable priority queue implementations.

## Overview
The goal of this project is to compare advanced priority queue data structures (Fibonacci heaps and pairing heaps) in graph algorithms and evaluate how theoretical complexity compares to practical performance.

## Structure
- `src/pq.h` – Abstract priority queue interface
- `src/graph.*` – Graph representation (adjacency list)
- `src/dijkstra.*` – Heap-agnostic Dijkstra implementation
- `src/prim.*` – Heap-agnostic Prim implementation
- `src/binheap_pq.*` – Binary heap baseline implementation
- `paring_pq.*` – Pairing heap implementation


## Modularity
All graph algorithms are implemented against an abstract priority queue interface, allowing different heap implementations to be swapped without modifying algorithm logic.

## Building
Example compilation command:
```bash
gcc src/*.c -o graph_algos
