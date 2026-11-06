# RETADSS - REal Time Automatic Data Sorter System (CPE 010, Group 2)

## Project Description

RETADSS is a C++ console program that simulates a movie theater's ticket and concession sales. It generates customers with random money, then processes them through the data structures and algorithms we studied in CPE 010:

- **Merge sort** arranges customers from least to highest money before they enter the queue.
- **Queue** (linked list) holds the customers in order.
- **Binary Search Tree** separates Regular and VIP customers by ticket price.
- **General tree** holds the food and drinks menu.
- **Array** stores each customer's purchases.
- **Graph** links customers to the movies they watched.
- **Binary search** finds a customer's record and purchases by ID.

## How to Compile and Run

Requires a C++ compiler (g++). Keep all files in `Final_Output` together.

```
cd Final_Output
g++ -std=c++17 main.cpp -o main
./main
```

On Windows, run `main.exe` (or `main` in Command Prompt) after compiling.

## Sample Input

```
Enter choice (1-4): 1        <- simulate 1,000 customers
Choose an option: 3          <- Search Customer Purchases by ID
Enter Customer ID to search (1 to 1000): 25
Choose an option: 0          <- Exit
```

Menu options: `1` Browse Movies, `2` Daily Sales, `3` Search Customer by ID, `4` Time Complexity Analysis, `0` Exit.

## Team Members

- Ramon Brendt Campo
- Benj Federic De Leon
- Guillian Carlo Ignacio
- Ryan Lewis Mauro
- Loreen Jae Pilapil
