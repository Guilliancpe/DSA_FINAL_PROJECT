#ifndef MENUITEM_H
#define MENUITEM_H

#include <iostream>
#include <cstring>

// One node of the menu tree: a category (price 0) or an item with a price.
// name is a string literal (like Item in ParamArray.h), so no delete is needed
// and the pointer stays valid when the item is copied into a Params record.
struct MenuItem {
    const char* name;
    int price;

    MenuItem() : name(""), price(0) {}
    MenuItem(const char* n, int p = 0) : name(n), price(p) {}

    // the tree finds nodes by name
    bool operator==(const MenuItem& other) const { return std::strcmp(name, other.name) == 0; }
};

inline std::ostream& operator<<(std::ostream& os, const MenuItem& m) {
    os << m.name;
    if (m.price > 0) os << " (" << m.price << ")";
    return os;
}

#endif
