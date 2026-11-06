
#ifndef CINEMASIM_H
#define CINEMASIM_H
 
#include <iostream>
#include <cstdlib>   // rand()
 
// ======================================================
// Person: what the queue holds
// ======================================================
struct Person {
    int id;     // unique ID, assigned when the person is generated
    int money;  // random starting money (pesos), their "wallet"
};
 
// lets display() print a Person using cout
std::ostream& operator<<(std::ostream& out, const Person& p) {
    out << "[ID " << p.id << ", P" << p.money << "]";
    return out;
}
 
// ======================================================
// Random generation
// srand(time(0)) is called ONCE in int main, not in here
// ======================================================
const int MIN_MONEY = 250;   // cheapest ticket (non-VIP)
const int MAX_MONEY = 1000;
 
// Returns a new[] array of n people. The caller must delete[] it.
Person* generatePeople(int n) {
    Person* people = new Person[n];
    for (int i = 0; i < n; i++) {
        people[i].id = i + 1;
        // modulo acts as the limit: (rand() % 751) gives 0-750, adding 250 gives 250-1000
        people[i].money = (rand() % (MAX_MONEY - MIN_MONEY + 1)) + MIN_MONEY;
    }
    return people;
}
 
// ======================================================
// Merge sort (least to highest money)
// If two people have the same money, the smaller ID goes first
// ======================================================
bool personLessOrEqual(const Person& a, const Person& b) {
    if (a.money != b.money) return a.money < b.money;
    return a.id <= b.id;
}
 
void merge(Person* arr, Person* temp, int left, int mid, int right) {
    int i = left;      // start of left half
    int j = mid + 1;   // start of right half
    int k = left;      // position in temp
 
    while (i <= mid && j <= right) {
        if (personLessOrEqual(arr[i], arr[j])) temp[k++] = arr[i++];
        else                                   temp[k++] = arr[j++];
    }
    while (i <= mid)   temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
 
    for (int x = left; x <= right; x++) arr[x] = temp[x];
}
 
void mergeSortHelper(Person* arr, Person* temp, int left, int right) {
    if (left >= right) return;              // 0 or 1 element: already sorted
    int mid = left + (right - left) / 2;
    mergeSortHelper(arr, temp, left, mid);
    mergeSortHelper(arr, temp, mid + 1, right);
    merge(arr, temp, left, mid, right);
}
 
void mergeSort(Person* arr, int n) {
    if (n <= 1) return;
    Person* temp = new Person[n];
    mergeSortHelper(arr, temp, 0, n - 1);
    delete[] temp;
}
 
// ======================================================
// QUEUE (linked list), same structure as the sample code
// frontPtr and backPtr are defined in int main
// ======================================================
template <typename T>
class qNode {
public:
    T data;
    qNode* next;
};
 
// creating a new node
template <typename T>
qNode<T>* new_node(T newData) {
    // allocates the new node
    qNode<T>* newNode = new qNode<T>;
    // sets the new data in the newNode's data
    newNode->data = newData;
    // points the next pointer of the newNode to nullptr
    newNode->next = nullptr;
    return newNode;
}
 
//===========
// enqueue
//===========
template <typename T>
void enqueue(qNode<T>** frontPtr, qNode<T>** backPtr, T newData) {
    // create a new node
    qNode<T>* newNode = new_node(newData);
 
    // inserting to an empty queue
    if ((*frontPtr) == nullptr && (*backPtr) == nullptr) {
        (*frontPtr) = newNode;
        (*backPtr) = newNode;
        return;   // stop here, otherwise the node would point to itself
    }
 
    // inserting an item into a non empty queue
    // point the backPtr next to the newNode
    (*backPtr)->next = newNode;
    (*backPtr) = newNode;
}
 
//============
// dequeueing
//============
template <typename T>
void dequeue(qNode<T>** frontPtr, qNode<T>** backPtr) {
    // nothing to remove
    if ((*frontPtr) == nullptr) {
        std::cout << "queue is empty \n";
        return;
    }
 
    // create a temporary node to store the node to be deleted
    qNode<T>* deleteNode = (*frontPtr);
 
    // check if the queue has only 1 node
    if ((*frontPtr)->next == nullptr) {
        (*frontPtr) = nullptr;
        (*backPtr) = nullptr;
        delete deleteNode;
        return;
    }
 
    // deleting of the node
    (*frontPtr) = deleteNode->next;
    delete deleteNode;
}
 
//=================================
// display all elements in the list
//=================================
template <typename T>
void display(qNode<T>* frontPtr) {
    if (frontPtr == nullptr) {
        std::cout << "queue is empty \n";
        return;
    }
 
    qNode<T>* current = frontPtr;
    while (current != nullptr) {
        std::cout << current->data << "->";
        current = current->next;
    }
    std::cout << "NULL \n";
}
 
//============================
// return the front variable
//============================
template <typename T>
T FrontVariable(qNode<T>* frontPtr) {
    if (frontPtr != nullptr) {
        return frontPtr->data;
    }
    return T();   // empty queue: return a default value
}
 
//============================
// helper: is the queue empty?
//============================
template <typename T>
bool isEmpty(qNode<T>* frontPtr) {
    return frontPtr == nullptr;
}
 
//=====================================================
// helper: put the whole (already sorted) array in the queue
//=====================================================
template <typename T>
void enqueueAll(qNode<T>** frontPtr, qNode<T>** backPtr, const T* arr, int n) {
    for (int i = 0; i < n; i++) {
        enqueue(frontPtr, backPtr, arr[i]);
    }
}
 
//=====================================================
// helper: delete every node (call this at the end of main)
//=====================================================
template <typename T>
void clearQueue(qNode<T>** frontPtr, qNode<T>** backPtr) {
    while ((*frontPtr) != nullptr) {
        dequeue(frontPtr, backPtr);
    }
}
 
#endif // CINEMASIM_H