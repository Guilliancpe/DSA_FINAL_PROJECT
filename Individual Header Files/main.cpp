#include <iostream>
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <cstring>   // strcmp
#include <vector>
#include <chrono>    // Para sa experimental time complexity analysis
#include <iomanip>   // Para sa output formatting

// Isama ang lahat ng kinakailangang header files ng proyekto
#include "CinemaSim.h"      // Queue + Person struct + random money + merge sort
#include "ParamArray.h"     // Params struct, prices, setClass / addFood / addDrink
#include "Customerarray.h"  // Sorted customer array + binary search
#include "BST.h"            // Binary Search Tree
#include "GT.h"             // General Tree para sa concession menu
#include "MenuItem.h"       // MenuItem struct para sa menu tree
#include "Graph.h"          // Movie Graph (undirected, unweighted)
#include "History.h"        // Interactive History viewer gamit ang graph traversal
#include "MergeSort.h"      // Generic stable merge sort
#include "Search.h"         // Generic binary and linear search

int CUSTOMERS = 1000;   // Babaguhin depende sa input ng user (1k, 10k, 100k, etc.)
const int SHOW = 5;     // Bilang ng ire-render sa detailed outputs

// Overload operator< para sa Person para ma-sort sa BST base sa perang dala
bool operator<(const Person& a, const Person& b) {
    return a.money < b.money;
}

// -------------------------------------------------------------------
// General Tree Walkthrough: Pagbili ng Pagkain at Inumin
// -------------------------------------------------------------------
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
                          << " (-P" << choice.price << ") -> money left P" << moneyLeft(p) << std::endl;
            }
        }
    }
}

void finishPerson(const Params* params, int id, int& saved, int capacity) {
    saved++;
    if (saved <= SHOW) {
        std::cout << "   DONE -> saved in slot " << (id - 1) << " (" << saved << "/" << capacity << "): "
                  << params[id - 1] << std::endl << std::endl;
    } else if (saved % (capacity / 4 == 0 ? 1 : capacity / 4) == 0 || saved == capacity) {
        std::cout << "... " << saved << "/" << capacity << " saved" << std::endl;
    }
}

