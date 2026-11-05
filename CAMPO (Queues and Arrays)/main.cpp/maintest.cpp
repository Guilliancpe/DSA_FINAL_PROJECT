// main.cpp
// Tests CinemaSim.h (queue) together with CinemaArrays.h (parameter array,
// customer array, merge sort, binary search).
// The BST and general tree are not built yet, so here a person's class, food and
// drink are picked with simple rand() code just to fill the records.
 
#include <iostream>
#include <ctime>
#include "CinemaSim.h"
#include "CinemaArrays.h"
 
// small menu used only for this test (the real menu is still undecided)
const char* FOOD_NAMES[]  = {"popcorn", "chips", "fries"};
const int   FOOD_PRICES[] = {80, 60, 70};
const char* DRINK_NAMES[]  = {"water", "juice", "shake"};
const int   DRINK_PRICES[] = {30, 50, 90};
 
// stand-in for the BST + general tree: gives one person a class, food and drink
void fakeChoices(Params& p) {
    // VIP only if they can afford it, and even then it is a coin flip
    if (p.startMoney >= VIP_PRICE && rand() % 2 == 0) setClass(p, VIP);
    else                                              setClass(p, REGULAR);
 
    int f = rand() % 3;
    int d = rand() % 3;
    addFood(p, FOOD_NAMES[f], FOOD_PRICES[f]);    // skipped automatically if they can't afford it
    addDrink(p, DRINK_NAMES[d], DRINK_PRICES[d]);
}
 
// prints PASS or FAIL for one check, returns 1 if it failed
int check(const char* name, bool passed) {
    std::cout << (passed ? "  PASS: " : "  FAIL: ") << name << std::endl;
    return passed ? 0 : 1;
}
 
