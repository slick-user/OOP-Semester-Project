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
  void login();
  void logout();

  // MESSAGING
  virtual void sendMessage();
  virtual void recieveMessage();

  // TASK MANAGEMENT  
  void viewTasks();

  // OPERATOR OVERLOADING
  bool operator==(const Junior& u1); 
};
