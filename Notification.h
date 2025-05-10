#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <cstring>  // For strncpy, strlen
#include <ctime>    // For time_t
#include "User.h"
#include "AuditLogger.h"

// Notification types (unchanged)
struct NotificationType {
    static const int INFO = 0;
    static const int WARNING = 1;
    static const int EMERGENCY = 2;
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
    
    // Getters (unchanged)
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
    Notification** notifications;  // Dynamic array instead of vector<Notification*>
    int notificationCount;
    int notificationCapacity;
    AuditLogger logger;
    
    bool saveToFile(const Notification* notification) const;
    bool loadFromFile();
    bool validatePermissions(const User* sender, int type) const;
    void resizeNotifications();

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
