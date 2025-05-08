#include "SystemInterface.h"
#include <iostream>

SystemInterface::SystemInterface() : currentUser(nullptr), auditLogger(nullptr) {}

SystemInterface::~SystemInterface() {
    delete tasks;
    delete auditLogger;
    delete currentUser;
}

bool SystemInterface::hasPermission(int requiredLevel) const {
    return currentUser && currentUser->getClearanceLevel() >= requiredLevel;
}

void SystemInterface::displayHeader() {
    cout << "\n====================================\n";
    cout << "      OSIM Security Task System      \n";
    cout << "====================================\n";
}

void SystemInterface::displayLoginMenu() {
    cout << "\n[1] Login\n";
    cout << "[2] Register New User\n";
    cout << "[3] Exit\n";
    cout << "Choice: ";
}

void SystemInterface::displayMainMenu() {
    cout << "\nLogged in as: " << currentUser->getName() 
         << " (" << currentUser->getRoleName() << ")\n";
    cout << "\n=== Main Menu ===\n";
    cout << "[1] Task Management\n";
    if (hasPermission(2)) cout << "[2] Messages & Notifications\n";
    if (hasPermission(3)) cout << "[3] Security & Audit\n";
    if (hasPermission(4)) cout << "[4] Employee Management\n";
    if (hasPermission(3)) cout << "[5] View Reports\n";
    cout << "[6] View Dashboard\n";
    cout << "[7] Logout\n";
    cout << "Choice: ";
}

void SystemInterface::handleTaskManagement() {
    while (true) {
        cout << "\n=== Task Management ===\n";
        cout << "[1] Create New Task\n";
        cout << "[2] View All Tasks\n";
        cout << "[3] Update Task Status\n";
        if (hasPermission(3)) cout << "[4] Assign Task\n";
        if (hasPermission(3)) cout << "[5] Set Task Priority\n";
        cout << "[6] View My Tasks\n";
        cout << "[7] Return to Main Menu\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                if (!hasPermission(2)) {
                    cout << "Permission denied.\n";
                    break;
                }
                string description;
                int priority, ttl;
                
                cout << "Enter task description: ";
                cin.ignore();
                getline(cin, description);
                cout << "Enter priority (0-High, 1-Medium, 2-Low): ";
                cin >> priority;
                cout << "Enter TTL in seconds: ";
                cin >> ttl;

                Task* newTask = new Task(currentUser, description.c_str(), ttl, priority);
                tasks.push_back(newTask);
              //  priorityManager.addTask(newTask);
                auditLogger->logTaskAction("CREATE", 
                    to_string(newTask->getId()).c_str(), "SUCCESS");
                updateDashboard();
                break;
            }
            // ... similar permission checks for other cases ...
        }
    }
}

void SystemInterface::handleMessaging() {
    if (!hasPermission(2)) {
        cout << "Permission denied.\n";
        return;
    }

    while (true) {
        cout << "\n=== Messages & Notifications ===\n";
        cout << "[1] Send Message\n";
        cout << "[2] View Inbox\n";
        if (hasPermission(3)) cout << "[3] Send System Notification\n";
        if (hasPermission(4)) cout << "[4] Send Emergency Alert\n";
        cout << "[5] View Notifications\n";
        cout << "[6] Return to Main Menu\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                string recipient, content;
                int type;
                cout << "Recipient username: ";
                cin >> recipient;
                cout << "Message type (0-INFO, 1-PRIVATE, 2-ALERT): ";
                cin >> type;
                
                if (type == MessageType::ALERT && !hasPermission(3)) {
                    cout << "Permission denied for sending alerts.\n";
                    break;
                }

                cout << "Content: ";
                cin.ignore();
                getline(cin, content);

                MessageSystem::sendMessage(currentUser, nullptr, type, content.c_str());
                break;
            }
            // ... similar permission checks for other cases ...
        }
    }
}