// -------------------------------------------------------------------
// Experimental Analysis: Pagsukat ng Execution Time ng mga Algorithm
// -------------------------------------------------------------------
void runExperimentalAnalysis(const Params* params, const Params* customers, const movieGraph<Params>& history, int totalCustomers) {
    using namespace std::chrono;

    std::cout << "\n======================================================\n";
    std::cout << "        EXPERIMENTAL ALGORITHM TIME ANALYSIS          \n";
    std::cout << "======================================================\n";

    // 1. Benchmark: Merge Sort Time (O(N log N))
    Person* tempPeople = generatePeople(totalCustomers);
    auto start = high_resolution_clock::now();
    mergeSort(tempPeople, totalCustomers);
    auto stop = high_resolution_clock::now();
    auto mergeTime = duration_cast<microseconds>(stop - start).count();
    delete[] tempPeople;
    std::cout << "[1] Merge Sort (Person money, N=" << totalCustomers << "): " 
              << mergeTime << " microseconds  [Theoretical: O(N log N)]\n";

    // 2. Benchmark: BST Insertion Time (O(N log N))
    Person threshold = {0, VIP_PRICE};
    binarySearchTree<Person> testBst(threshold);
    Person* testGroup = generatePeople(totalCustomers);
    start = high_resolution_clock::now();
    for (int i = 0; i < totalCustomers; i++) {
        testBst.insert(testGroup[i]);
    }
    stop = high_resolution_clock::now();
    auto bstTime = duration_cast<microseconds>(stop - start).count();
    delete[] testGroup;
    std::cout << "[2] BST Insertion (N=" << totalCustomers << "): " 
              << bstTime << " microseconds  [Theoretical: O(N log N)]\n";

    // 3. Benchmark: Binary Search vs Linear Search
    int targetId = customers[totalCustomers / 2].id;
    int targetTotal = customers[totalCustomers / 2].total;

    // Binary Search timing
    start = high_resolution_clock::now();
    for (int i = 0; i < 10000; i++) {
        binarySearch(customers, totalCustomers, targetTotal, targetId);
    }
    stop = high_resolution_clock::now();
    auto bsTime = duration_cast<microseconds>(stop - start).count();

    // Linear Search timing
    start = high_resolution_clock::now();
    for (int i = 0; i < 10000; i++) {
        for (int j = 0; j < totalCustomers; j++) {
            if (customers[j].id == targetId && customers[j].total == targetTotal) break;
        }
    }
    stop = high_resolution_clock::now();
    auto lsTime = duration_cast<microseconds>(stop - start).count();

    std::cout << "[3] Binary Search (10,000 runs): " << bsTime << " microseconds  [Theoretical: O(log N)]\n";
    std::cout << "[4] Linear Search (10,000 runs): " << lsTime << " microseconds  [Theoretical: O(N)]\n";
    std::cout << "    -> Binary Search is " << (bsTime > 0 ? (double)lsTime / bsTime : 1.0) 
              << "x faster than Linear Search!\n";

    // 4. Benchmark: Graph Traversals (BFS & DFS)
    start = high_resolution_clock::now();
    std::vector<int> bfsResult = history.bfs(0);
    stop = high_resolution_clock::now();
    auto bfsTime = duration_cast<microseconds>(stop - start).count();

    start = high_resolution_clock::now();
    std::vector<int> dfsResult = history.dfs(0);
    stop = high_resolution_clock::now();
    auto dfsTime = duration_cast<microseconds>(stop - start).count();

    std::cout << "[5] Graph BFS Traversal: " << bfsTime << " microseconds  [Theoretical: O(V + E)]\n";
    std::cout << "[6] Graph DFS Traversal: " << dfsTime << " microseconds  [Theoretical: O(V + E)]\n";
    std::cout << "======================================================\n\n";
}

// -------------------------------------------------------------------
// Total Daily Sales & Per-Movie Sales Breakdown (Naka-Pesos 'P')
// -------------------------------------------------------------------
void displaySalesSummary(const movieGraph<Params>& history, const Params* params, int totalCustomers) {
    std::cout << "\n======================================================\n";
    std::cout << "          CINEMA DAILY SALES & REVENUE REPORT         \n";
    std::cout << "======================================================\n";

    long long grandTotalSales = 0;
    for (int i = 0; i < totalCustomers; i++) {
        grandTotalSales += params[i].total;
    }

    std::cout << std::left << std::setw(6) << "No." 
              << std::setw(28) << "Movie Title" 
              << std::setw(15) << "Customers" 
              << "Total Sales (Pesos)" << std::endl;
    std::cout << "----------------------------------------------------------------\n";

    for (int m = 0; m < history.size(); m++) {
        long long movieSales = history.movieRevenue(m);
        std::cout << std::left << std::setw(6) << (m + 1)
                  << std::setw(28) << history.movieName(m)
                  << std::setw(15) << history.customerCount(m)
                  << "P" << movieSales << std::endl;
    }

    std::cout << "----------------------------------------------------------------\n";
    std::cout << "TOTAL SALES FOR THE DAY: P" << grandTotalSales << std::endl;
    std::cout << "AVERAGE SPEND PER CUSTOMER: P" << (grandTotalSales / totalCustomers) << std::endl;
    std::cout << "======================================================\n\n";
}

