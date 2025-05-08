#include "PolicyEngine.h"

bool PolicyEngine::canAccess(User* user, int requiredClearance) {
    return user->getClearanceLevel() >= requiredClearance;
}

bool PolicyEngine::canAccess(User* sender, User* recipient, int type) {
    int senderLevel = sender->getClearanceLevel();
    int recipientLevel = recipient->getClearanceLevel();

    switch (type) {
        case MessageType::INFO: return true;
        case MessageType::PRIVATE: return senderLevel >= recipientLevel;
        case MessageType::ALERT: return senderLevel >= recipientLevel;
        default: return false;
    }
}