#include "PolicyEngine.h"

bool PolicyEngine::canAccess(User* user, int requiredClearance) {
    return user->getClearanceLevel() >= requiredClearance;
}

bool PolicyEngine::canAccess(User* sender, User* recipient, MessageType type) {
    int senderLevel = sender->getClearanceLevel();
    int recipientLevel = recipient->getClearanceLevel();

    switch (type) {
        case INFO: return true;
        case PRIVATE: return senderLevel >= recipientLevel;
        case ALERT: return senderLevel >= recipientLevel;
        default: return false;
    }
}