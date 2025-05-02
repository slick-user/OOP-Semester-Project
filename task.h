#ifndef TASK_H
#define TASK_H

#include <iostream>

using namespace std;

class Task {
private:
  int status; // FLAG for STATUS (1: CREATED, 2: ASSIGNED, 3: INPROGRESS,  4: COMPLETED, 5: EXPIRED)
  string name;
  string creator;
  string assignee;
  time_t TTL;
  time_t TimeStamp;
  string priority;

public:
  Task(string name, string creator, string assignee);

};

#endif 
