#include "Junior.h"

class Employee : public Junior {
  

public:
  Employee(string username, string password);

  void AssignTask();
  void CreateTask();
  void DelegateTask(); // Employee should not be able to delegate task.... probably?

};
