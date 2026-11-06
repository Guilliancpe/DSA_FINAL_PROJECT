#include <iostream>
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <cstring>   // strcmp
#include <vector>

#include "CinemaSim.h"      // queue + Person + random money + merge sort
#include "ParamArray.h"     // Params record, prices, setClass / addFood / addDrink
#include "Customerarray.h"  // sorted customer array + binary search
#include "BST.h"
#include "GT.h"
#include "MenuItem.h"

const int CUSTOMERS = 1000;   // how many people are created (the array capacity too)
const int SHOW = 5;           // how many people are printed in detail (1000 lines is too long)

// The BST orders people by money (CinemaSim.h doesn't have a < operator)
bool operator<(const Person& a, const Person& b) {
    return a.money < b.money;
}

// -------------------------------------------------------
// One person walks through the menu tree. In each category (Food, Drinks)
// they buy 1-3 items. Every pick goes straight into THEIR record, so their
// money left shrinks each time. p is the person's own slot in the parameter array.
// show = print the steps or stay quiet
// -------------------------------------------------------
void orderFood(Params& p, genTree<MenuItem>& menu, bool show) {
    if (show) {
        std::cout << "Person " << p.id << " [" << className(p.cls) << "] starts ordering with P"
                  << moneyLeft(p) << std::endl;
    }

    std::vector<MenuItem> categories = menu.getChildren(MenuItem("Concession"));

    for (size_t c = 0; c < categories.size(); c++) {
        std::vector<MenuItem> items = menu.getChildren(categories[c]);
        bool isFood = (std::strcmp(categories[c].name, "Food") == 0);
        int picks = 1 + rand() % 3;

        for (int k = 0; k < picks; k++) {
            // only the items they can still afford
            std::vector<MenuItem> affordable;
            for (size_t i = 0; i < items.size(); i++) {
                if (items[i].price <= moneyLeft(p)) affordable.push_back(items[i]);
            }

            if (affordable.empty()) {
                if (show) {
                    std::cout << "   " << categories[c].name << ": can't afford anything more (P"
                              << moneyLeft(p) << ")" << std::endl;
                }
                break;
            }

            MenuItem choice = affordable[rand() % affordable.size()];

            bool ok = isFood ? addFood(p, choice.name, choice.price)
                             : addDrink(p, choice.name, choice.price);
            if (!ok) {
                if (show) std::cout << "   " << categories[c].name << ": no free slot left" << std::endl;
                break;
            }

            if (show) {
                std::cout << "   " << categories[c].name << ": picked " << choice.name
                          << " (-" << choice.price << ") -> money left P" << moneyLeft(p) << std::endl;
            }
        }
    }
}

