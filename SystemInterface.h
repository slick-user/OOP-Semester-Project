#ifndef SYSTEM_INTERFACE_H
#define SYSTEM_INTERFACE_H

//#include <vector>
#include "Task.h"
#include "User.h"
#include "Notification.h"
#include "AuditLogger.h"
#include "Authentication.h"
#include "Message.h"
#include "PerformanceReview.h"
#include "AuditAnomalyDetector.h"
#include "SecurityFeatures.h"
#include "TimeManager.h"
#include "PolicyEngine.h"
#include "PriorityManager.h" 


class SystemInterface {
private:
    Task* tasks;
    int taskCount;
    User* currentUser;
    AuditLogger* auditLogger;
    NotificationSystem notificationSystem;
    AuditAnomalyDetector anomalyDetector;
    ASCIIDashboard dashboard;
    EncryptedLogger encryptedLogger;
    PriorityManager priorityManager; 
    // PriorityManager priorityManager; 

    // Helper functions
    void displayHeader();
    void displayLoginMenu();
    void displayMainMenu();
    void displayTaskManagement();
    void displayMessaging();
    void displaySecurity();
    void displayEmployeeManagement();
    void displayReports();
    
    bool hasPermission(int requiredLevel) const;
    User* loginUser();
    void handleLoginMenu();
    void handleTaskManagement();
    void handleMessaging();
    void handleSecurity();
    void handleEmployeeManagement();
    void handleReports();
    void updateDashboard();
    void checkAndExpireAllTasks();

    void addTask(Task newTask);

public:
    SystemInterface();
    ~SystemInterface();
    void start();
};

#endif // SYSTEM_INTERFACE_H
