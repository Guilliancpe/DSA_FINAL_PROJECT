#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <vector>
#include <string>
#include "CinemaSim.h"   // the project's own queue is used by bfs

// ======================================================
// Movie graph (undirected, unweighted)
//
// 1. HOLD the data
//   Every node is one movie and holds its own customer records
//
// 2. TRAVERSABLE
//   Starting from one movie, bfs / dfs reach every movie that is
//   connected through the links, and the records inside each movie
//   can be read
// ======================================================
template <typename T>
class movieGraph {
    private:
        struct MovieNode {
            std::string name;
            std::vector<T> customers;  
        };

        std::vector<MovieNode> movies;
        std::vector< std::vector<int> > links;   

        bool validIndex(int i) const;
        void dfsHelper(int current, std::vector<bool>& seen, std::vector<int>& order) const;

    public:
        movieGraph();

        // building 
        // adds one movie holding count records copied from arr, returns its index
        int addMovie(const std::string& name, const T* arr, int count);

        // links two movies both ways
        bool addLink(int a, int b);
        void connectAll();

        // cuts the array into parts, one per movie, then links them all
        // returns how many movies were created
        int buildFromArray(const T* arr, int n, const std::string* names, int movieCount);

        // gives every customer a RANDOM movie, then links all the movies
        // uses rand(), so call srand once in maiin first
        int buildRandom(const T* arr, int n, const std::string* names, int movieCount);

        // reading 
        int size() const;
        const std::string& movieName(int index) const;
        int customerCount(int index) const;
        const std::vector<T>& customers(int index) const;
        const std::vector<int>& neighbours(int index) const;
        bool isLinked(int a, int b) const;

        // traversal
        std::vector<int> bfs(int start) const;
        std::vector<int> dfs(int start) const;

        // looking things up 
        // which movie holds this customer ID, and at which position inside it
        bool findCustomer(int id, int& movieIndex, int& position) const;
        long long movieRevenue(int index) const;   // sum of every customer's total

        // printing 
        void displayMovie(int index, int count = -1) const;   // count = -1 prints all
        void displayGraph() const;
        void displayTraversal(const std::vector<int>& order) const;
};

template <typename T>
movieGraph<T>::movieGraph() {}

template <typename T>
bool movieGraph<T>::validIndex(int i) const {
    return i >= 0 && i < (int)movies.size();
}

template <typename T>
int movieGraph<T>::addMovie(const std::string& name, const T* arr, int count) {
    MovieNode node;
    node.name = name;
    if (arr != nullptr && count > 0) node.customers.assign(arr, arr + count);

    movies.push_back(node);
    links.push_back(std::vector<int>());
    return (int)movies.size() - 1;
}

template <typename T>
bool movieGraph<T>::addLink(int a, int b) {
    if (!validIndex(a) || !validIndex(b) || a == b) return false;
    if (isLinked(a, b)) return true; 

    links[a].push_back(b);
    links[b].push_back(a);               // undirected: both directions
    return true;
}

template <typename T>
void movieGraph<T>::connectAll() {
    for (int a = 0; a < (int)movies.size(); a++) {
        for (int b = a + 1; b < (int)movies.size(); b++) {
            addLink(a, b);
        }
    }
}

template <typename T>
int movieGraph<T>::buildFromArray(const T* arr, int n, const std::string* names, int movieCount) {
    if (arr == nullptr || names == nullptr || n < 0 || movieCount <= 0) return 0;

    int base = n / movieCount;
    int extra = n % movieCount;
    int start = 0;

    for (int i = 0; i < movieCount; i++) {
        int count = base + (i < extra ? 1 : 0);
        addMovie(names[i], arr + start, count);
        start += count;
    }
    connectAll();
    return movieCount;
}

template <typename T>
int movieGraph<T>::buildRandom(const T* arr, int n, const std::string* names, int movieCount) {
    if (arr == nullptr || names == nullptr || n < 0 || movieCount <= 0) return 0;

    int first = (int)movies.size();  // index of the first movie created here

    // empty movies first
    for (int i = 0; i < movieCount; i++) {
        addMovie(names[i], nullptr, 0);
    }

    // each customer picks a random movie
    for (int i = 0; i < n; i++) {
        int pick = rand() % movieCount;
        movies[first + pick].customers.push_back(arr[i]);
    }
    connectAll();
    return movieCount;
}

template <typename T>
int movieGraph<T>::size() const {
    return (int)movies.size();
}

template <typename T>
const std::string& movieGraph<T>::movieName(int index) const {
    static const std::string none = "";
    if (!validIndex(index)) return none;
    return movies[index].name;
}

