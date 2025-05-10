#include "Notification.h"
#include <fstream>
#include <iostream>

Notification::Notification(const User* sender, const char* content, int type, int targetLevel) 
    : type(type), timestamp(time(0)), acknowledged(false), targetClearanceLevel(targetLevel)
{
    strncpy(this->sender, sender->getName().c_str(), 49);
    this->sender[49] = '\0';
    strncpy(this->content, content, MAX_CONTENT_LENGTH - 1);
    this->content[MAX_CONTENT_LENGTH - 1] = '\0';
}

bool Notification::validateSenderPermission(const User* sender) const {
    if (!sender) return false;
    
    int clearanceLevel = sender->getClearanceLevel();
    
    switch (type) {
        case NotificationType::INFO:
            return true;  // Anyone can send INFO
        case NotificationType::WARNING:
            return clearanceLevel >= 3;  // Manager and above
        case NotificationType::EMERGENCY:
            return clearanceLevel >= 4;  // Director and above
        default:
            return false;
    }
}

void Notification::logNotification() const {
    AuditLogger logger(sender);
    char details[300];
    snprintf(details, sizeof(details), "Type: %s, Content: %s, Target Level: %d", 
             getTypeString(), content, targetClearanceLevel);
    logger.logSecurityEvent("NOTIFICATION", details, "BROADCAST");
}

bool Notification::broadcast() {
    logNotification();
    return true;
}

bool Notification::acknowledge(const User* user) {
    if (!user || user->getClearanceLevel() < targetClearanceLevel) {
        return false;
    }
    acknowledged = true;
    return true;
}

const char* Notification::getTypeString() const {
    switch (type) {
        case NotificationType::INFO: return "INFO";
        case NotificationType::WARNING: return "WARNING";
        case NotificationType::EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

// NotificationSystem Implementation
NotificationSystem::NotificationSystem() : logger("NotificationSystem"), notifications(new Notification*[10]), notificationCount(0), notificationCapacity(10) {
    loadFromFile();
}

NotificationSystem::~NotificationSystem() {
  for (int i = 0; i < notificationCount; i++) {
    delete notifications[i];
  }
  delete[] notifications;
}

void NotificationSystem::resizeNotifications() {
    int newCapacity = notificationCapacity * 2;
    Notification** newArray = new Notification*[newCapacity];
    
    for (int i = 0; i < notificationCount; i++) {
        newArray[i] = notifications[i];
    }
    
    delete[] notifications;
    notifications = newArray;
    notificationCapacity = newCapacity;
}

bool NotificationSystem::validatePermissions(const User* sender, int type) const {
    if (!sender) return false;
    
    switch (type) {
        case NotificationType::INFO:
            return true;
        case NotificationType::WARNING:
            return sender->getClearanceLevel() >= 3;
        case NotificationType::EMERGENCY:
            return sender->getClearanceLevel() >= 4;
        default:
            return false;
    }
}

bool NotificationSystem::sendNotification(const User* sender, const char* content, int type, int targetLevel) {
    if (!validatePermissions(sender, type)) {
        logger.logSecurityEvent("SEND_NOTIFICATION", "Permission denied", "FAILED");
        return false;
    }
    
    if (notificationCount == notificationCapacity) {
        resizeNotifications();
    }
    
    notifications[notificationCount++] = new Notification(sender, content, type, targetLevel);
    saveToFile(notifications[notificationCount - 1]);
    logger.logSecurityEvent("SEND_NOTIFICATION", "Notification broadcast", "SUCCESS");
    return true;
}

void NotificationSystem::displayUserNotifications(const User* user) const {
    cout << "\n=== Notifications for " << user->getName() << " ===\n";
    bool hasNotifications = false;
    
    for (int i = 0; i < notificationCount; i++) {
        if (user->getClearanceLevel() >= notifications[i]->getTargetLevel()) {
            cout << "\nNotification #" << i + 1 << ":\n"
                 << "Type: " << notifications[i]->getTypeString() << "From: " << notifications[i]->getSender() << "\n"
                 << "Content: " << notifications[i]->getContent() << "\n"
                 << "Status: " << (notifications[i]->isAcknowledged() ? 
                                 "Acknowledged" : "Unacknowledged") << "\n"
                 << "-------------------\n";
            hasNotifications = true;
        }
    }
    
    if (!hasNotifications) {
        cout << "No notifications available.\n";
    }
}

bool NotificationSystem::saveToFile(const Notification* notification) const {
    ofstream file("notifications.txt", ios::app);
    if (!file.is_open()) return false;
    
    file << "[" << notification->getTimestamp() << "] "
         << notification->getTypeString() << " from "
         << notification->getSender() << " (Level " 
         << notification->getTargetLevel() << "): "
         << notification->getContent() << "\n";
    
    file.close();
    return true;
}

bool NotificationSystem::loadFromFile() {
    ifstream file("notifications.txt");
    if (!file.is_open()) return false;
    
    string line;
    while (getline(file, line)) {
    }
    
    file.close();
    return true;
}

void NotificationSystem::broadcastEmergency(const User* sender, const char* content) {
    if (sender->getClearanceLevel() >= 4) { // Director or above
        sendNotification(sender, content, NotificationType::EMERGENCY, 1);
    }
}

void NotificationSystem::getUnacknowledgedNotifications(const User* user) const {
    cout << "\n=== Unacknowledged Notifications ===\n";
    bool hasUnacknowledged = false;
    
    for (int i = 0; i < notificationCount; i++) {
        if (!notifications[i]->isAcknowledged() && 
            user->getClearanceLevel() >= notifications[i]->getTargetLevel()) {
            cout << "Notification #" << i + 1 << ": "
                 << notifications[i]->getContent() << "\n";
            hasUnacknowledged = true;
        }
    }
    
    if (!hasUnacknowledged) {
        cout << "No unacknowledged notifications.\n";
    }
}

bool NotificationSystem::acknowledgeNotification(const User* user, int notificationId) {
    if (notificationId < 1 || notificationId > static_cast<int>(notificationCount)) 
        return false;
    
    return notifications[notificationId - 1]->acknowledge(user);
}

int NotificationSystem::getPendingNotificationCount(const User* user) const {
    int count = 0;
    for (int i = 0; i < notificationCount; i++) {
        if (!notifications[i]->isAcknowledged() && 
            user->getClearanceLevel() >= notifications[i]->getTargetLevel()) {
            count++;
        }
    }
    return count;
}
