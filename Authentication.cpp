#include "Authentication.h"
#include "AuthUtils.h"
#include <fstream>
#include <iostream>
#include <cstring>

using namespace std;

User* Authentication::loginUserInteractive() {
    char username[HASH_SIZE], password[HASH_SIZE];
    
    for (int attempt = 1; attempt <= MAX_LOGIN_ATTEMPTS; ++attempt) {
        cout << "\nLogin Attempt (" << attempt << "/" << MAX_LOGIN_ATTEMPTS << ")\n";
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        User* user = loginUser(username, password);
        if (user != nullptr) {
            logAuthenticationAttempt(username, true);
            return user;
        }
        
        cout << "Invalid credentials. Try again.\n";
        logAuthenticationAttempt(username, false);
    }

    cout << "Maximum attempts reached. Access denied.\n";
    return nullptr;
}

User* Authentication::loginUser(const char* username, const char* password) {
    char hashedInput[HASH_SIZE];
    hashPassword(password, hashedInput);

    ifstream in("users.txt");
    if (!in.is_open()) {
        cerr << "Error opening users.txt\n";
        return nullptr;
    }

    char storedUser[HASH_SIZE], storedHash[HASH_SIZE];
    int storedClearance;

    while (in >> storedUser >> storedHash >> storedClearance) {
        if (strcmp(storedUser, username) == 0 && strcmp(storedHash, hashedInput) == 0) {
            in.close();
            return createUserByRole(username, password, storedClearance);
        }
    }
    
    in.close();
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

bool Authentication::registerUser(const char* username, const char* password, int clearanceLevel) {
    if (userExists(username)) {
        cout << "Username already exists.\n";
        return false;
    }

    if (clearanceLevel < 1 || clearanceLevel > 5) {
        cout << "Invalid clearance level. Must be between 1 and 5.\n";
        return false;
    }

    char hashed[HASH_SIZE];
    hashPassword(password, hashed);

    ofstream out("users.txt", ios::app);
    if (!out.is_open()) {
        cerr << "Error creating user file.\n";
        return false;
    }

    out << username << " " << hashed << " " << clearanceLevel << "\n";
    out.close();
    
    return true;
}

bool Authentication::changePassword(const char* username, const char* oldPassword, const char* newPassword) {
    char oldHashed[HASH_SIZE], newHashed[HASH_SIZE];
    hashPassword(oldPassword, oldHashed);
    hashPassword(newPassword, newHashed);

    ifstream in("users.txt");
    ofstream temp("temp.txt");
    
    if (!in.is_open() || !temp.is_open()) {
        cerr << "Error accessing user files.\n";
        return false;
    }

    char fileUser[HASH_SIZE], fileHash[HASH_SIZE];
    int clearance;
    bool updated = false;

    while (in >> fileUser >> fileHash >> clearance) {
        if (strcmp(fileUser, username) == 0 && strcmp(fileHash, oldHashed) == 0) {
            temp << fileUser << " " << newHashed << " " << clearance << "\n";
            updated = true;
        } else {
            temp << fileUser << " " << fileHash << " " << clearance << "\n";
        }
    }

    in.close();
    temp.close();

    if (updated) {
        remove("users.txt");
        rename("temp.txt", "users.txt");
    } else {
        remove("temp.txt");
    }

    return updated;
}

bool Authentication::userExists(const char* username) {
    ifstream in("users.txt");
    if (!in.is_open()) return false;

    char existingUser[HASH_SIZE], existingHash[HASH_SIZE];
    int existingClearance;

    while (in >> existingUser >> existingHash >> existingClearance) {
        if (strcmp(existingUser, username) == 0) {
            in.close();
            return true;
        }
    }
    
    in.close();
    return false;
}

void Authentication::logAuthenticationAttempt(const char* username, bool success) {
    ofstream log("auth_log.txt", ios::app);
    if (log.is_open()) {
        time_t now = time(nullptr);
        char* timestamp = ctime(&now);
        timestamp[strlen(timestamp) - 1] = '\0'; // Remove newline
        
        log << "[" << timestamp << "] User '" << username 
            << "' authentication " << (success ? "successful" : "failed") << "\n";
        
        log.close();
    }
}
