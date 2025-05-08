#ifndef USER_ROLES_H
#define USER_ROLES_H

#include "User.h"

class Junior : public User {
public:
    Junior(const string& name, const string& pass);
    int getClearanceLevel() const override { return 1; }
    string getRoleName() const override { return "Junior"; }
};

class Employee : public User {
public:
    Employee(const string& name, const string& pass);
    int getClearanceLevel() const override { return 2; }
    string getRoleName() const override { return "Employee"; }
};

class Manager : public User {
public:
    Manager(const string& name, const string& pass);
    int getClearanceLevel() const override { return 3; }
    string getRoleName() const override { return "Manager"; }
};

class Director : public User {
public:
    Director(const string& name, const string& pass);
    int getClearanceLevel() const override { return 4; }
    string getRoleName() const override { return "Director"; }
};

class Executive : public User {
public:
    Executive(const string& name, const string& pass);
    int getClearanceLevel() const override { return 5; }
    string getRoleName() const override { return "Executive"; }
};

#endif
