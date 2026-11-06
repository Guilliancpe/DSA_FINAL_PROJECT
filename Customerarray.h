#ifndef CUSTOMERARRAY_H
#define CUSTOMERARRAY_H
 
#include <iostream>
#include "ParamArray.h"
 
// ======================================================
// Merge sort, by (total, id)
// ======================================================
template <typename T>
bool paramLessOrEqual(const T& a, const T& b) {
    if (a.total != b.total) return a.total < b.total;
    return a.id <= b.id;
}
 
template <typename T>
void mergeParams(T* arr, T* temp, int left, int mid, int right) {
    int i = left;      // start of left half
    int j = mid + 1;   // start of right half
    int k = left;      // position in temp
 
    while (i <= mid && j <= right) {
        if (paramLessOrEqual(arr[i], arr[j])) temp[k++] = arr[i++];
        else                                  temp[k++] = arr[j++];
    }
    while (i <= mid)   temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
 
    for (int x = left; x <= right; x++) arr[x] = temp[x];
}
 
template <typename T>
void mergeSortParamsHelper(T* arr, T* temp, int left, int right) {
    if (left >= right) return;              // 0 or 1 element: already sorted
    int mid = left + (right - left) / 2;
    mergeSortParamsHelper(arr, temp, left, mid);
    mergeSortParamsHelper(arr, temp, mid + 1, right);
    mergeParams(arr, temp, left, mid, right);
}
 
template <typename T>
void mergeSortParams(T* arr, int n) {
    if (n <= 1) return;
    T* temp = new T[n];
    mergeSortParamsHelper(arr, temp, 0, n - 1);
    delete[] temp;
}
 
// ======================================================
// The customer array
// A COPY of the parameter array, so the parameter array (slot = ID - 1)
// stays untouched while the customer array gets sorted.
// Call this AFTER everyone has finished the tree.
// The caller must delete[] it.
// ======================================================
template <typename T>
T* createCustomerArray(const T* paramArr, int n) {
    T* customers = new T[n];
    for (int i = 0; i < n; i++) {
        customers[i] = paramArr[i];
    }
    return customers;
}
 
// copy, then sort by total spent (ID is the tiebreaker)
template <typename T>
T* createSortedCustomerArray(const T* paramArr, int n) {
    T* customers = createCustomerArray(paramArr, n);
    mergeSortParams(customers, n);
    return customers;
}
 
// ======================================================
// Binary search
// The array must already be sorted by (total, id).
// Returns the index of the person, or -1 if not found.
// ======================================================
template <typename T>
int binarySearch(const T* customers, int n, int total, int id) {
    int low = 0;
    int high = n - 1;
 
    while (low <= high) {
        int mid = low + (high - low) / 2;
 
        if (customers[mid].total == total && customers[mid].id == id) {
            return mid;   // found
        }
 
        // is the key (total, id) before the middle person?
        bool keyIsBefore = (total < customers[mid].total) ||
                           (total == customers[mid].total && id < customers[mid].id);
 
        if (keyIsBefore) high = mid - 1;   // look in the left half
        else             low  = mid + 1;   // look in the right half
    }
    return -1;   // not found
}
 
// search for the person and display their parameters
template <typename T>
bool searchAndDisplay(const T* customers, int n, int total, int id) {
    int index = binarySearch(customers, n, total, id);
 
    if (index == -1) {
        std::cout << "Customer ID " << id << " with total P" << total << " not found \n";
        return false;
    }
 
    std::cout << "Found at index " << index << ": " << customers[index] << std::endl;
    return true;
}
 
#endif // CUSTOMERARRAY_H