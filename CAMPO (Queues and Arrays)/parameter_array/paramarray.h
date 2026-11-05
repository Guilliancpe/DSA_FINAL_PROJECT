#ifndef PARAMARRAY_H
#define PARAMARRAY_H
 
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
// Helpers that only need the class type
// (C is deduced from the argument, so C = ClassType)
// ======================================================
template <typename C>
int classPrice(C c) {
    if (c == REGULAR) return REGULAR_PRICE;
    if (c == VIP)     return VIP_PRICE;
    return 0;   // NOT_CHOSEN
}
 
template <typename C>
const char* className(C c) {
    if (c == REGULAR) return "Regular";
    if (c == VIP)     return "VIP";
    return "None";
}
 
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
 
    // Printing: written inside the struct so it needs no static or inline
    friend std::ostream& operator<<(std::ostream& out, const Params& p) {
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
};
 
// ======================================================
// Helpers (T = Params)
// ======================================================
 
// money the person still has left
template <typename T>
int moneyLeft(const T& p) {
    return p.startMoney - p.total;
}
 
// total = class price + all foods + all drinks
template <typename T>
void recalcTotal(T& p) {
    int sum = classPrice(p.cls);
    for (int i = 0; i < p.foodCount; i++)  sum += p.foods[i].price;
    for (int i = 0; i < p.drinkCount; i++) sum += p.drinks[i].price;
    p.total = sum;
}
 
// sets up an empty record from a Person (nothing chosen yet)
template <typename T>
void initParams(T& p, const Person& person) {
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
template <typename T>
bool setClass(T& p, ClassType c) {
    if (c == NOT_CHOSEN) return false;
    if (classPrice(c) > p.startMoney) return false;
    p.cls = c;
    recalcTotal(p);
    return true;
}
 
// the item must fit the money left AND there must be a free slot
template <typename T>
bool addFood(T& p, const char* name, int price) {
    if (p.cls == NOT_CHOSEN) return false;       // ticket comes first
    if (p.foodCount >= MAX_ITEMS) return false;
    if (price > moneyLeft(p)) return false;
    p.foods[p.foodCount].name = name;
    p.foods[p.foodCount].price = price;
    p.foodCount++;
    recalcTotal(p);
    return true;
}
 
template <typename T>
bool addDrink(T& p, const char* name, int price) {
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
 
// prints the first "count" records of the array
template <typename T>
void displayParamArray(const T* arr, int count) {
    for (int i = 0; i < count; i++) {
        std::cout << arr[i] << std::endl;
    }
}
 
// ======================================================
// The parameter array: FinEL[queueInput]
// Size = number of people from the queue input.
// Slot = ID - 1, so every person always writes to their OWN slot.
// That also keeps multithreading safe later (no two threads share a slot,
// so no lock is needed for this array).
// T can't be guessed from the arguments, so call it like this:
//     Params* arr = createParamArray<Params>(people, n);
// The caller must delete[] it.
// ======================================================
template <typename T>
T* createParamArray(const Person* people, int n) {
    T* arr = new T[n];
    for (int i = 0; i < n; i++) {
        initParams(arr[people[i].id - 1], people[i]);
    }
    return arr;
}
 
#endif // PARAMARRAY_H