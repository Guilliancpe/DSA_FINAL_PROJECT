#include <iostream>
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <string>
#include <vector>

#include "CinemaSim.h"   // your queue + Person + random money + merge sort
#include "BST.h"
#include "GT.h"
#include "MenuItem.h"

const int REGULAR_PRICE = 250;   // regular ticket
const int VIP_PRICE     = 500;   // VIP ticket
const int CUSTOMERS     = 8;

// The BST orders people by money (CinemaSim.h doesn't have a < operator)
bool operator<(const Person& a, const Person& b) {
    return a.money < b.money;
}

// -------------------------------------------------------
// One person walks through the menu tree: one pick per category
// (Food, Drinks). Every pick lowers their money.
// -------------------------------------------------------
void orderFood(Person& p, bool isVip, genTree<MenuItem>& menu) {
    std::cout << "Person " << p.id << (isVip ? " [VIP]" : " [Regular]")
              << " starts ordering with P" << p.money << std::endl;

    std::vector<MenuItem> categories = menu.getChildren(MenuItem("Concession"));

    for (size_t c = 0; c < categories.size(); c++) {
        std::vector<MenuItem> items = menu.getChildren(categories[c]);

        // only the items they can still afford
        std::vector<MenuItem> affordable;
        for (size_t i = 0; i < items.size(); i++) {
            if (items[i].price <= p.money) affordable.push_back(items[i]);
        }

        if (affordable.empty()) {
            std::cout << "   " << categories[c].name << ": can't afford anything (P" << p.money << ")" << std::endl;
            continue;
        }

        MenuItem choice = affordable[rand() % affordable.size()];
        p.money -= choice.price;

        std::cout << "   " << categories[c].name << ": picked " << choice.name
                  << " (-" << choice.price << ") -> money left P" << p.money << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // srand is called ONCE here; pass a number (./sim 42) to repeat the same run
    unsigned int seed = (argc > 1) ? (unsigned int)std::atoi(argv[1]) : (unsigned int)time(0);
    srand(seed);

    // ---------------------------------------------------
    // STEP 1: people are generated, sorted, and already enqueued
    // ---------------------------------------------------
    qNode<Person>* frontPtr = nullptr;
    qNode<Person>* backPtr = nullptr;

    Person* people = generatePeople(CUSTOMERS);
    mergeSort(people, CUSTOMERS);
    enqueueAll(&frontPtr, &backPtr, people, CUSTOMERS);
    delete[] people;

    std::cout << "=== STEP 1: THE QUEUE (front -> back) ===" << std::endl;
    display(frontPtr);
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
    // STEP 3: separate VIP from non-VIP and pay for the ticket
    // ---------------------------------------------------
    std::vector<Person> regulars, vips;
    bst.collectLeft(regulars);
    bst.collectRight(vips);

    std::cout << "=== STEP 3: SEPARATED + TICKETS PAID ===" << std::endl;
    for (size_t i = 0; i < vips.size(); i++) {
        vips[i].money -= VIP_PRICE;
        std::cout << "VIP     Person " << vips[i].id << " paid P" << VIP_PRICE
                  << " -> money left P" << vips[i].money << std::endl;
    }
    for (size_t i = 0; i < regulars.size(); i++) {
        regulars[i].money -= REGULAR_PRICE;
        std::cout << "Regular Person " << regulars[i].id << " paid P" << REGULAR_PRICE
                  << " -> money left P" << regulars[i].money << std::endl;
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
    // STEP 5: VIPs order first, then regulars
    // ---------------------------------------------------
    std::cout << "=== STEP 5: ORDERING ===" << std::endl;
    for (size_t i = 0; i < vips.size(); i++)     orderFood(vips[i], true, menu);
    for (size_t i = 0; i < regulars.size(); i++) orderFood(regulars[i], false, menu);

    clearQueue(&frontPtr, &backPtr);   // queue is already empty, but this keeps the cleanup habit
    std::cout << "(seed used: " << seed << ")" << std::endl;
    return 0;
}
