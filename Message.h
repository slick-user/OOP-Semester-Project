#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstring>
#include <string>
#include <ctime>
#include "User.h"

struct MessageType {
    static const int INFO = 0;
    static const int PRIVATE = 1;
    static const int ALERT = 2;
};

class Message {
private:
    char senderName[50];
    char recipientName[50];
    int type;
    char content[256];
    time_t timestamp;
    bool encrypted;

    void encrypt();
    void decrypt();
    bool canSendAlert() const;
    static User* getUserByName(const char* name);

public:
    Message(User* sender, User* recipient, int msgType, const char* msgContent);
    bool send();
    bool readMessage(User* reader);
    const char* getTypeString() const;
    
    // Getters
    const char* getSender() const { return senderName; }
    const char* getRecipient() const { return recipientName; }
    int getType() const { return type; }
    time_t getTimestamp() const { return timestamp; }
};

class MessageSystem {
public:
    static bool sendMessage(User* sender, User* recipient, int type, const char* content);
    static void readInbox(User* user);
    static bool validateMessagePermissions(User* sender, User* recipient, int type);
    static void clearInbox(User* user);

private:
    static const char* INBOX_SUFFIX;
};

#endif