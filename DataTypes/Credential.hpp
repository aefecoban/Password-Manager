#pragma once
#include <iostream>
#include <string>
#include "../Utils.hpp"
#include "sqlite3.h"

class Credential {
public:
    std::string Title;
    int ID;
    enum class Type { Username, Email, Wallet, General };

    Credential(Type t) : mytype(t), Title("") {}
    Credential(Type t, const char* title) : mytype(t), Title(title) {}
    Credential(Type t, std::string title) : mytype(t), Title(title) {}
    Credential(Type t, int ID, const char* title) : mytype(t), Title(title), ID(ID) {}
    Credential(Type t, int ID, std::string title) : mytype(t), Title(title), ID(ID) {}
    virtual ~Credential() = default;

    Type getType() const { return mytype; }
    virtual void print() const = 0;
    virtual float Search(std::string searchText) const = 0; // 0 to 1 value for similarity

protected:
    Type mytype;
};