#include <iostream>

using namespace std;

class PolicyEngine {
private:
  static PolicyEngine* engine;

public:
  
  bool getPermission(const string role, const string action);
  bool CheckClearance(const string role);

  static PolicyEngine* getEngine();

};

