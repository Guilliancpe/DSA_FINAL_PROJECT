# RETADSS - REal Time Automatic Data Sorter System (CPE 010, Group 2)

Build and run:
    g++ -std=c++17 -O2 main.cpp -o retadss
    ./retadss

Files
- models.h          Person, Record, Tier
- MergeSort.h       stable merge sort (sorts customers by money before the queue)
- Queue.h           linked-list FIFO queue
- TierBST.h         BST: Regular (root) / VIP (right child), keyed by ticket price
- ConcessionTree.h  general weighted tree for food/drinks, recursive traversal
- Graph.h           undirected, unweighted graph: Day -- Movie -- Person (BFS, recursive DFS)
- Search.h          binary search (O(log n)) and linear search (O(n))
- main.cpp          pipeline, menu, reports, benchmark

Pipeline: generate -> merge sort -> enqueue -> dequeue into BST -> weighted tree -> Record[] -> graph -> binary search.
