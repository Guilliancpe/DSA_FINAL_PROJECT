// CinemaArrays.h
// All the ARRAY parts of the simulation in one file.
//   PART A - parameter array
//     1. Item / ClassType   - small helper types
//     2. Params             - one person's record: class, food, drinks, total
//     3. param functions    - set class, add food/drink, recompute total
//     4. ParamArray<T>      - generic array (template <typename T>), used with T = Params
//   PART B - customer array
//     5. buildCustomerArray() - copies every finished Params into a new array
//     6. sortCustomers()      - merge sort by total spent (ID is the tiebreaker)
//     7. binarySearchCustomer() - finds ONE person by (total, ID)
//
// Needs CinemaSim.h (for Person). The BST and general tree will fill the
// Params records later.
 
#ifndef CINEMAARRAYS_H
#define CINEMAARRAYS_H
 
#include <iostream>
#include "CinemaSim.h"
 
// ======================================================
// Constants
// ======================================================
const int REGULAR_PRICE = 250;   // non-VIP ticket (left of the BST)
const int VIP_PRICE     = 500;   // VIP ticket (right of the BST)
const int MAX_ITEMS     = 5;     // max foods and max drinks one person can hold
 
enum ClassType { NOT_CHOSEN, REGULAR, VIP };
 
// one food or one drink that the person bought
struct Item {
    const char* name;   // example: "popcorn" (string literal, no delete needed)
    int price;          // pesos
};
 
// ======================================================
// Params: P = { Class, Food, Drinks, Money(total) }
// ======================================================
struct Params {
    int id;               // same ID as the Person (used later as the tiebreaker)
    int startMoney;       // money the person started with
    ClassType cls;        // REGULAR or VIP
    Item foods[MAX_ITEMS];
    int foodCount;
    Item drinks[MAX_ITEMS];
    int drinkCount;
    int total;            // total spent = class price + food + drinks
};
 
// ======================================================
// Helpers
// ======================================================
int classPrice(ClassType c) {
    if (c == REGULAR) return REGULAR_PRICE;
    if (c == VIP)     return VIP_PRICE;
    return 0;   // NOT_CHOSEN
}
 
const char* className(ClassType c) {
    if (c == REGULAR) return "Regular";
    if (c == VIP)     return "VIP";
    return "None";
}
 
// money the person still has left
int moneyLeft(const Params& p) {
    return p.startMoney - p.total;
}
 
// total = class price + all foods + all drinks
void recalcTotal(Params& p) {
    int sum = classPrice(p.cls);
    for (int i = 0; i < p.foodCount; i++)  sum += p.foods[i].price;
    for (int i = 0; i < p.drinkCount; i++) sum += p.drinks[i].price;
    p.total = sum;
}
 
// sets up an empty record from a Person (nothing chosen yet)
void initParams(Params& p, const Person& person) {
    p.id = person.id;
    p.startMoney = person.money;
    p.cls = NOT_CHOSEN;
    p.foodCount = 0;
    p.drinkCount = 0;
    p.total = 0;
}
 
// ======================================================
// Filling a record (called by the BST / general tree later)
// Each returns false if the choice is not allowed.
// ======================================================
 
// the person can only pick a class if they can afford its ticket
bool setClass(Params& p, ClassType c) {
    if (c == NOT_CHOSEN) return false;
    if (classPrice(c) > p.startMoney) return false;
    p.cls = c;
    recalcTotal(p);
    return true;
}
 
// the item must fit the money left AND there must be a free slot
bool addFood(Params& p, const char* name, int price) {
    if (p.cls == NOT_CHOSEN) return false;       // ticket comes first
    if (p.foodCount >= MAX_ITEMS) return false;
    if (price > moneyLeft(p)) return false;
    p.foods[p.foodCount].name = name;
    p.foods[p.foodCount].price = price;
    p.foodCount++;
    recalcTotal(p);
    return true;
}
 
bool addDrink(Params& p, const char* name, int price) {
    if (p.cls == NOT_CHOSEN) return false;
    if (p.drinkCount >= MAX_ITEMS) return false;
    if (price > moneyLeft(p)) return false;
    p.drinks[p.drinkCount].name = name;
    p.drinks[p.drinkCount].price = price;
    p.drinkCount++;
    recalcTotal(p);
    return true;
}
 
// ======================================================
// Printing
// ======================================================
std::ostream& operator<<(std::ostream& out, const Params& p) {
    out << "[ID " << p.id << " | " << className(p.cls) << " | Food: ";
    if (p.foodCount == 0) out << "none";
    for (int i = 0; i < p.foodCount; i++) {
        out << p.foods[i].name << (i < p.foodCount - 1 ? ", " : "");
    }
    out << " | Drinks: ";
    if (p.drinkCount == 0) out << "none";
    for (int i = 0; i < p.drinkCount; i++) {
        out << p.drinks[i].name << (i < p.drinkCount - 1 ? ", " : "");
    }
    out << " | Total: P" << p.total << "]";
    return out;
}
 