int main() {
 
    srand(time(0));   // seed, called once
    int failed = 0;
 
    //==================================
    // PART 1: small test, 8 people (you can read every line)
    //==================================
    std::cout << "PART 1: 8 people \n";
    int small = 8;
 
    Person* few = generatePeople(small);
    mergeSort(few, small);
 
    // queue
    qNode<Person>* front = nullptr;
    qNode<Person>* back = nullptr;
    enqueueAll(&front, &back, few, small);
    std::cout << "Queue (poorest first): ";
    display(front);
 
    // parameter array: dequeue one by one, fill each person's record
    ParamArray<Params>* params = createParamArray<Params>(small);
    initFromPeople(params, few, small);
 
    while (!isEmpty(front)) {
        Person current = FrontVariable(front);
        dequeue(&front, &back);
        fakeChoices(slotAt(params, current.id - 1));
    }
    std::cout << "\nParameter array (slot = ID - 1):\n";
    displayParamArray(params, small);
 
    // customer array: copy, sort by total spent
    CustomerArray* cust = buildCustomerArray(params);
    sortCustomers(cust);
    std::cout << "\nCustomer array sorted by total spent:\n";
    displayParamArray(cust, small);
 
    // binary search for the 3rd person in the sorted list
    Params target = cust->data[2];
    int idx = binarySearchCustomer(cust, target.total, target.id);
    std::cout << "\nSearching ID " << target.id << " with total P" << target.total << " -> index " << idx << std::endl;
    if (idx != -1) std::cout << cust->data[idx] << std::endl;
 
    destroyParamArray(cust);
    destroyParamArray(params);
    delete[] few;
 
 
    //==================================
    // PART 2: 1000 people, automatic checks
    //==================================
    std::cout << "\nPART 2: 1000 people \n";
    int n = 1000;
 
    Person* people = generatePeople(n);
    mergeSort(people, n);
    qNode<Person>* front2 = nullptr;
    qNode<Person>* back2 = nullptr;
    enqueueAll(&front2, &back2, people, n);
 
    ParamArray<Params>* params2 = createParamArray<Params>(n);
    initFromPeople(params2, people, n);
 
    int served = 0;
    while (!isEmpty(front2)) {
        Person current = FrontVariable(front2);
        dequeue(&front2, &back2);
        fakeChoices(slotAt(params2, current.id - 1));
        served++;
    }
 
    CustomerArray* cust2 = buildCustomerArray(params2);
    sortCustomers(cust2);
 
    // check 1: everyone went through the queue
    failed += check("all 1000 people were served", served == n);
 
    // check 2: nobody spent more than they had, and the total adds up
    bool moneyOk = true;
    bool totalOk = true;
    for (int i = 0; i < n; i++) {
        const Params& p = params2->data[i];
        if (p.total > p.startMoney) moneyOk = false;
        int sum = classPrice(p.cls);
        for (int j = 0; j < p.foodCount; j++)  sum += p.foods[j].price;
        for (int j = 0; j < p.drinkCount; j++) sum += p.drinks[j].price;
        if (sum != p.total) totalOk = false;
    }
    failed += check("nobody spent more than their starting money", moneyOk);
    failed += check("total = class + food + drinks for everyone", totalOk);
 
    // check 3: parameter array keeps slot = ID - 1 (sorting must not touch it)
    bool slotsOk = true;
    for (int i = 0; i < n; i++) {
        if (params2->data[i].id != i + 1) slotsOk = false;
    }
    failed += check("parameter array slot = ID - 1", slotsOk);
 
    // check 4: customer array is sorted by (total, ID)
    bool sortedOk = true;
    for (int i = 1; i < cust2->size; i++) {
        if (!customerLessOrEqual(cust2->data[i - 1], cust2->data[i])) sortedOk = false;
    }
    failed += check("customer array sorted by total (ID tiebreaker)", sortedOk);
 
    // check 5: binary search finds every single person at the right spot
    bool foundAll = true;
    for (int i = 0; i < n; i++) {
        int found = binarySearchCustomer(cust2, cust2->data[i].total, cust2->data[i].id);
        if (found != i) foundAll = false;
    }
    failed += check("binary search finds all 1000 people", foundAll);
 
    // check 6: binary search says -1 for people who do not exist
    bool missingOk = (binarySearchCustomer(cust2, 300, 99999) == -1)
                  && (binarySearchCustomer(cust2, 0, 0) == -1)
                  && (binarySearchCustomer(cust2, 99999, 1) == -1);
    failed += check("binary search returns -1 for missing people", missingOk);
 
    // example from your notes: find P503
    int total503 = params2->data[502].total;
    int idx503 = binarySearchCustomer(cust2, total503, 503);
    std::cout << "\nFind P503 (total P" << total503 << "): ";
    if (idx503 != -1) std::cout << cust2->data[idx503] << std::endl;
    else              std::cout << "not found" << std::endl;
 
    std::cout << "\nFront of sorted list (lowest total): " << cust2->data[0] << std::endl;
    std::cout << "Back of sorted list (highest total): " << cust2->data[n - 1] << std::endl;
 
    destroyParamArray(cust2);
    destroyParamArray(params2);
    delete[] people;
 
 
    //==================================
    // PART 3: 100000 people (speed + correctness)
    //==================================
    std::cout << "\nPART 3: 100000 people \n";
    int big = 100000;
 
    Person* crowd = generatePeople(big);
    mergeSort(crowd, big);
    ParamArray<Params>* params3 = createParamArray<Params>(big);
    initFromPeople(params3, crowd, big);
    for (int i = 0; i < big; i++) fakeChoices(params3->data[i]);
 
    CustomerArray* cust3 = buildCustomerArray(params3);
    sortCustomers(cust3);
 
    bool sorted3 = true;
    bool found3 = true;
    for (int i = 0; i < big; i++) {
        if (i > 0 && !customerLessOrEqual(cust3->data[i - 1], cust3->data[i])) sorted3 = false;
        if (binarySearchCustomer(cust3, cust3->data[i].total, cust3->data[i].id) != i) found3 = false;
    }
    failed += check("100000 people sorted", sorted3);
    failed += check("binary search finds all 100000 people", found3);
 
    destroyParamArray(cust3);
    destroyParamArray(params3);
    delete[] crowd;
 
 
    //==================================
    // result
    //==================================
    std::cout << "\n";
    if (failed == 0) std::cout << "ALL CHECKS PASSED \n";
    else             std::cout << failed << " CHECK(S) FAILED \n";
 
    return 0;
}
