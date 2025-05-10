#include "SystemInterface.h"
#include "Audit.h"
#include "Authentication.h"
#include <iostream>

SystemInterface::SystemInterface() : currentUser(nullptr), auditLogger(nullptr) {
  tasks = new Task*[10];
  taskCapacity = 10;
  taskCount = 0;
}

SystemInterface::~SystemInterface() {
    for (int i=0; i < taskCount; i++) {  
      delete tasks[i];
    }
    delete[] tasks;
    delete auditLogger;
    delete currentUser;
}

void SystemInterface::resizeTaskArray() {
    int newCapacity = taskCapacity * 2;
    Task** newArray = new Task*[newCapacity];
    
    for (int i = 0; i < taskCount; i++) {
        newArray[i] = tasks[i];
    }
    
    delete[] tasks;
    tasks = newArray;
    taskCapacity = newCapacity;
}

void SystemInterface::addTask(Task* newTask) {
    if (taskCount == taskCapacity) {
        resizeTaskArray();
    }
    tasks[taskCount++] = newTask;
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
                addTask(newTask);
                priorityManager.addTask(newTask);
                auditLogger->logTaskAction("CREATE", 
                    to_string(newTask->getId()).c_str(), "SUCCESS");
                updateDashboard();
                break;
            }
            case 2: {       
                cout << "\n=== All Tasks ===\n";
                if (taskCount == 0) {
                    cout << "No tasks available.\n";
                } else {
                    for (int i = 0; i < taskCount; i++) {
                        cout << "[" << i+1 << "] ";
                        priorityManager.displayTasksByPriority();
                    }
                }
                break;
            }
            case 3: {
                if (taskCount == 0) {
                    cout << "No tasks available to update.\n";
                    break;
                }
                
                cout << "Enter task number to update: ";
                int taskNum;
                cin >> taskNum;
                
                if (taskNum < 1 || taskNum > taskCount) {
                    cout << "Invalid task number.\n";
                    break;
                }
                
                Task* selectedTask = tasks[taskNum-1];
                
                // Check if user has permission to update this task
                if (selectedTask->getCreator() != currentUser && !hasPermission(3)) {
                    cout << "Permission denied: You can only update your own tasks.\n";
                    break;
                }
                
                cout << "Current status: " << selectedTask->getStatusString() << endl;
                cout << "Select new status:\n";
                cout << "[0] PENDING\n[1]Assigned\n[2] IN_PROGRESS\n[3] COMPLETED\n";
                cout << "Choice: ";
                
                int statusChoice;
                cin >> statusChoice;
                
                if (statusChoice < 0 || statusChoice > 3) {
                    cout << "Invalid status choice.\n";
                    break;
                }
              
                if (statusChoice == 3) {
                  selectedTask->completeTask(currentUser);
                }

                selectedTask->updateStatus(statusChoice);
                auditLogger->logTaskAction("UPDATE", 
                    to_string(selectedTask->getId()).c_str(), "SUCCESS");
                updateDashboard();
                cout << "Task status updated successfully.\n";
                break;
            }
            case 4: {
                if (!hasPermission(3)) {
                    cout << "Permission denied.\n";
                    break;
                }
                
                if (taskCount == 0) {
                    cout << "No tasks available to assign.\n";
                    break;
                }
                
                cout << "Enter task number to assign: ";
                int taskNum;
                cin >> taskNum;
                
                if (taskNum < 1 || taskNum > taskCount) {
                    cout << "Invalid task number.\n";
                    break;
                }
                
                // Display list of users to assign to
                cout << "Enter username to assign task to: ";
                string username;
                cin >> username;
                
                User* assignee = Authentication::findUser(username.c_str());
                if (!assignee) {
                    cout << "User not found.\n";
                    break;
                }
                
                tasks[taskNum-1]->assignTo(currentUser, assignee);
                auditLogger->logTaskAction("ASSIGN", 
                    to_string(tasks[taskNum-1]->getId()).c_str(), "SUCCESS");
                updateDashboard();
                
                // Send notification to the user
                notificationSystem.sendNotification(assignee, "Task assigned to you", 1);
        
                cout << "Task assigned successfully.\n";
                break;
            }
            case 5: {
                if (!hasPermission(3)) {
                    cout << "Permission denied.\n";
                    break;
                }
                
                if (taskCount == 0) {
                    cout << "No tasks available to set priority.\n";
                    break;
                }
                
                cout << "Enter task number to update priority: ";
                int taskNum;
                cin >> taskNum;
                
                if (taskNum < 1 || taskNum > taskCount) {
                    cout << "Invalid task number.\n";
                    break;
                }
                
                cout << "Enter new priority (0-High, 1-Medium, 2-Low): ";
                int priority;
                cin >> priority;
                
                if (priority < 0 || priority > 2) {
                    cout << "Invalid priority level.\n";
                    break;
                }
                
                tasks[taskNum-1]->setPriority(priority);
                priorityManager.updateTaskPriority(currentUser, tasks[taskNum-1], priority);
                auditLogger->logTaskAction("UPDATE_PRIORITY", to_string(tasks[taskNum-1]->getId()).c_str(), "SUCCESS");
                updateDashboard();
                
                cout << "Task priority updated successfully.\n";
                break;
            }
            case 6: {
                cout << "\n=== My Tasks ===\n";
                bool foundTasks = false;
                
                for (int i = 0; i < taskCount; i++) {
                    if (tasks[i]->getCreator() == currentUser || 
                        tasks[i]->getAssignee() == currentUser) {
                        cout << "[" << i+1 << "] ";
                        // we are not getting the guys tasks... so that is an issue
                        priorityManager.displayTasksByPriority();
                        foundTasks = true;
                    }
                }
                
                if (!foundTasks) {
                    cout << "You have no tasks assigned.\n";
                }
                break;
            }
            case 7:
                return;
            default:
                cout << "Invalid choice!\n";
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

                User* rec = Authentication::findUser(recipient.c_str());

                cout << "Content: ";
                cin.ignore();
                getline(cin, content);

                MessageSystem::sendMessage(currentUser, rec, type, content.c_str());
                break;
            }
            case 2: {
                string filename = currentUser->getName() + "_inbox.txt";
                ifstream inbox(filename);
                if (inbox.is_open())
                {
                    string line;
                    cout << "\n=== Your Inbox ===\n";
                    while (getline(inbox, line))
                    {
                        cout << line << endl;
                    }
                inbox.close();
                }
                else
                {
                    cout << "No messages in inbox.\n";
                }
                break;
            }
            case 3: {
                string message;
                int type;
                int targetlevel;
                
                cout << "System Noification Type (0-INFO, 1-WARNING)";
                cin >> type;
                
                if (type > 1) { cout << "invalid type!"; break;}

                cout << "Target Level (0-JUNIOR, 1-EMPLOYEE, 2-MANAGER, 3-DIRECTOR, 4-EXECUTIVE): ";
                cin >> targetlevel;

                cout << "Notification message: ";
                cin.ignore();
                getline(cin, message); 
                notificationSystem.sendNotification(currentUser, message.c_str(), type, targetlevel);
                break;
            }
            case 4: {
                string message;
                int targetlevel;

                cout << "Target Level (0-JUNIOR, 1-EMPLOYEE, 2-MANAGER, 3-DIRECTOR, 4-EXECUTIVE): ";
                cin >> targetlevel;

                cout << "Notification message: ";
                cin.ignore();
                getline(cin, message); 
                notificationSystem.sendNotification(currentUser, message.c_str(), NotificationType::EMERGENCY, targetlevel);
                break;
            }
            case 5:
                notificationSystem.displayUserNotifications(currentUser);
                break;
            case 6:
                return;
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
        //if (hasPermission(5)) cout << "[5] Modify Security Settings\n";
        cout << "[5] Return to Main Menu\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            
            case 1:
                //AuditLog::readLogs();
                break;
            
            case 2:
                anomalyDetector.generateAnomalyReport();
                break;
            
            case 3: {
                string oldPass, newPass;
                cout << "Enter old password: ";
                cin >> oldPass;
                cout << "Enter new password: ";
                cin >> newPass;
                if (Authentication::changePassword(currentUser->getName().c_str(), 
                                                  oldPass.c_str(), newPass.c_str())) {
                    auditLogger->logSecurityEvent("CHANGE_PASSWORD", "Password Changed", 
                                                  "SUCCESS");
                    cout << "Password Changed Successfully!" << endl;
                }
                else {
                    auditLogger->logSecurityEvent("CHANGE_PASSWORD ", "Password change failed", 
                                                  "FAILURE");
                    cout << "Failed to change the password" << endl;
                }
                break;
            }
            case 4: {

                cout << "\n=== Security Policies ===\n";
                cout << "1. Password Requirements:\n";
                cout << "   - Minimum 8 characters\n";
                cout << "   - Must contain uppercase and lowercase letters\n";
                cout << "   - Must contain numbers\n";
                cout << "   - Must contain special characters\n\n";

                cout << "2. Access Levels:\n";
                cout << "   - Level 1: Junior (Basic access)\n";
                cout << "   - Level 2: Employee (Standard access)\n";
                cout << "   - Level 3: Manager (Elevated access)\n";
                cout << "   - Level 4: Director (High access)\n";
                cout << "   - Level 5: Executive (Full access)\n\n";

                cout << "3. Message Security:\n";
                cout << "   - Private messages are encrypted\n";
                cout << "   - Alerts require elevated clearance\n";
                cout << "   - System logs all message activities\n\n";

                cout << "4. Audit Policy:\n";
                cout << "   - All actions are logged\n";
                cout << "   - Failed attempts are monitored\n";
                cout << "   - Anomaly detection is active\n";
                break;
            }
            case 5: {
                return;
            }

        }
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
        
        switch (choice) {
            case 1: {
                cout << "\n=== Task Performance Report ===\n";
                int completed = 0, inProgress = 0, assigned = 0, expired = 0;
                
                for (int i = 0; i < taskCount; i++) {
                    switch (tasks[i]->getStatus()) {
                        case TaskStatus::COMPLETED:
                            completed++;
                            break;
                        case TaskStatus::IN_PROGRESS:
                            inProgress++;
                            break;
                        case TaskStatus::ASSIGNED:
                            assigned++;
                            break;
                        case TaskStatus::EXPIRED:
                            expired++;
                            break;
                    }
                }
                
                cout << "Total Tasks: " << taskCount << endl;
                cout << "Completed: " << completed << endl;
                cout << "In Progress: " << inProgress << endl;
                cout << "Assigned: " << assigned << endl;
                cout << "Expired: " << expired << endl;
                
                float completionRate = taskCount > 0 ? 
                    (float)completed / taskCount * 100 : 0;
                cout << "Task Completion Rate: " << completionRate << "%\n";
                break;
            }
            case 2: {
                cout << "\n=== User Activity Report ===\n";
                //AuditLog::readLogs();
                break;
            }
            case 3: {
                if (!hasPermission(4)) {
                    cout << "Permission denied.\n";
                    break;
                }
                cout << "\n=== Security Audit Report ===\n";
                anomalyDetector.generateAnomalyReport();
                break;
            }
            case 4: {
                if (!hasPermission(4)) {
                    cout << "Permission denied.\n";
                    break;
                }
                cout << "\n=== System Statistics ===\n";
                cout << "Total Tasks: " << taskCount << endl;
                //cout << "Active Tasks: " << (taskCount - getExpiredTaskCount()) << endl;
                //cout << "System Uptime: " << getSystemUptime() << " seconds\n";
                break;
            }
            case 5:
                return;
            default:
                cout << "Invalid choice!\n"; 
        }
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

    int activeCount = 0;
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i] && 
            tasks[i]->getStatus() != TaskStatus::COMPLETED && 
            tasks[i]->getStatus() != TaskStatus::EXPIRED) {
            activeCount++;
        }
    }

    Task** activeTasks = new Task*[taskCount];  
    int index = 0;
    for (int i=0; i < taskCount; i++) {
        if (tasks[i] &&
            tasks[i]->getStatus() != TaskStatus::COMPLETED && 
            tasks[i]->getStatus() != TaskStatus::EXPIRED) {
            activeTasks[index++] = tasks[i];
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

    cout << "\n=== Employee Management ===\n";
    cout << "[1] View Employee Details\n";
    cout << "[2] Adjust Clearance Level\n";
    cout << "[3] Approve Time Off\n";
    cout << "[4] Return to Main Menu\n";
    cout << "Choice: ";

    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1: {
            cout << "\n=== Employee Details ===\n";
            ifstream userFile("users.txt");
            if (!userFile.is_open()) {
                cout << "Error accessing user database.\n";
                break;
            }

            char username[50], password[64];
            int clearanceLevel;
            while (userFile >> username >> password >> clearanceLevel) {
                cout << "Username: " << username << endl;
                cout << "Clearance Level: " << clearanceLevel << endl;
                    
                    // Count user's tasks
                int userTaskCount = 0;
                for (int i = 0; i < taskCount; i++) {
                    if (tasks[i]->getCreator()->getName() == username || 
                        (tasks[i]->getAssignee() && tasks[i]->getAssignee()->getName() == username)) {
                        userTaskCount++;
                    }
                }
                cout << "Active Tasks: " << userTaskCount << endl;
                cout << "-------------------\n";
            }
            userFile.close();
            break;
        }
        case 2: {
            char username[50];
            int newLevel;
            cout << "Enter username to modify: ";
            cin >> username;
            cout << "Enter new clearance level (1-5): ";
            cin >> newLevel;
                
            if (newLevel < 1 || newLevel > 5) {
                cout << "Invalid clearance level!\n";
            break;
            }
                
            ifstream inFile("users.txt");
            ofstream outFile("users_temp.txt");
            if (!inFile.is_open() || !outFile.is_open()) {
                cout << "Error accessing user database.\n";
                break;
            }

            char storedUser[50], storedPass[64];
            int storedLevel;
            bool found = false;

            while (inFile >> storedUser >> storedPass >> storedLevel) {
                if (strcmp(storedUser, username) == 0) {
                    outFile << username << " " << storedPass << " " << newLevel << endl;
                    found = true;
                    auditLogger->logSecurityEvent("UPDATE_CLEARANCE", 
                                                  "Updated clearance of user ", "SUCCESS");
                } else {
                    outFile << storedUser << " " << storedPass << " " << storedLevel << endl;
                }
            }

            inFile.close();
            outFile.close();

            if (found) {
                remove("users.txt");
                rename("users_temp.txt", "users.txt");
                cout << "Clearance level updated successfully!\n";
            } else {
                remove("users_temp.txt");
                cout << "User not found!\n";
            }
            break;
        }
        case 3: {
                char username[50];
                char startDate[11], endDate[11];  // YYYY-MM-DD format
                cout << "Enter username: ";
                cin >> username;
                cout << "Enter start date (YYYY-MM-DD): ";
                cin >> startDate;
                cout << "Enter end date (YYYY-MM-DD): ";
                cin >> endDate;
                
                User* targetUser = Authentication::findUser(username);
                if (!targetUser) {
                    cout << "User not found!\n";
                    break;
                }
                
                cout << "Time off request approved for " << username << endl;
                cout << "Period: " << startDate << " to " << endDate << endl;
                
                notificationSystem.sendNotification(targetUser, 
                    "Your time off request has been approved", NotificationType::INFO);
                break;
                }
        case 4:
            return;
        default:
            cout << "Invalid choice!\n";}
}

void SystemInterface::checkAndExpireAllTasks() {
    for (int i=0; i < taskCount; i++) {
        if (tasks[i]) {
            tasks[i]->checkExpiration();
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