template <typename T>
int movieGraph<T>::customerCount(int index) const {
    if (!validIndex(index)) return 0;
    return (int)movies[index].customers.size();
}

template <typename T>
const std::vector<T>& movieGraph<T>::customers(int index) const {
    static const std::vector<T> none;
    if (!validIndex(index)) return none;
    return movies[index].customers;
}

template <typename T>
const std::vector<int>& movieGraph<T>::neighbours(int index) const {
    static const std::vector<int> none;
    if (!validIndex(index)) return none;
    return links[index];
}

template <typename T>
bool movieGraph<T>::isLinked(int a, int b) const {
    if (!validIndex(a) || !validIndex(b)) return false;
    for (size_t k = 0; k < links[a].size(); k++) {
        if (links[a][k] == b) return true;
    }
    return false;
}

// ---------------------------------------------
// BFS: visit the start then all of its
// neighbours, then their neighbours (uses the queue).
// ---------------------------------------------
template <typename T>
std::vector<int> movieGraph<T>::bfs(int start) const {
    std::vector<int> order;
    if (!validIndex(start)) return order;

    std::vector<bool> seen(movies.size(), false);
    qNode<int>* front =nullptr;
    qNode<int>* back =nullptr;

    seen[start] = true;
    enqueue(&front, &back, start);

    while (!isEmpty(front)) {
        int current = FrontVariable(front);
        dequeue(&front, &back);
        order.push_back(current);

        for (size_t k = 0; k < links[current].size(); k++) {
            int next = links[current][k];
            if (!seen[next]) {
                seen[next] = true;
                enqueue(&front, &back, next);
            }
        }
    }
    return order;
}

// ---------------------------------------------
// DFS: go as deep as possible then backtrack
// ---------------------------------------------
template <typename T>
void movieGraph<T>::dfsHelper(int current, std::vector<bool>& seen, std::vector<int>& order) const {
    seen[current] = true;
    order.push_back(current);

    for (size_t k = 0; k < links[current].size(); k++) {
        int next = links[current][k];
        if (!seen[next]) dfsHelper(next, seen, order);
    }
}

template <typename T>
std::vector<int> movieGraph<T>::dfs(int start) const {
    std::vector<int> order;
    if (!validIndex(start)) return order;

    std::vector<bool> seen(movies.size(), false);
    dfsHelper(start, seen, order);
    return order;
}

template <typename T>
bool movieGraph<T>::findCustomer(int id, int& movieIndex, int& position) const {
    for (int m = 0; m < (int)movies.size(); m++) {
        for (int i = 0; i < (int)movies[m].customers.size(); i++) {
            if (movies[m].customers[i].id == id) {
                movieIndex = m;
                position = i;
                return true;
            }
        }
    }
    return false;
}

template <typename T>
long long movieGraph<T>::movieRevenue(int index) const {
    long long sum = 0;
    if (!validIndex(index)) return sum;
    for (size_t i = 0; i < movies[index].customers.size(); i++) {
        sum += movies[index].customers[i].total;
    }
    return sum;
}

template <typename T>
void movieGraph<T>::displayMovie(int index, int count) const {
    if (!validIndex(index)) {
        std::cout << "Movie not found!"<< std::endl;
        return;
    }

    const std::vector<T>& list = movies[index].customers;
    int total = (int)list.size();
    int shown = (count < 0 || count > total) ? total : count;

    std::cout << "=== " << movies[index].name<< " (" << total << " customers) ===" << std::endl;
    for (int i = 0; i < shown; i++) {
        std::cout << list[i] << std::endl;
    }
    if (shown < total) {
        std::cout << "... "<< (total - shown) << " more" << std::endl;
    }
}

template <typename T>
void movieGraph<T>::displayGraph() const {
    for (int m = 0; m < (int)movies.size(); m++) {
        std::cout << "[" << m << "] "<< movies[m].name << " - "
                  << movies[m].customers.size() << " customers, linked to: ";
        if (links[m].empty()) std::cout << "nobody";
        for (size_t k = 0; k < links[m].size(); k++) {
            std::cout << movies[links[m][k]].name << (k + 1 < links[m].size() ? ", " : "");
        }
        std::cout << std::endl;
    }
}

template <typename T>
void movieGraph<T>::displayTraversal(const std::vector<int>& order) const {
    for (size_t k = 0; k < order.size(); k++) {
        std::cout << movies[order[k]].name << (k + 1 < order.size() ? " -> " : "");
    }
    std::cout << std::endl;
}

#endif