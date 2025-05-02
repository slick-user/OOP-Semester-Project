#ifndef POLICYENGINE_H
#define POLICYENGINE_H

#include <iostream>

using namespace std;

class PolicyEngine {
private:
  static PolicyEngine* engine;
 
  string clearanceLevels[5] = {"Junior", "Employee", "Manager", "Director", "Executive"};

public:
  
  bool getPermission(const string role, const string action);
  bool CheckClearance(const string role);

  static PolicyEngine* getEngine();

};

#endif
