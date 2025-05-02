#include "Junior.h"

Junior::Junior(string username, string password) : username(username), password(password) {
  clearanceLevel = "Junior";
}

// GETTERS
string Junior::getUsername() { return username; }
string Junior::getPassword() { return password; }
string Junior::getClearanceLevel()   { return clearanceLevel; }

// SETTERS
void Junior::setUsername(string username) { this->username = username; }
void Junior::setPassword(string password) { this->password = password; } // need to save these into txt file

// TASK CREATION
void Junior::createTask(string name, Junior& assignee) { 
  Task t(name, username, assignee.getUsername()); 
} 

// OPERATOR OVERLOADING
bool Junior::operator==(const Junior& u1) {
  if ( (username == u1.username) && (password == u1.password) && (clearanceLevel == u1.clearanceLevel) ) {
    return true;
  }
  return false;
}