// -------------------------------------------------------------------
// Customer Individual Search via Binary Search (VIP/Regular & Items)
// -------------------------------------------------------------------
void searchCustomerById(const Params* params, const Params* customers, int n, const movieGraph<Params>& history) {
    std::cout << "\n--- CUSTOMER INDIVIDUAL PURCHASE SEARCH (BINARY SEARCH) ---\n";
    std::cout << "Enter Customer ID to search (1 to " << n << "): ";
    int searchId;
    if (!(std::cin >> searchId) || searchId < 1 || searchId > n) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Invalid Customer ID.\n";
        return;
    }

    // Step A: Kunin ang kabuuang gastos para sa search key (total, id)
    int targetTotal = params[searchId - 1].total;

    // Step B: Gamitin ang Binary Search sa naka-sort na customer array
    int index = binarySearch(customers, n, targetTotal, searchId);

    if (index == -1) {
        std::cout << "Customer ID " << searchId << " not found in sorted records.\n";
        return;
    }

    const Params& p = customers[index];

    // Alamin kung anong pelikula ang napanood ng customer sa Graph
    int movieIdx = -1, pos = -1;
    history.findCustomer(searchId, movieIdx, pos);

    std::cout << "\n==================================================\n";
    std::cout << "  CUSTOMER RECEIPT & INDIVIDUAL PURCHASES\n";
    std::cout << "==================================================\n";
    std::cout << " Customer ID     : " << p.id << std::endl;
    std::cout << " Ticket Status   : " << (p.cls == VIP ? "[VIP CUSTOMER]" : "[REGULAR CUSTOMER]") << std::endl;
    std::cout << " Ticket Price    : P" << classPrice(p.cls) << std::endl;
    if (movieIdx != -1) {
        std::cout << " Movie Attended  : " << history.movieName(movieIdx) << std::endl;
    }
    std::cout << " Initial Wallet  : P" << p.startMoney << std::endl;
    std::cout << " Wallet Balance  : P" << moneyLeft(p) << std::endl;
    std::cout << " Total Spent     : P" << p.total << std::endl;
    std::cout << "--------------------------------------------------\n";
    std::cout << " Food Purchases  : ";
    if (p.foodCount == 0) std::cout << "None";
    for (int i = 0; i < p.foodCount; i++) {
        std::cout << p.foods[i].name << " (P" << p.foods[i].price << ")" 
                  << (i < p.foodCount - 1 ? ", " : "");
    }
    std::cout << std::endl;

    std::cout << " Drink Purchases : ";
    if (p.drinkCount == 0) std::cout << "None";
    for (int i = 0; i < p.drinkCount; i++) {
        std::cout << p.drinks[i].name << " (P" << p.drinks[i].price << ")" 
                  << (i < p.drinkCount - 1 ? ", " : "");
    }
    std::cout << std::endl;
    std::cout << "==================================================\n\n";
}

