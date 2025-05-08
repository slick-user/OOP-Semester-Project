#include "SecurityFeatures.h"
#include <iostream>
#include <fstream>

// Digital Signature Implementation
unsigned long DigitalSignature::generateHash(const char* data) {
    unsigned long hash = 5381;
    int c;
    while (c = *data++) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void DigitalSignature::sign(const char* username, time_t timestamp, char* output) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s%ld", username, timestamp);
    unsigned long hash = generateHash(buffer);
    snprintf(output, SIGNATURE_LENGTH, "%s-%lu", username, hash);
}

bool DigitalSignature::verify(const char* signature, const char* username, time_t timestamp) {
    char expectedSignature[SIGNATURE_LENGTH];
    sign(username, timestamp, expectedSignature);
    return strcmp(signature, expectedSignature) == 0;
}

// Cycle Detector Implementation
CycleDetector::CycleDetector() : chainLength(0) {
    memset(delegationChain, 0, sizeof(delegationChain));
}

bool CycleDetector::addDelegation(const char* username) {
    if (chainLength >= MAX_CHAIN) return false;
    
    // Check for cycle
    for (int i = 0; i < chainLength; i++) {
        if (strcmp(delegationChain[i], username) == 0) {
            return false;  // Cycle detected
        }
    }
    
    strncpy(delegationChain[chainLength], username, 49);
    delegationChain[chainLength][49] = '\0';
    chainLength++;
    return true;
}

bool CycleDetector::hasCycle() const {
    for (int i = 0; i < chainLength; i++) {
        for (int j = i + 1; j < chainLength; j++) {
            if (strcmp(delegationChain[i], delegationChain[j]) == 0) {
                return true;
            }
        }
    }
    return false;
}

void CycleDetector::clear() {
    chainLength = 0;
    memset(delegationChain, 0, sizeof(delegationChain));
}

// Encrypted Logger Implementation
const char* EncryptedLogger::LOG_FILE = "encrypted_log.bin";

EncryptedLogger::EncryptedLogger() {
    // Simple encryption key
    strncpy(key, "SecurityKey123456", sizeof(key) - 1);
    key[sizeof(key) - 1] = '\0';
}

void EncryptedLogger::encrypt(char* data) const {
    for (size_t i = 0; data[i] != '\0'; i++) {
        data[i] = data[i] ^ key[i % sizeof(key)];
    }
}

void EncryptedLogger::decrypt(char* data) const {
    encrypt(data); // XOR encryption is its own inverse
}

void EncryptedLogger::writeLog(const char* entry) {
    char buffer[1024];
    strncpy(buffer, entry, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    encrypt(buffer);
    
    ofstream log(LOG_FILE, ios::app | ios::binary);
    if (log.is_open()) {
        log.write(buffer, strlen(buffer));
        log << '\n';
        log.close();
    }
}

void EncryptedLogger::readLog() const {
    ifstream log(LOG_FILE, ios::binary);
    if (!log.is_open()) return;
    
    char buffer[1024];
    while (log.getline(buffer, sizeof(buffer))) {
        decrypt(buffer);
        cout << buffer << endl;
    }
    log.close();
}

// ASCII Dashboard Implementation
ASCIIDashboard::ASCIIDashboard() {
    clear();
}

void ASCIIDashboard::clear() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            screen[y][x] = ' ';
        }
        screen[y][SCREEN_WIDTH] = '\0';
    }
}

void ASCIIDashboard::drawBox(int x, int y, int width, int height) {
    // Draw corners
    screen[y][x] = '+';
    screen[y][x + width - 1] = '+';
    screen[y + height - 1][x] = '+';
    screen[y + height - 1][x + width - 1] = '+';
    
    // Draw horizontal lines
    for (int i = x + 1; i < x + width - 1; i++) {
        screen[y][i] = '-';
        screen[y + height - 1][i] = '-';
    }
    
    // Draw vertical lines
    for (int i = y + 1; i < y + height - 1; i++) {
        screen[i][x] = '|';
        screen[i][x + width - 1] = '|';
    }
}

void ASCIIDashboard::writeAt(int x, int y, const char* text) {
    size_t len = strlen(text);
    for (size_t i = 0; i < len && x + i < SCREEN_WIDTH; i++) {
        screen[y][x + i] = text[i];
    }
}

void ASCIIDashboard::updateTaskSection(Task** tasks, int count) {
    drawBox(1, 1, 40, 10);
    writeAt(3, 1, "[ Active Tasks ]");
    
    for (int i = 0; i < count && i < 8; i++) {
        char buffer[38];
        snprintf(buffer, sizeof(buffer), "- %s (%s)", 
                tasks[i]->getDescription(),
                tasks[i]->getPriorityString());
        writeAt(3, i + 2, buffer);
    }
}

void ASCIIDashboard::updateUserSection(User* currentUser) {
    drawBox(42, 1, 37, 5);
    writeAt(44, 1, "[ User Information ]");
    
    char buffer[35];
    snprintf(buffer, sizeof(buffer), "Name: %s", currentUser->getName().c_str());
    writeAt(44, 2, buffer);
    
    snprintf(buffer, sizeof(buffer), "Role: %s", currentUser->getRoleName().c_str());
    writeAt(44, 3, buffer);
}

void ASCIIDashboard::updateStatsSection(const char* stats) {
    drawBox(42, 6, 37, 5);
    writeAt(44, 6, "[ System Statistics ]");
    writeAt(44, 7, stats);
}

void ASCIIDashboard::render() const {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        cout << screen[y] << endl;
    }
}