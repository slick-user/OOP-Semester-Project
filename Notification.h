#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <map>
#include "User.h"

using namespace std;

enum NotificationType { WARNING, EMERGENCY };

class Notification {
public:
    Notification(const string& sender, NotificationType type, const string& message);
    void send();
    void logNotification();

private:
    string sender;
    NotificationType type;
    string message;
    string timestamp;

    string getTimestamp() const;
    string getTypeString() const;
};

class NotificationSystem {
public:
    static bool canSendNotification(User* sender, NotificationType type);
    static void sendNotification(User* sender, NotificationType type, const string& message);
    static void displayNotifications();  // Move this declaration here
};

#endif