void SystemInterface::handleSecurity() {
    if (!hasPermission(3)) {
        cout << "Permission denied.\n";
        return;
    }

    while (true) {
        cout << "\n=== Security & Audit ===\n";
        cout << "[1] View Audit Logs\n";
        cout << "[2] Generate Anomaly Report\n";
        cout << "[3] Change Password\n";
        if (hasPermission(4)) cout << "[4] View Security Policies\n";
        if (hasPermission(5)) cout << "[5] Modify Security Settings\n";
        cout << "[6] Return to Main Menu\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        // ... implement with proper permission checks ...
    }
}

void SystemInterface::handleReports() {
    if (!hasPermission(3)) {
        cout << "Permission denied.\n";
        return;
    }

    while (true) {
        cout << "\n=== Reports ===\n";
        cout << "[1] Task Performance Report\n";
        cout << "[2] User Activity Report\n";
        if (hasPermission(4)) cout << "[3] Security Audit Report\n";
        if (hasPermission(4)) cout << "[4] System Statistics\n";
        cout << "[5] Return to Main Menu\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        // ... implement with proper permission checks ...
    }
}

void SystemInterface::start() {
    AuditLogger systemLogger("SYSTEM");
    systemLogger.logSecurityEvent("SYSTEM_START", "System initialized", "SUCCESS");

    while (true) {
        displayHeader();
        
        if (!currentUser) {
            handleLoginMenu();
        } else {
            displayMainMenu();
            int choice;
            cin >> choice;

            switch (choice) {
                case 1:
                    handleTaskManagement();
                    break;
                case 2:
                    if (hasPermission(2)) handleMessaging();
                    break;
                case 3:
                    if (hasPermission(3)) handleSecurity();
                    break;
                case 4:
                    if (hasPermission(4)) handleEmployeeManagement();
                    break;
                case 5:
                    if (hasPermission(3)) handleReports();
                    break;
                case 6:
                    dashboard.render();
                    break;
                case 7:
                    auditLogger->logLogout();
                    delete auditLogger;
                    auditLogger = nullptr;
                    currentUser = nullptr;
                    cout << "Logged out successfully!\n";
                    break;
                default:
                    cout << "Invalid choice!\n";
            }
        }

        // Periodic checks
        checkAndExpireAllTasks();
        anomalyDetector.generateAnomalyReport();
    }
}
void SystemInterface::updateDashboard() {
    if (!currentUser) return;

    // Update task section
    Task** activeTasks = new Task*[tasks.size()];
    int activeCount = 0;
    for (Task* task : tasks) {
        if (task->getStatus() != TaskStatus::COMPLETED && 
            task->getStatus() != TaskStatus::EXPIRED) {
            activeTasks[activeCount++] = task;
        }
    }
    dashboard.updateTaskSection(activeTasks, activeCount);
    delete[] activeTasks;

    // Update user section
    dashboard.updateUserSection(currentUser);

    // Update stats section
    char stats[100];
    snprintf(stats, sizeof(stats), "Active Tasks: %d\nPending Messages: %d", 
             activeCount, notificationSystem.getPendingNotificationCount(currentUser));
    dashboard.updateStatsSection(stats);
}

void SystemInterface::handleEmployeeManagement() {
    if (!hasPermission(4)) {
        cout << "Permission denied.\n";
        return;
    }

    // Add employee management implementation
    cout << "Employee management functionality not implemented yet.\n";
}

void SystemInterface::checkAndExpireAllTasks() {
    for (Task* task : tasks) {
        if (task) {
            task->checkExpiration();
        }
    }
}

void SystemInterface::handleLoginMenu() {
    displayLoginMenu();
    int choice;
    cin >> choice;

    switch (choice) {
        case 1: {
            string username, password;
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;
            currentUser = Authentication::loginUser(username.c_str(), password.c_str());
            if (currentUser) {
                auditLogger = new AuditLogger(currentUser->getName().c_str());
            }
            break;
        }
        case 2: {
            string username, password;
            int clearanceLevel;
            cout << "New username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;
            cout << "Clearance level (1-5): ";
            cin >> clearanceLevel;
            if (Authentication::registerUser(username.c_str(), password.c_str(), clearanceLevel)) {
                cout << "Registration successful!\n";
            }
            break;
        }
        case 3:
            exit(0);
    }
}


// Duplicate destructor removed
