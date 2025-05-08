#include "Message.h"

Message::Message(const char* sender, const char* recipient, MessageType msgType, const char* msgContent) {
    strncpy(senderName, sender, 49);
    strncpy(recipientName, recipient, 49);
    type = msgType;
    strncpy(content, msgContent, 255);
}

void Message::encryptContent() {
    for (int i = 0; content[i] != '\0'; ++i)
        content[i] += 3;
}

void Message::decryptContent() {
    for (int i = 0; content[i] != '\0'; ++i)
        content[i] -= 3;
}


// Add to Message.cpp
void Message::sendMessage() {
    string filename = string(recipientName) + "_inbox.txt";
    ofstream out(filename, ios::app);
    
    if (out.is_open()) {
        if (type == PRIVATE) {
            encryptContent();
        }
        
        out << "[" << senderName << "] [" 
            << (type == PRIVATE ? "PRIVATE" : type == ALERT ? "ALERT" : "INFO") 
            << "]: " << content << endl;
            
        if (type == PRIVATE) {
            decryptContent();  // Restore original content
        }
        
        out.close();
    }
}

// decryption logic


// void readInbox(User* currentUser) {
//     char filename[64];
//     strcpy(filename, currentUser->getName());
//     strcat(filename, "_inbox.txt");

//     ifstream in(filename);
//     if (!in.is_open()) {
//         cout << "No messages found.\n";
//         return;
//     }

//     cout << "\n--- Inbox for " << currentUser->getName() << " ---\n";

//     char line[512];
//     while (in.getline(line, 512)) {
//         // Simple parser: find message type
//         if (strstr(line, "[PRIVATE]")) {
//             // Decrypt since it's PRIVATE
//             char* start = strstr(line, "]: ");
//             if (start) {
//                 start += 3; // Skip past "]: "
//                 for (int i = 0; start[i] != '\0'; ++i) {
//                     start[i] -= 3; // Decrypt Caesar
//                 }
//             }
//         }
//         cout << line << endl;
//     }

//     in.close();
//     cout << "---------------------------\n";
// }
