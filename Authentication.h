#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include "User.h"
#include "UserRoles.h"
#include "MFA.h"
#include <string>

class Authentication {
private:
    static const int MAX_LOGIN_ATTEMPTS = 3;
    static const int HASH_SIZE = 64;
    static char attemptLogFile[];  // Declaration of static member

    // Static private helper methods
    static void hashPassword(const char* input, char* output);
    static void logFailedAttempt(const char* username);
    static bool isLockedOut(const char* username);
    static bool verifyCredentials(const char* username, const char* password);
    static User* createUserByRole(const char* username, const char* password, int clearanceLevel);

public:
    static User* loginUser(const char* username, const char* password);
    static bool registerUser(const char* username, const char* password, int clearanceLevel);
    static bool changePassword(const char* username, const char* oldPassword, const char* newPassword);
};

#endif