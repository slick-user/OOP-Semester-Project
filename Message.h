#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstring>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

enum MessageType { INFO, PRIVATE, ALERT };

class Message {
    char senderName[50];
    char recipientName[50];
    MessageType type;
    char content[256];

public:
    Message(const char* sender, const char* recipient, MessageType msgType, const char* msgContent);
    
    void encryptContent();
    void decryptContent();
    void sendMessage();

    // Getters
    const char* getSender() const { return senderName; }
    const char* getRecipient() const { return recipientName; }
    MessageType getType() const { return type; }
    const char* getContent() const { return content; }

    void setContent(const char* newContent) {
        strncpy(content, newContent, 255);
        content[255] = '\0'; // Ensure null termination
    }
};

#endif