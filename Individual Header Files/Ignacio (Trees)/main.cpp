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

// The BST orders people by money (CinemaSim.h doesn't have a < operator)
bool operator<(const Person& a, const Person& b) {
    return a.money < b.money;
}

// -------------------------------------------------------
// One person walks through the menu tree. In each category (Food, Drinks)
// they buy 1-3 items. Every pick goes straight into THEIR record, so their
// money left shrinks each time. p is the person's own slot in the parameter array.
// -------------------------------------------------------
void orderFood(Params& p, genTree<MenuItem>& menu) {
    std::cout << "Person " << p.id << " [" << className(p.cls) << "] starts ordering with P"
              << moneyLeft(p) << std::endl;

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
                std::cout << "   " << categories[c].name << ": can't afford anything more (P"
                          << moneyLeft(p) << ")" << std::endl;
                break;
            }

            MenuItem choice = affordable[rand() % affordable.size()];

            bool ok = isFood ? addFood(p, choice.name, choice.price)
                             : addDrink(p, choice.name, choice.price);
            if (!ok) {
                std::cout << "   " << categories[c].name << ": no free slot left" << std::endl;
                break;
            }

            std::cout << "   " << categories[c].name << ": picked " << choice.name
                      << " (-" << choice.price << ") -> money left P" << moneyLeft(p) << std::endl;
        }
    }
}

// the person is done with both trees: their record is complete in the array
void finishPerson(const Params* params, int id, int& saved, int capacity) {
    saved++;
    std::cout << "   DONE -> saved in slot " << (id - 1) << " (" << saved << "/" << capacity << "): "
              << params[id - 1] << std::endl << std::endl;
}

int main(int argc, char* argv[]) {
    // srand is called ONCE here; pass a number (./sim 42) to repeat the same run
    unsigned int seed = (argc > 1) ? (unsigned int)std::atoi(argv[1]) : (unsigned int)time(0);
    srand(seed);

    // ---------------------------------------------------
    // STEP 1: generate people, sort them, make the parameter array, enqueue
    // The array has one slot per person from the queue input (slot = ID - 1)
    // ---------------------------------------------------
    qNode<Person>* frontPtr = nullptr;
    qNode<Person>* backPtr = nullptr;
    const int CUSTOMERS = 8;

    Person* people = generatePeople(CUSTOMERS);
    mergeSort(people, CUSTOMERS);

    Params* params = createParamArray<Params>(people, CUSTOMERS);   // capacity = CUSTOMERS
    enqueueAll(&frontPtr, &backPtr, people, CUSTOMERS);
    delete[] people;

    std::cout << "=== STEP 1: THE QUEUE (front -> back) ===" << std::endl;
    display(frontPtr);
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

    while (!isEmpty(frontPtr)) {
        Person current = FrontVariable(frontPtr);
        dequeue(&frontPtr, &backPtr);

        std::cout << "Dequeued " << current << " -> goes "
                  << (current.money < VIP_PRICE ? "LEFT (regular)" : "RIGHT (VIP)") << std::endl;
        bst.insert(current);
    }
    std::cout << std::endl << "BST (sideways, right side on top):" << std::endl;
    bst.display();
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 3: separate VIP from non-VIP, each person's class goes into their slot
    // ---------------------------------------------------
    std::vector<Person> regulars, vips;
    bst.collectLeft(regulars);
    bst.collectRight(vips);

    std::cout << "=== STEP 3: SEPARATED + TICKETS PAID ===" << std::endl;
    for (size_t i = 0; i < vips.size(); i++) {
        Params& p = params[vips[i].id - 1];
        if (setClass(p, VIP)) {
            std::cout << "VIP     Person " << p.id << " paid P" << classPrice(p.cls)
                      << " -> money left P" << moneyLeft(p) << std::endl;
        }
    }
    for (size_t i = 0; i < regulars.size(); i++) {
        Params& p = params[regulars[i].id - 1];
        if (setClass(p, REGULAR)) {
            std::cout << "Regular Person " << p.id << " paid P" << classPrice(p.cls)
                      << " -> money left P" << moneyLeft(p) << std::endl;
        }
    }
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
    std::cout << "=== STEP 5: ORDERING ===" << std::endl;
    int saved = 0;
    for (size_t i = 0; i < vips.size(); i++) {
        orderFood(params[vips[i].id - 1], menu);
        finishPerson(params, vips[i].id, saved, CUSTOMERS);
    }
    for (size_t i = 0; i < regulars.size(); i++) {
        orderFood(params[regulars[i].id - 1], menu);
        finishPerson(params, regulars[i].id, saved, CUSTOMERS);
    }

    // ---------------------------------------------------
    // STEP 6: the full parameter array (slot = ID - 1)
    // ---------------------------------------------------
    std::cout << "=== STEP 6: PARAMETER ARRAY (slot = ID - 1) ===" << std::endl;
    displayParamArray(params, CUSTOMERS);
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 7: customer array = sorted copy (by total spent, then ID)
    // ---------------------------------------------------
    Params* customers = createSortedCustomerArray(params, CUSTOMERS);

    std::cout << "=== STEP 7: CUSTOMER ARRAY (sorted by total spent) ===" << std::endl;
    displayParamArray(customers, CUSTOMERS);
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 8: binary search on the sorted customer array
    // ---------------------------------------------------
    std::cout << "=== STEP 8: BINARY SEARCH ===" << std::endl;
    int mid = CUSTOMERS / 2;
    searchAndDisplay(customers, CUSTOMERS, customers[mid].total, customers[mid].id);   // exists
    searchAndDisplay(customers, CUSTOMERS, 9999, 1);                                    // does not exist

    // cleanup
    delete[] customers;
    delete[] params;
    clearQueue(&frontPtr, &backPtr);
    std::cout << std::endl << "(seed used: " << seed << ")" << std::endl;
    return 0;
}
