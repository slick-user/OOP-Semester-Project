#ifndef POLICYENGINE_H
#define POLICYENGINE_H

#include "PolicyEngine.h"

PolicyEngine* PolicyEngine::getEngine() {
  if (!engine)
    engine = new PolicyEngine;
  return engine;
}

bool PolicyEngine::getPermission(const string role, const string action) {
  
  // Matching Role with list of possible roles
  int i=0;
  bool role_match_found = false;
  for (i=0; i<5; i++) {
    if (role == clearanceLevels[i]) {
      role_match_found = true; 
      break;
    }
  }
  
  if (!role_match_found)
    return false;

  // Permissions
  if ( (i <= 0) && (action == "send_message") ) return true;
  if ( (i < 2) && (action == "send_alert") ) return true;
  if ( (i < 1) && (action == "send_info") ) return true;

  // This does not do the check for who it is sending it to but it should. I might have to add that functionality

  return false;
}

bool PolicyEngine::CheckClearance(const string role) {

  for (int i=0; i<5; i++) {
    if (role == clearanceLevels[i]) {
      return true;
    } 
  }

  cout << "Invalid Clearance Level";  
  return false;
}

#endif
