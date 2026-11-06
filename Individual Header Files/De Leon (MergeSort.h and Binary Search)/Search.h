// Search.h - binary search (O(log n)) and linear search (O(n)) over a sorted array.
#pragma once

// a[] must be sorted ascending by keyOf(a[i]). Returns the index, or -1 if absent.
template <typename T, typename KeyFn>
int binarySearch(const T* a, int n, int key, KeyFn keyOf) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        int k = keyOf(a[mid]);
        if (k == key) return mid;
        if (k < key) lo = mid + 1;
        else         hi = mid - 1;
    }
    return -1;
}

template <typename T, typename KeyFn>
int linearSearch(const T* a, int n, int key, KeyFn keyOf) {
    for (int i = 0; i < n; ++i)
        if (keyOf(a[i]) == key) return i;
    return -1;
}
