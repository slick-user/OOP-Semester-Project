#ifndef JUNIOR_H
#define JUNIOR_H

#include "task.h"
#include <iostream>

using namespace std;

class Junior {
protected:
  string username;
  string password;
  string clearanceLevel;

public:
  // CONSTRUCTOR AND DESTRUCTOR
  Junior(string username, string password);
  virtual ~Junior();

  // GETTERS
  string getUsername();
  string getPassword();
  string getClearanceLevel();

  // SETTERS
  void setUsername(string username);
  void setPassword(string password);

  // AUTHENTICATION
  // Not done yet... on creation the log should add the credentials to the log... I also need to add Encryption the same way
  void login();
  void logout();

  // MESSAGING 
  // We could add the messaging as well won't be a problem
  virtual void sendMessage();
  virtual void recieveMessage();

  // TASK MANAGEMENT  
  void viewTasks();
  void createTask(string name, Junior& assignee);
  void delegateTask();

  // OPERATOR OVERLOADING
  bool operator==(const Junior& u1); 
};

#endif
