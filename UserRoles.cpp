#include "UserRoles.h"

Junior::Junior(const string& name, const string& pass) : User(name, pass) {}
Employee::Employee(const string& name, const string& pass) : User(name, pass) {}
Manager::Manager(const string& name, const string& pass) : User(name, pass) {}
Director::Director(const string& name, const string& pass) : User(name, pass) {}
Executive::Executive(const string& name, const string& pass) : User(name, pass) {}