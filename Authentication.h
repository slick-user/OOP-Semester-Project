#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <string>
#include "User.h"
#include "UserRoles.h"

class Authentication {
public:
    // Interactive login with retry mechanism
    static User* loginUserInteractive();
    
    // Direct login with credentials
    static User* loginUser(const char* username, const char* password);
    
    // User management
    static bool registerUser(const char* username, const char* password, int clearanceLevel);
    static bool changePassword(const char* username, const char* oldPassword, const char* newPassword);
    
private:
    static const int MAX_LOGIN_ATTEMPTS = 3;
    static const int HASH_SIZE = 50;
    
    // Helper functions
    static User* createUserByRole(const char* username, const char* password, int clearanceLevel);
    static bool userExists(const char* username);
    static void logAuthenticationAttempt(const char* username, bool success);
};

#endif
