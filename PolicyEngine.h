#ifndef POLICY_ENGINE_H
#define POLICY_ENGINE_H

#include "User.h"
#include "Message.h"

class PolicyEngine {
public:
    static bool canAccess(User* user, int requiredClearance);
    static bool canAccess(User* sender, User* recipient, MessageType type);
};

#endif