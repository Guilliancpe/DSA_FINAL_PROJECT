#ifndef MENUITEM_H
#define MENUITEM_H

#include <iostream>
#include <string>

// One node of the menu tree: a category (price 0) or an item with a price
struct MenuItem {
    std::string name;
    int price;

    MenuItem() : name(""), price(0) {}
    MenuItem(std::string n, int p = 0) : name(n), price(p) {}

    // the tree finds nodes by name
    bool operator==(const MenuItem& other) const { return name == other.name; }
};

inline std::ostream& operator<<(std::ostream& os, const MenuItem& m) {
    os << m.name;
    if (m.price > 0) os << " (" << m.price << ")";
    return os;
}

#endif
