// MergeSort.h - stable merge sort (divide and conquer)
// Time: O(n log n) best/average/worst.  Space: O(n) for the temp buffer.
#pragma once

template <typename T, typename Less>
static void mergeRange(T* a, T* tmp, int lo, int mid, int hi, Less less) {
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        if (less(a[j], a[i])) tmp[k++] = a[j++];
        else                  tmp[k++] = a[i++];
    }
    while (i < mid) tmp[k++] = a[i++];
    while (j < hi)  tmp[k++] = a[j++];
    for (int x = lo; x < hi; ++x) a[x] = tmp[x];
}

template <typename T, typename Less>
static void mergeSortRec(T* a, T* tmp, int lo, int hi, Less less) {
    if (hi - lo < 2) return;             
    int mid = lo + (hi - lo) / 2;
    mergeSortRec(a, tmp, lo, mid, less); 
    mergeSortRec(a, tmp, mid, hi, less); 
    mergeRange(a, tmp, lo, mid, hi, less);
}

template <typename T, typename Less>
void mergeSort(T* a, int n, Less less) {
    if (n < 2) return;
    T* tmp = new T[n];
    mergeSortRec(a, tmp, 0, n, less);
    delete[] tmp;
}