// -------------------------------------------------------------------
// MAIN FUNCTION
// -------------------------------------------------------------------
int main(int argc, char* argv[]) {
    unsigned int seed = (argc > 1) ? (unsigned int)std::atoi(argv[1]) : (unsigned int)time(0);
    srand(seed);

    // =================================================================
    // 0. INPUT FOR NUMBER OF CUSTOMERS BEFORE RUNNING SIMULATION
    // =================================================================
    std::cout << "======================================================\n";
    std::cout << "          CINEMA SIMULATION INITIALIZATION            \n";
    std::cout << "======================================================\n";
    std::cout << "Select Number of Customers to Simulate:\n";
    std::cout << "  1. 1,000 customers (1k)\n";
    std::cout << "  2. 10,000 customers (10k)\n";
    std::cout << "  3. 100,000 customers (100k)\n";
    std::cout << "  4. Custom amount\n";
    std::cout << "------------------------------------------------------\n";
    std::cout << "Enter choice (1-4): ";
    
    int sizeChoice = 1;
    if (std::cin >> sizeChoice) {
        if (sizeChoice == 1) CUSTOMERS = 1000;
        else if (sizeChoice == 2) CUSTOMERS = 10000;
        else if (sizeChoice == 3) CUSTOMERS = 100000;
        else if (sizeChoice == 4) {
            std::cout << "Enter custom number of customers: ";
            std::cin >> CUSTOMERS;
            if (CUSTOMERS <= 0) CUSTOMERS = 1000;
        }
    } else {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        CUSTOMERS = 1000;
    }

    std::cout << "\n--> Starting simulation for " << CUSTOMERS << " customers...\n\n";

    // ---------------------------------------------------
    // STEP 1: CREATE, MERGE SORT, ENQUEUE
    // ---------------------------------------------------
    qNode<Person>* frontPtr = nullptr;
    qNode<Person>* backPtr = nullptr;

    Person* people = generatePeople(CUSTOMERS);
    mergeSort(people, CUSTOMERS);

    Params* params = createParamArray<Params>(people, CUSTOMERS); 
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
    // STEP 2: DEQUEUE INTO THE BST
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
    std::cout << "BST height: " << bst.height() << " levels" << std::endl;
    std::cout << std::endl;

    // ---------------------------------------------------
    // STEP 3: SEPARATED + TICKETS PAID
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
    // STEP 4: THE MENU (GENERAL TREE)
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
    // STEP 5: ORDERING
    // ---------------------------------------------------
    std::cout << "=== STEP 5: ORDERING (first " << SHOW << " people shown in detail) ===" << std::endl;
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
    // STEP 6: PARAMETER ARRAY
    // ---------------------------------------------------
    std::cout << "=== STEP 6: PARAMETER ARRAY (slot = ID - 1) ===" << std::endl;
    std::cout << "First " << SHOW << " slots:" << std::endl;
    displayParamArray(params, SHOW);
    std::cout << "Last " << SHOW << " slots:" << std::endl;
    displayParamArray(params + (CUSTOMERS - SHOW), SHOW);

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
    // STEP 7: CUSTOMER ARRAY (SORTED COPY BY TOTAL SPENT)
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
    // STEP 8: BINARY SEARCH DEMONSTRATION
    // ---------------------------------------------------
    std::cout << "=== STEP 8: BINARY SEARCH ===" << std::endl;
    int mid = CUSTOMERS / 2;
    searchAndDisplay(customers, CUSTOMERS, customers[0].total, customers[0].id); 
    searchAndDisplay(customers, CUSTOMERS, customers[mid].total, customers[mid].id); 
    searchAndDisplay(customers, CUSTOMERS, customers[CUSTOMERS - 1].total, customers[CUSTOMERS - 1].id); 
    searchAndDisplay(customers, CUSTOMERS, 9999, 1); 

    // MGA PAMAGAT NG PELIKULA SA MOVIE GRAPH
    std::string movieNames[5] = {
        "Avatar: The Way of Water",
        "Avengers: Endgame",
        "Inception",
        "Interstellar",
        "The Dark Knight"
    };

    movieGraph<Params> history;
    history.buildRandom(params, CUSTOMERS, movieNames, 5);

    // ---------------------------------------------------
    // INTERACTIVE SYSTEM MENU
    // ---------------------------------------------------
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n======================================================\n";
        std::cout << "             CINEMA SIMULATION DATABASE               \n";
        std::cout << "======================================================\n";
        std::cout << "  1. Browse Movies (Graph Navigation)\n";
        std::cout << "  2. View Daily Total Sales & Per-Movie Sales (in Pesos)\n";
        std::cout << "  3. Search Customer Purchases by ID (Binary Search)\n";
        std::cout << "  4. Run Experimental Time Complexity Analysis\n";
        std::cout << "  0. Exit\n";
        std::cout << "------------------------------------------------------\n";
        std::cout << "Choose an option: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
            case 1:
                browseHistory(history);
                break;
            case 2:
                displaySalesSummary(history, params, CUSTOMERS);
                break;
            case 3:
                searchCustomerById(params, customers, CUSTOMERS, history);
                break;
            case 4:
                runExperimentalAnalysis(params, customers, history, CUSTOMERS);
                break;
            case 0:
                std::cout << "Exiting system...\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    // Memory Cleanup
    delete[] customers;
    delete[] params;
    clearQueue(&frontPtr, &backPtr);
    return 0;
}