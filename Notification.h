#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <cstring>
#include <ctime>
#include "User.h"
#include "AuditLogger.h"
#include <vector>

struct NotificationType {
    static const int INFO = 0;     // Can be sent by anyone
    static const int WARNING = 1;   // Requires Manager level (3) or higher
    static const int EMERGENCY = 2; // Requires Director level (4) or higher
};

class Notification {
private:
    static const int MAX_CONTENT_LENGTH = 256;
    char sender[50];
    char content[MAX_CONTENT_LENGTH];
    int type;
    time_t timestamp;
    bool acknowledged;
    int targetClearanceLevel;
    
    void logNotification() const;

public:
    Notification(const User* sender, const char* content, int type, int targetLevel = 1);
    bool broadcast();
    bool acknowledge(const User* user);
    bool validateSenderPermission(const User* sender) const;
    
    // Getters
    const char* getSender() const { return sender; }
    const char* getContent() const { return content; }
    int getType() const { return type; }
    time_t getTimestamp() const { return timestamp; }
    bool isAcknowledged() const { return acknowledged; }
    const char* getTypeString() const;
    int getTargetLevel() const { return targetClearanceLevel; }
};

class NotificationSystem {
private:
    vector<Notification*> notifications;
    AuditLogger logger;
    
    bool saveToFile(const Notification* notification) const;
    bool loadFromFile();
    bool validatePermissions(const User* sender, int type) const;

public:
    NotificationSystem();
    ~NotificationSystem();
    
    bool sendNotification(const User* sender, const char* content, int type, int targetLevel = 1);
    void getUnacknowledgedNotifications(const User* user) const;
    bool acknowledgeNotification(const User* user, int notificationId);
    void broadcastEmergency(const User* sender, const char* content);
    int getPendingNotificationCount(const User* user) const;
    void displayUserNotifications(const User* user) const;
};

#endif