// ======================================================
// The parameter array: FinEL[queueInput]  (generic, template <typename T>)
// Same idea as qNode<T>: the container does not care what T is.
// For this project T = Params, but it works with any type that has a default
// constructor (and operator<< if you want to display it).
// ======================================================
template <typename T>
class ParamArray {
public:
    T* data;    // the slots
    int size;   // number of slots (= queue input)
};
 
// makes an array with n empty slots. Free it with destroyParamArray().
template <typename T>
ParamArray<T>* createParamArray(int n) {
    ParamArray<T>* arr = new ParamArray<T>;
    arr->data = new T[n];
    arr->size = n;
    return arr;
}
 
template <typename T>
void destroyParamArray(ParamArray<T>* arr) {
    if (arr == nullptr) return;
    delete[] arr->data;
    delete arr;
}
 
// returns the slot at index (0 to size-1)
template <typename T>
T& slotAt(ParamArray<T>* arr, int index) {
    return arr->data[index];
}
 
// prints the first "count" slots
template <typename T>
void displayParamArray(const ParamArray<T>* arr, int count) {
    if (count > arr->size) count = arr->size;
    for (int i = 0; i < count; i++) {
        std::cout << arr->data[i] << std::endl;
    }
}
 
// ======================================================
// Params-specific: set up every slot from the people.
// Slot = ID - 1, so every person always writes to their OWN slot.
// That also keeps multithreading safe later (no two threads share a slot,
// so no lock is needed for this array).
// ======================================================
void initFromPeople(ParamArray<Params>* arr, const Person* people, int n) {
    for (int i = 0; i < n; i++) {
        initParams(arr->data[people[i].id - 1], people[i]);
    }
}
 
// ======================================================
// PART B: CUSTOMER ARRAY
// Holds every finished Params (P1 to Pn). It is a COPY of the parameter array,
// because sorting moves the records around and the parameter array must keep
// slot = ID - 1.
// ======================================================
typedef ParamArray<Params> CustomerArray;
 
// copies every record from the parameter array. Free it with destroyParamArray().
CustomerArray* buildCustomerArray(const ParamArray<Params>* src) {
    CustomerArray* cust = createParamArray<Params>(src->size);
    for (int i = 0; i < src->size; i++) {
        cust->data[i] = src->data[i];
    }
    return cust;
}
 
// ------------------------------------------------------
// Merge sort by total spent (least to highest)
// Same total: the smaller ID goes first, so every (total, ID) key is unique
// ------------------------------------------------------
bool customerLessOrEqual(const Params& a, const Params& b) {
    if (a.total != b.total) return a.total < b.total;
    return a.id <= b.id;
}
 
void mergeCustomers(Params* arr, Params* temp, int left, int mid, int right) {
    int i = left;      // start of left half
    int j = mid + 1;   // start of right half
    int k = left;      // position in temp
 
    while (i <= mid && j <= right) {
        if (customerLessOrEqual(arr[i], arr[j])) temp[k++] = arr[i++];
        else                                     temp[k++] = arr[j++];
    }
    while (i <= mid)   temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
 
    for (int x = left; x <= right; x++) arr[x] = temp[x];
}
 
void mergeSortCustomersHelper(Params* arr, Params* temp, int left, int right) {
    if (left >= right) return;              // 0 or 1 element: already sorted
    int mid = left + (right - left) / 2;
    mergeSortCustomersHelper(arr, temp, left, mid);
    mergeSortCustomersHelper(arr, temp, mid + 1, right);
    mergeCustomers(arr, temp, left, mid, right);
}
 
// call this AFTER everyone finished the trees
void sortCustomers(CustomerArray* cust) {
    if (cust->size <= 1) return;
    Params* temp = new Params[cust->size];
    mergeSortCustomersHelper(cust->data, temp, 0, cust->size - 1);
    delete[] temp;
}
 
// ------------------------------------------------------
// Binary search (only works if sortCustomers() was called first)
// Looks for the person with this exact total AND id.
// Returns the index in the array, or -1 if not found.
// ------------------------------------------------------
int binarySearchCustomer(const CustomerArray* cust, int total, int id) {
    int low = 0;
    int high = cust->size - 1;
 
    while (low <= high) {
        int mid = low + (high - low) / 2;
        const Params& m = cust->data[mid];
 
        if (m.total == total && m.id == id) return mid;   // found
 
        // compare the (total, ID) keys to pick a half
        bool midIsSmaller = (m.total < total) || (m.total == total && m.id < id);
        if (midIsSmaller) low = mid + 1;
        else              high = mid - 1;
    }
    return -1;   // not found
}
 
#endif // CINEMAARRAYS_H