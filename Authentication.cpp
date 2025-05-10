#include "Authentication.h"
#include <fstream>
#include <cstring>
#include <ctime>
#include <iostream>
#include "AuditLogger.h"
#include "AuditAnomalyDetector.h"

using namespace std;

// Define static member
char Authentication::attemptLogFile[] = "login_attempts.txt";

void Authentication::hashPassword(const char* input, char* output) {
    unsigned long hash = 5381;
    int c;
    while ((c = *input++)) {
        hash = ((hash << 5) + hash) + c;
    }
    snprintf(output, HASH_SIZE, "%lu", hash);
}

void Authentication::logFailedAttempt(const char* username) {
    ofstream log(attemptLogFile, ios::app);
    if (log.is_open()) {
        time_t now = time(0);
        log << username << " " << now << endl;
        log.close();
    }
}

bool Authentication::isLockedOut(const char* username) {
    ifstream log(attemptLogFile);
    if (!log.is_open()) return false;

    char storedUser[50];
    time_t attemptTime;
    int failCount = 0;
    time_t now = time(0);

    while (log >> storedUser >> attemptTime) {
        if (strcmp(storedUser, username) == 0) {
            if (difftime(now, attemptTime) < 1800) { // 30-minute lockout
                failCount++;
            }
        }
    }
    log.close();
    return failCount >= MAX_LOGIN_ATTEMPTS;
}

bool Authentication::registerUser(const char* username, const char* password, int clearanceLevel) {
    ifstream checkFile("users.txt");
    char storedUser[50], storedHash[HASH_SIZE];
    int storedLevel;

    while (checkFile >> storedUser >> storedHash >> storedLevel) {
        if (strcmp(storedUser, username) == 0) {
            checkFile.close();
            return false;
        }
    }
    checkFile.close();

    char hashedPassword[HASH_SIZE];
    hashPassword(password, hashedPassword);

    ofstream userFile("users.txt", ios::app);
    if (!userFile.is_open()) return false;

    userFile << username << " " << hashedPassword << " " << clearanceLevel << endl;
    userFile.close();
    return true;
}

bool Authentication::verifyCredentials(const char* username, const char* password) {
    char hashedInput[HASH_SIZE];
    hashPassword(password, hashedInput);

    ifstream userFile("users.txt");
    if (!userFile.is_open()) return false;

    char storedUser[50], storedHash[HASH_SIZE];
    int clearanceLevel;

    while (userFile >> storedUser >> storedHash >> clearanceLevel) {
        if (strcmp(storedUser, username) == 0 && 
            strcmp(storedHash, hashedInput) == 0) {
            userFile.close();
            return true;
        }
    }
    userFile.close();
    return false;
}

User* Authentication::findUser(const char* username) {
  char hashedInput[HASH_SIZE];

  ifstream userFile("users.txt");
  if (!userFile.is_open()) return nullptr;

  char storedUser[50], storedHash[HASH_SIZE];
  int clearanceLevel;
  while (userFile >> storedUser >> storedHash >> clearanceLevel) {
    if (strcmp(storedUser, username) == 0) {
        userFile.close();
        return createUserByRole(username, storedHash, clearanceLevel);
    }
  }
  userFile.close();
  return nullptr;
}

User* Authentication::createUserByRole(const char* username, const char* password, int clearanceLevel) {
    switch (clearanceLevel) {
        case 1: return new Junior(username, password);
        case 2: return new Employee(username, password);
        case 3: return new Manager(username, password);
        case 4: return new Director(username, password);
        case 5: return new Executive(username, password);
        default: return nullptr;
    }
}

bool Authentication::changePassword(const char* username, const char* oldPassword, const char* newPassword) {
    if (!verifyCredentials(username, oldPassword)) {
        return false;
    }

    char newHash[HASH_SIZE];
    hashPassword(newPassword, newHash);

    ifstream inFile("users.txt");
    ofstream outFile("users_temp.txt");
    
    char storedUser[50], storedHash[HASH_SIZE];
    int clearanceLevel;

    while (inFile >> storedUser >> storedHash >> clearanceLevel) {
        if (strcmp(storedUser, username) == 0) {
            outFile << username << " " << newHash << " " << clearanceLevel << endl;
        } else {
            outFile << storedUser << " " << storedHash << " " << clearanceLevel << endl;
        }
    }

    inFile.close();
    outFile.close();

    remove("users.txt");
    rename("users_temp.txt", "users.txt");
    return true;
}

User* Authentication::loginUser(const char* username, const char* password) {
    AuditAnomalyDetector detector;
    AuditLogger logger(username);
    
    if (isLockedOut(username)) {
        detector.checkLoginPattern(username, time(0));
        logger.logSecurityEvent("LOGIN_ATTEMPT", "Account locked", "DENIED");
        cout << "Account is locked due to multiple failed attempts.\n";
        cout << "Please try again after 30 minutes.\n";
        return nullptr;
    }

    if (!verifyCredentials(username, password)) {
        detector.checkLoginPattern(username, time(0));
        logger.logSecurityEvent("LOGIN_ATTEMPT", "Invalid credentials", "FAILED");
        logFailedAttempt(username);
        cout << "Invalid credentials.\n";
        return nullptr;
    }

    MFA mfa(username);
    if (!mfa.generateAndStoreOTP()) {
        logger.logSecurityEvent("MFA_GENERATION", "Failed to generate OTP", "FAILED");
        return nullptr;
    }

    cout << "OTP has been generated. Please check your file.\n";
    cout << "Enter OTP: ";
    char inputOTP[7];
    cin >> inputOTP;

    if (!mfa.validateOTP(inputOTP)) {
        logger.logSecurityEvent("MFA_VALIDATION", "Invalid or expired OTP", "FAILED");
        logFailedAttempt(username);
        return nullptr;
    }

    ifstream userFile("users.txt");
    char storedUser[50], storedHash[HASH_SIZE];
    int clearanceLevel;

    while (userFile >> storedUser >> storedHash >> clearanceLevel) {
        if (strcmp(storedUser, username) == 0) {
            userFile.close();
            logger.logSecurityEvent("LOGIN", "User authenticated successfully", "SUCCESS");
            return createUserByRole(username, password, clearanceLevel);
        }
    }
    
    userFile.close();
    logger.logSecurityEvent("LOGIN_ATTEMPT", "User not found", "FAILED");
    return nullptr;
}
