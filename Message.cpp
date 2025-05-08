#include "Message.h"
#include <fstream>
#include <iostream>
#include"AuditLogger.h"
#include "AuditAnomalyDetector.h"
#include "PerformanceReview.h"

const char* MessageSystem::INBOX_SUFFIX = "_inbox.txt";

Message::Message(User* sender, User* recipient, int msgType, const char* msgContent) {
    strncpy(senderName, sender->getName().c_str(), 49);
    strncpy(recipientName, recipient->getName().c_str(), 49);
    senderName[49] = recipientName[49] = '\0';
    type = msgType;
    strncpy(content, msgContent, 255);
    content[255] = '\0';
    timestamp = time(0);
    encrypted = false;
}

void Message::encrypt() {
    if (!encrypted) {
        for (int i = 0; content[i] != '\0'; ++i) {
            content[i] = content[i] + 3; // Simple Caesar cipher
        }
        encrypted = true;
    }
}

void Message::decrypt() {
    if (encrypted) {
        for (int i = 0; content[i] != '\0'; ++i) {
            content[i] = content[i] - 3;
        }
        encrypted = false;
    }
}
bool Message::send() {
    static AuditAnomalyDetector detector;
    AuditLogger logger(senderName);
    
    if (type == MessageType::ALERT && !canSendAlert()) {
        logger.logMessageAction("SEND_ALERT", recipientName, "DENIED_CLEARANCE");
        return false;
    }

    if (type == MessageType::PRIVATE) {
        encrypt();
    }

    string filename = string(recipientName) ;// + MessageSystem::INBOX_SUFFIX;
    ofstream out(filename.c_str(), ios::app);
    
    if (!out.is_open()) {
        logger.logMessageAction("SEND_MESSAGE", recipientName, "FAILED_FILE_ACCESS");
        return false;
    }

    out << "[" << timestamp << "] From: " << senderName 
        << " Type: " << getTypeString() 
        << " Content: " << content << endl;
    
    out.close();

    detector.checkMessagePattern(senderName, recipientName, time(0));
    logger.logMessageAction("SEND_MESSAGE", recipientName, "SUCCESS");
    
    // Update performance metrics
    PerformanceReview review(recipientName, 
        getUserByName(recipientName)->getClearanceLevel());
    review.addMessageMetric(time(0) - timestamp);
    
    return true;
}

bool Message::readMessage(User* reader) {
    AuditLogger logger(reader->getName().c_str());
    
    if (strcmp(reader->getName().c_str(), recipientName) != 0) {
        logger.logMessageAction("READ_MESSAGE", "Unauthorized access attempt", "DENIED");
        return false;
    }

    if (type == MessageType::PRIVATE) {
        decrypt();
    }

    cout << "Message from " << senderName << ":\n"
         << "Type: " << getTypeString() << "\n"
         << "Content: " << content << "\n";

    if (type == MessageType::PRIVATE) {
        encrypt();
    }

    logger.logMessageAction("READ_MESSAGE", senderName, "SUCCESS");
    return true;
}

const char* Message::getTypeString() const {
    switch (type) {
        case MessageType::INFO: return "INFO";
        case MessageType::PRIVATE: return "PRIVATE";
        case MessageType::ALERT: return "ALERT";
        default: return "UNKNOWN";
    }
}

bool Message::canSendAlert() const {
    User* sender = getUserByName(senderName);
    return sender && sender->getClearanceLevel() >= 3;
}

User* Message::getUserByName(const char* name) {
    // Implementation would connect to user database
    return nullptr;
}

// MessageSystem implementation
bool MessageSystem::sendMessage(User* sender, User* recipient, int type, const char* content) {
    if (!validateMessagePermissions(sender, recipient, type)) {
        return false;
    }

    Message msg(sender, recipient, type, content);
    return msg.send();
}

void MessageSystem::readInbox(User* user) {
    string filename = user->getName() + INBOX_SUFFIX;
    ifstream inbox(filename.c_str());
    
    if (!inbox.is_open()) {
        cout << "No messages in inbox.\n";
        return;
    }

    string line;
    while (getline(inbox, line)) {
        cout << line << endl;
    }
    inbox.close();
}

bool MessageSystem::validateMessagePermissions(User* sender, User* recipient, int type) {
    if (!sender || !recipient) return false;

    int senderLevel = sender->getClearanceLevel();
    int recipientLevel = recipient->getClearanceLevel();

    if (type == MessageType::ALERT) {
        return senderLevel >= 3;
    }

    if (type == MessageType::PRIVATE || type == MessageType::INFO) {
        return true;
    }

    return false;
}

void MessageSystem::clearInbox(User* user) {
    string filename = user->getName() + INBOX_SUFFIX;
    ofstream inbox(filename.c_str(), ios::trunc);
    inbox.close();
}