// the person is done with both trees: their record is complete in the array
void finishPerson(const Params* params, int id, int& saved, int capacity) {
    saved++;
    if (saved <= SHOW) {
        std::cout << "   DONE -> saved in slot " << (id - 1) << " (" << saved << "/" << capacity << "): "
                  << params[id - 1] << std::endl << std::endl;
    } else if (saved % 250 == 0 || saved == capacity) {
        std::cout << "... " << saved << "/" << capacity << " saved" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // srand is called ONCE here; pass a number (./sim 42) to repeat the same run
    unsigned int seed = (argc > 1) ? (unsigned int)std::atoi(argv[1]) : (unsigned int)time(0);
    srand(seed);

    // ---------------------------------------------------
    // STEP 1: create the people, merge sort them, make the parameter array, enqueue
    // The array has one slot per person from the queue input (slot = ID - 1)
    // ---------------------------------------------------
    qNode<Person>* frontPtr = nullptr;
    qNode<Person>* backPtr = nullptr;

    Person* people = generatePeople(CUSTOMERS);
    mergeSort(people, CUSTOMERS);

    Params* params = createParamArray<Params>(people, CUSTOMERS);   // capacity = CUSTOMERS
    enqueueAll(&frontPtr, &backPtr, people, CUSTOMERS);
    delete[] people;

    std::cout << "=== STEP 1: CREATE, MERGE SORT, ENQUEUE ===" << std::endl;
    std::cout << CUSTOMERS << " people created, merge sorted (least to most money), and enqueued." << std::endl;
    std::cout << "Queue (front -> back): ";
    qNode<Person>* cur = frontPtr;
    for (int i = 0; i < SHOW && cur != nullptr; i++, cur = cur->next) {
        std::cout << cur->data << "->";
    }
    std::cout << " ... ->" << backPtr->data << "->NULL" << std::endl;
    std::cout << "Parameter array capacity: " << CUSTOMERS << " (same as the queue input)" << std::endl;
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 2: dequeue everyone into a BST.
    // The root is the VIP price: money < 500 goes LEFT (regular),
    // money >= 500 goes RIGHT (VIP).
    // ---------------------------------------------------
    std::cout << "=== STEP 2: DEQUEUE INTO THE BST (root = P" << VIP_PRICE << ") ===" << std::endl;

    Person threshold = {0, VIP_PRICE};
    binarySearchTree<Person> bst(threshold);

    int dequeued = 0, wentLeft = 0, wentRight = 0;
    while (!isEmpty(frontPtr)) {
        Person current = FrontVariable(frontPtr);
        dequeue(&frontPtr, &backPtr);
        dequeued++;

        bool goesLeft = current.money < VIP_PRICE;
        if (goesLeft) wentLeft++; else wentRight++;

        if (dequeued <= SHOW) {
            std::cout << "Dequeued " << current << " -> goes "
                      << (goesLeft ? "LEFT (regular)" : "RIGHT (VIP)") << std::endl;
        }
        bst.insert(current);
    }
    std::cout << "... (" << (dequeued - SHOW) << " more people dequeued)" << std::endl;
    std::cout << "Went left (regular): " << wentLeft << " | went right (VIP): " << wentRight << std::endl;
    std::cout << "BST height: " << bst.height() << " levels (the queue was sorted, so each side grows like a chain)" << std::endl;
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 3: separate VIP from non-VIP, each person's class goes into their slot
    // ---------------------------------------------------
    std::vector<Person> regulars, vips;
    bst.collectLeft(regulars);
    bst.collectRight(vips);

    std::cout << "=== STEP 3: SEPARATED + TICKETS PAID ===" << std::endl;
    int classFailed = 0;
    for (size_t i = 0; i < vips.size(); i++) {
        Params& p = params[vips[i].id - 1];
        if (!setClass(p, VIP)) { classFailed++; continue; }
        if (i < (size_t)SHOW) {
            std::cout << "VIP     Person " << p.id << " paid P" << classPrice(p.cls)
                      << " -> money left P" << moneyLeft(p) << std::endl;
        }
    }
    for (size_t i = 0; i < regulars.size(); i++) {
        Params& p = params[regulars[i].id - 1];
        if (!setClass(p, REGULAR)) { classFailed++; continue; }
        if (i < (size_t)SHOW) {
            std::cout << "Regular Person " << p.id << " paid P" << classPrice(p.cls)
                      << " -> money left P" << moneyLeft(p) << std::endl;
        }
    }
    std::cout << "... " << vips.size() << " VIP and " << regulars.size() << " regular tickets paid"
              << " (" << classFailed << " failed)" << std::endl;
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 4: the menu is a GENERAL tree (any number of children per node)
    // ---------------------------------------------------
    genTree<MenuItem> menu(MenuItem("Concession"));
    menu.insertChild(MenuItem("Concession"), MenuItem("Food"));
    menu.insertChild(MenuItem("Concession"), MenuItem("Drinks"));

    menu.insertChild(MenuItem("Food"), MenuItem("Fries", 60));
    menu.insertChild(MenuItem("Food"), MenuItem("Hotdog", 70));
    menu.insertChild(MenuItem("Food"), MenuItem("Popcorn", 80));
    menu.insertChild(MenuItem("Food"), MenuItem("Nachos", 100));

    menu.insertChild(MenuItem("Drinks"), MenuItem("Water", 20));
    menu.insertChild(MenuItem("Drinks"), MenuItem("Soda", 40));
    menu.insertChild(MenuItem("Drinks"), MenuItem("Iced Tea", 50));
    menu.insertChild(MenuItem("Drinks"), MenuItem("Milk Tea", 90));

    std::cout << "=== STEP 4: THE MENU (general tree) ===" << std::endl;
    menu.display();
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 5: VIPs order first, then regulars.
    // As soon as one person is done, their record is in the array.
    // ---------------------------------------------------
    std::cout << "=== STEP 5: ORDERING (first " << SHOW << " people shown in detail) ===" << std::endl;
    // (the BST lists people from least to most money, so we go backwards: richest first)
    int saved = 0;
    for (int i = (int)vips.size() - 1; i >= 0; i--) {
        orderFood(params[vips[i].id - 1], menu, saved < SHOW);
        finishPerson(params, vips[i].id, saved, CUSTOMERS);
    }
    for (int i = (int)regulars.size() - 1; i >= 0; i--) {
        orderFood(params[regulars[i].id - 1], menu, saved < SHOW);
        finishPerson(params, regulars[i].id, saved, CUSTOMERS);
    }
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 6: the full parameter array (slot = ID - 1)
    // ---------------------------------------------------
    std::cout << "=== STEP 6: PARAMETER ARRAY (slot = ID - 1) ===" << std::endl;
    std::cout << "First " << SHOW << " slots:" << std::endl;
    displayParamArray(params, SHOW);
    std::cout << "Last " << SHOW << " slots:" << std::endl;
    displayParamArray(params + (CUSTOMERS - SHOW), SHOW);

    // quick check: every slot belongs to the right person and has a class
    int filled = 0, slotsOk = 0;
    long long revenue = 0;
    for (int i = 0; i < CUSTOMERS; i++) {
        if (params[i].id == i + 1) slotsOk++;
        if (params[i].cls != NOT_CHOSEN) filled++;
        revenue += params[i].total;
    }
    std::cout << "Check: " << filled << "/" << CUSTOMERS << " slots filled, "
              << slotsOk << "/" << CUSTOMERS << " in the right slot, total spent P" << revenue << std::endl;
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 7: customer array = sorted copy (by total spent, then ID)
    // ---------------------------------------------------
    Params* customers = createSortedCustomerArray(params, CUSTOMERS);

    std::cout << "=== STEP 7: CUSTOMER ARRAY (sorted by total spent) ===" << std::endl;
    std::cout << "Lowest " << SHOW << ":" << std::endl;
    displayParamArray(customers, SHOW);
    std::cout << "Highest " << SHOW << ":" << std::endl;
    displayParamArray(customers + (CUSTOMERS - SHOW), SHOW);

    bool sortedOk = true;
    for (int i = 1; i < CUSTOMERS; i++) {
        if (!paramLessOrEqual(customers[i - 1], customers[i])) sortedOk = false;
    }
    std::cout << "Check: array sorted = " << (sortedOk ? "yes" : "NO") << std::endl;
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 8: binary search on the sorted customer array
    // ---------------------------------------------------
    std::cout << "=== STEP 8: BINARY SEARCH ===" << std::endl;
    int mid = CUSTOMERS / 2;
    searchAndDisplay(customers, CUSTOMERS, customers[0].total, customers[0].id);                     // first
    searchAndDisplay(customers, CUSTOMERS, customers[mid].total, customers[mid].id);                 // middle
    searchAndDisplay(customers, CUSTOMERS, customers[CUSTOMERS - 1].total, customers[CUSTOMERS - 1].id); // last
    searchAndDisplay(customers, CUSTOMERS, 9999, 1);                                                 // does not exist

    // cleanup
    delete[] customers;
    delete[] params;
    clearQueue(&frontPtr, &backPtr);
    std::cout << std::endl << "(seed used: " << seed << ")" << std::endl;
    return 0;
}
