#ifndef HISTORY_H
#define HISTORY_H

#include <iostream>
#include <limits>
#include <string>
#include "Graph.h"

// ======================================================
// History tab: the user walks through the movie graph.
//
//   1. The list of movies is shown and the user CHOOSES one movie.
//   2. The customers inside that movie are shown, one page at a time.
//   3. From a movie the user can GO TO a linked movie.
//   4. BACK returns to the movie they came from 
// ======================================================

const int HISTORY_PAGE = 15;   // customers shown per page

// Reads a whole number from low to high. Asks again if the input is wrong.
// If the input ends, it returns 0, which every menu treats as "back" or "exit".
inline int readChoice(const std::string& prompt, int low, int high) {
    while (true) {
        std::cout << prompt;
        int value;
        if (!(std::cin >> value)) {
            if (std::cin.eof()) {
                std::cout << std::endl;
                return 0;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "  Please enter a number." << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (value < low || value > high) {
            std::cout << "  Enter a number from " << low << " to " << high << "." << std::endl;
            continue;
        }
        return value;
    }
}

// prints one page of customers
template <typename T>
void printMoviePage(const movieGraph<T>& graph, int movie, int page, int pages) {
    const std::vector<T>& list = graph.customers(movie);
    int total = (int)list.size();
    int start = page * HISTORY_PAGE;
    int end = start + HISTORY_PAGE;
    if (end > total) end = total;

    std::cout << "--- " << graph.movieName(movie) << " | page " << (page + 1) << "/" << pages;
    if (total > 0) std::cout << " | customers " << (start + 1) << "-" << end << " of " << total;
    std::cout << " ---" << std::endl;

    if (total == 0) {
        std::cout << "No customers in this movie." << std::endl;
        return;
    }
    for (int i = start; i < end; i++) {
        std::cout << list[i] << std::endl;
    }
}

// one movie: view its customers, move to a linked movie, or go back
template <typename T>
void viewMovie(const movieGraph<T>& graph, int movie, const std::string& trail) {
    int total = graph.customerCount(movie);
    int pages = (total + HISTORY_PAGE - 1) / HISTORY_PAGE;
    if (pages == 0) pages = 1;
    int page = 0;

    while (true) {
        std::cout << std::endl << "Path: " << trail << std::endl;
        printMoviePage(graph, movie, page, pages);

        std::cout << "  1. Next page" << std::endl
                  << "  2. Previous page" << std::endl
                  << "  3. Go to linked movies" << std::endl
                  << "  0. Back" << std::endl;
        int choice = readChoice("Choice: ", 0, 3);

        if (choice == 0) {
            return;   // backtrack
        }

        if (choice == 1) {
            if (page + 1 < pages) page++;
            else std::cout << "  This is the last page." << std::endl;
        }

        if (choice == 2) {
            if (page > 0) page--;
            else std::cout << "  This is the first page." << std::endl;
        }

        if (choice == 3) {
            const std::vector<int>& linked = graph.neighbours(movie);
            if (linked.empty()) {
                std::cout << "  This movie is not linked to any other movie." << std::endl;
                continue;
            }

            std::cout << "Linked movies:" << std::endl;
            for (size_t k = 0; k < linked.size(); k++) {
                std::cout << "  " << (linked[k] + 1) << ". "<< graph.movieName(linked[k])<< std::endl;
            }

            int next = readChoice("Movie number (0 to cancel): ", 0, graph.size());
            if (next == 0) continue;
            if (!graph.isLinked(movie, next - 1)) {
                std::cout << "  That movie is not linked to "<< graph.movieName(movie) << "." << std::endl;
                continue;
            }

            viewMovie(graph, next - 1, trail + " > " + graph.movieName(next - 1));
        }
    }
}

// movie list: user picks a movie or exit
template <typename T>
void browseHistory(const movieGraph<T>& graph) {
    if (graph.size() == 0) {
        std::cout << "No movies in the history yet." << std::endl;
        return;
    }

    while (true) {
        std::cout << std::endl << "===== HISTORY: MOVIES =====" << std::endl;
        for (int m = 0; m < graph.size(); m++) {
            std::cout << "  " << (m + 1) << ". " << graph.movieName(m) << " - "
                      << graph.customerCount(m) << " customers" << std::endl;
        }
        std::cout << "  0. Exit" << std::endl;

        int choice = readChoice("Choose a movie to view: ", 0, graph.size());
        if (choice == 0) return;

        viewMovie(graph, choice - 1, graph.movieName(choice - 1));
    }
}

#endif 