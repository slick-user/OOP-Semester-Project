#include "Notification.h"

Notification::Notification(const string& sender, NotificationType type, const string& message)
    : sender(sender), type(type), message(message) {
    timestamp = getTimestamp();
}

void Notification::send() {
    cout << "[" << timestamp << "] " << sender << " sent a " << getTypeString() << " notification: " << message << endl;
    logNotification();
}

void Notification::logNotification() {
    ofstream notificationFile("notifications.txt", ios::app);
    if (notificationFile.is_open()) {
        notificationFile << "[" << timestamp << "] " << sender << " sent a " << getTypeString() << " notification: " << message << endl;
        notificationFile.close();
    }
}

string Notification::getTimestamp() const {
    time_t now = time(0);
    char* dt = ctime(&now);
    return string(dt);
}

string Notification::getTypeString() const {
    switch (type) {
        case WARNING: return "WARNING";
        case EMERGENCY: return "EMERGENCY";
        default: return "UNKNOWN";
    }
}

void NotificationSystem::sendNotification(User* sender, NotificationType type, const string& message) {
    if (canSendNotification(sender, type)) {
        Notification notification(sender->getName(), type, message);
        notification.send();
    } else {
        cout << "ACCESS DENIED: " << sender->getName() << " does not have permission to send " << (type == EMERGENCY ? "EMERGENCY" : "WARNING") << " notifications." << endl;
    }
}

bool NotificationSystem::canSendNotification(User* sender, NotificationType type) {
    // Only Managers and Executives can send EMERGENCY notifications
    if (type == EMERGENCY) {
        if (sender->getClearanceLevel() >= 4) {  // Clearance level 4 corresponds to Manager or higher
            return true;
        } else {
            return false;
        }
    }
    // All roles can send WARNING notifications
    return true;
}

void NotificationSystem::displayNotifications() {
    try {
        ifstream notificationFile("notifications.txt");
        if (!notificationFile.is_open()) {
            cout << "\n=== Notifications ===\n";
            cout << "No notifications found.\n";
            cout << "===================\n";
            return;
        }

        cout << "\n=== Notifications ===\n";
        string line;
        bool hasNotifications = false;
        while (getline(notificationFile, line)) {
            cout << line << endl;
            hasNotifications = true;
        }
        if (!hasNotifications) {
            cout << "No notifications available.\n";
        }
        cout << "===================\n";
        notificationFile.close();
    } catch (const exception& e) {
        cerr << "Error reading notifications: " << e.what() << endl;
    }
}