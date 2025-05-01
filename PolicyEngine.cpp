#ifndef POLICYENGINE_H
#define POLICYENGINE_H

#include "PolicyEngine.h"

PolicyEngine* PolicyEngine::getEngine() {
  if (!engine)
    engine = new PolicyEngine;
  return engine;
}

bool PolicyEngine::getPermission(const string role, const string action) {
  if ( (role == "Junior") && (action == "send_message") ) return true;

  return false;
}

bool PolicyEngine::CheckClearance(const string role) {
  string clearanceLevels[5] = {"Junior", "Employee", "Manager", "Director", "Executive"};

  for (int i=0; i<5; i++) {
    if (role == clearanceLevels[i]) {
      return true;
    } 
  }

  cout << "Invalid Clearance Level";  
  return false;
}

#endif
