#ifndef SECURITY_FEATURES_H
#define SECURITY_FEATURES_H

#include <cstring>
#include <ctime>
#include "User.h"
#include "Task.h"

class DigitalSignature {
private:
    static const int SIGNATURE_LENGTH = 64;
    char signature[SIGNATURE_LENGTH];
    
    static unsigned long generateHash(const char* data);

public:
    static void sign(const char* username, time_t timestamp, char* output);
    static bool verify(const char* signature, const char* username, time_t timestamp);
};

class CycleDetector {
private:
    static const int MAX_CHAIN = 20;
    char delegationChain[MAX_CHAIN][50];
    int chainLength;

public:
    CycleDetector();
    bool addDelegation(const char* username);
    bool hasCycle() const;
    void clear();
};

class EncryptedLogger {
private:
    static const char* LOG_FILE;
    char key[16];
    
    void encrypt(char* data) const;
    void decrypt(char* data) const;

public:
    EncryptedLogger();
    void writeLog(const char* entry);
    void readLog() const;
};

class ASCIIDashboard {
private:
    static const int SCREEN_WIDTH = 80;
    static const int SCREEN_HEIGHT = 24;
    
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH + 1];
    void drawBox(int x, int y, int width, int height);
    void writeAt(int x, int y, const char* text);

public:
    ASCIIDashboard();
    void clear();
    void updateTaskSection(Task** tasks, int count);
    void updateUserSection(User* currentUser);
    void updateStatsSection(const char* stats);
    void render() const;
};

#endif
