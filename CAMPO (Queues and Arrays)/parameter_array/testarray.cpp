// main_customer.cpp
// Tests the customer array (sort by total + binary search) with 1000 people.
// The BST and general tree are not built yet, so the class and the food/drinks
// are picked randomly here just to fill the records (temporary, for testing only).
#include <iostream>
#include <ctime>
#include "CustomerArray.h"
 
 
int main(){
 
    srand(time(0));   // seed, called once
 
    const int N = 1000;
 
    // temporary menu for the test
    const char* foodNames[]  = {"popcorn", "chips", "fries"};
    const int   foodPrices[] = {80, 60, 70};
    const char* drinkNames[]  = {"water", "juice", "shake"};
    const int   drinkPrices[] = {30, 50, 90};
 
    //==================================
    // Step 1: queue (same as before)
    //==================================
    Person* people = generatePeople(N);
    mergeSort(people, N);
 
    qNode<Person>* front = nullptr;
    qNode<Person>* back = nullptr;
    enqueueAll(&front, &back, people, N);
 
    //==================================
    // Step 2: parameter array, filled one person at a time
    //==================================
    Params* paramArr = createParamArray<Params>(people, N);
 
    while(!isEmpty(front)){
        Person p = FrontVariable(front);
        dequeue(&front, &back);
 
        Params& rec = paramArr[p.id - 1];
 
        // fake choice: if they can afford VIP, 50% chance of VIP, otherwise Regular
        if(rec.startMoney >= VIP_PRICE && rand() % 2 == 0) setClass(rec, VIP);
        else setClass(rec, REGULAR);
 
        // fake food and drinks: try 0 to 3 of each (addFood refuses if they can't afford it)
        int foodTries = rand() % 4;
        for(int i = 0; i < foodTries; i++){
            int pick = rand() % 3;
            addFood(rec, foodNames[pick], foodPrices[pick]);
        }
        int drinkTries = rand() % 4;
        for(int i = 0; i < drinkTries; i++){
            int pick = rand() % 3;
            addDrink(rec, drinkNames[pick], drinkPrices[pick]);
        }
    }
 
    std::cout << "Parameter array (first 5, in ID order): \n";
    displayParamArray(paramArr, 5);
 
    //==================================
    // Step 3: customer array, sorted by total spent
    //==================================
    Params* customers = createSortedCustomerArray(paramArr, N);
 
    std::cout << "\nCustomer array (first 5, lowest total): \n";
    displayParamArray(customers, 5);
    std::cout << "\nCustomer array (last 5, highest total): \n";
    displayParamArray(customers + (N - 5), 5);
 
    // is it really sorted by (total, id)?
    bool sortedOk = true;
    for(int i = 1; i < N; i++){
        if(!paramLessOrEqual(customers[i - 1], customers[i])) sortedOk = false;
        if(customers[i - 1].total == customers[i].total && customers[i - 1].id == customers[i].id) sortedOk = false;
    }
    std::cout << "\nSorted by (total, ID): " << (sortedOk ? "yes" : "NO") << std::endl;
 
    // the parameter array must be untouched (slot = ID - 1)
    bool paramOk = true;
    for(int i = 0; i < N; i++){
        if(paramArr[i].id != i + 1) paramOk = false;
    }
    std::cout << "Parameter array still in ID order: " << (paramOk ? "yes" : "NO") << std::endl;
 
    // a total should never be more than what the person started with
    bool moneyOk = true;
    for(int i = 0; i < N; i++){
        if(customers[i].total > customers[i].startMoney) moneyOk = false;
    }
    std::cout << "Nobody spent more than they had: " << (moneyOk ? "yes" : "NO") << std::endl;
 
    //==================================
    // Step 4: binary search
    //==================================
    std::cout << "\nBinary search: \n";
 
    // look for one person (ID 503, taken from the parameter array)
    int searchId = 503;
    int searchTotal = paramArr[searchId - 1].total;
    searchAndDisplay(customers, N, searchTotal, searchId);
 
    // look for the first and last person in the sorted array
    searchAndDisplay(customers, N, customers[0].total, customers[0].id);
    searchAndDisplay(customers, N, customers[N - 1].total, customers[N - 1].id);
 
    // someone that does not exist (ID 5000), and a wrong total for a real ID
    searchAndDisplay(customers, N, 300, 5000);
    searchAndDisplay(customers, N, searchTotal + 1, searchId);
 
    // can binary search find EVERY person?
    int foundCount = 0;
    for(int i = 0; i < N; i++){
        int index = binarySearch(customers, N, customers[i].total, customers[i].id);
        if(index == i) foundCount++;
    }
    std::cout << "\nBinary search found every person at the right index: " << foundCount << "/" << N << std::endl;
 
    delete[] customers;
    delete[] paramArr;
    delete[] people;
 
    return 0;
}