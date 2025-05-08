#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include "User.h"
#include "Task.h"
#include "Notification.h"
#include "Audit.h"
#include "Authentication.h"
#include "TimeManager.h"
#include "Message.h"
#include "Employee.h"
#include "AuditLogger.h"
#include "Employee.h"

using namespace std;

// File handling functions
void saveTaskToFile(const Task &task)
{
    ofstream file("tasks.dat", ios::app);
    if (file.is_open())
    {
        file << task.getId() << " "
             << task.getCreator()->getName() << " "
             << task.getAssignee()->getName() << " "
             << task.getStatusString() << " "
             << task.getPriorityString() << "\n";
        file.close();
    }
    else
    {
        cerr << "Error opening file for saving tasks.\n";
    }
}

void loadTasksFromFile()
{
    ifstream file("tasks.dat");
    if (file.is_open())
    {
        int taskId;
        string creatorName, assigneeName, status, priority;
        while (file >> taskId >> creatorName >> assigneeName >> status >> priority)
        {
            cout << "Loaded Task - ID: " << taskId << " Creator: " << creatorName
                 << " Assignee: " << assigneeName << " Status: " << status
                 << " Priority: " << priority << "\n";
        }
        file.close();
    }
    else
    {
        cerr << "Error opening task file for reading.\n";
    }
}
// Add after the includes and before SystemInterface class

class SystemInterface
{
private:
    Task* tasks;
    int taskCount;
    User *currentUser;
    NotificationSystem notificationSystem;
    AuditLogger *auditLogger;

    // Helper functions
    TaskStatus stringToTaskStatus(const string &status)
    {
        if (status == "CREATED")
            return CREATED;
        if (status == "ASSIGNED")
            return ASSIGNED;
        if (status == "IN_PROGRESS")
            return IN_PROGRESS;
        if (status == "COMPLETED")
            return COMPLETED;
        if (status == "EXPIRED")
            return EXPIRED;
        return CREATED;
    }

    void addTask(Task newTask) {
      Task* newTasks = new Task[taskCount + 1];
      for (int i=0; i < taskCount; i++) {
        newTasks[i] = tasks[i];
      }
      newTasks[taskCount] = newTask;

      delete[] tasks;
      tasks = newTasks;
      ++taskCount;
    }

    // Remove a task by name (or use ID, your call)
    void removeTask() {
      int index = -1;

      Task* newTasks = new Task[taskCount - 1];
      for (int i = 0, j = 0; i < taskCount; ++i) {
        if (i != index) {
            newTasks[j++] = tasks[i];
        }
      }

      delete[] tasks;
      tasks = newTasks;
      --taskCount;
    }

    User *loginUser()
    {
        string username, password;
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;

        User *user = Authentication::loginUser(username.c_str(), password.c_str());
        if (!user)
        {
            cout << "Login failed!\n";
        }
        return user;
    }

    // Display functions
    void displayHeader()
    {
        cout << "\n====================================\n";
        cout << "      OSIM Security Task System      \n";
        cout << "====================================\n";
    }

    void displayLoginMenu()
    {
        cout << "\n[1] Login\n";
        cout << "[2] Register New User\n";
        cout << "[3] Exit\n";
        cout << "Choice: ";
    }

    void displayMainMenu()
    {
        cout << "\n=== Main Menu ===\n";
        cout << "[1] Task Management\n";
        cout << "[2] Messages & Notifications\n";
        cout << "[3] Security & Audit\n";
        cout << "[4] Employee Management\n";
        cout << "[5] Logout\n";
        cout << "Choice: ";
    }

    // Handler functions
    void handleTaskManagement();
    void handleMessaging();
    void handleSecurity();

public:
    SystemInterface() : currentUser(nullptr), auditLogger(nullptr), tasks(nullptr), taskCount(0) {}

    ~SystemInterface()
    {
        delete[] tasks;
        tasks = nullptr;
        taskCount = 0;
        if (auditLogger)
        {
            delete auditLogger;
        }
    }

    void start();
};

// Implementation of handler functions
void SystemInterface::handleTaskManagement()
{
    while (true)
    {
        cout << "\n=== Task Management ===\n";
        cout << "[1] Create New Task\n";
        cout << "[2] View All Tasks\n";
        cout << "[3] Update Task Status\n";
        cout << "[4] Assign Task\n";
        cout << "[5] View Expired Tasks\n";
        cout << "[6] Return to Main Menu\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string description;
            int priority;
            cout << "Task description: ";
            cin.ignore();
            getline(cin, description);
            cout << "Priority (0-High, 1-Medium, 2-Low): ";
            cin >> priority;

            Task *newTask = new Task(currentUser, 86400, static_cast<TaskPriority>(priority));
            addTask(*newTask);
            saveTaskToFile(*newTask);
            auditLogger->logAction("CREATE_TASK", "New task created", "SUCCESS");
            break;
        }
        case 2:
            loadTasksFromFile();
            break;
        case 3:
        {
            int taskId;
            string newStatus;
            cout << "Enter task ID: ";
            cin >> taskId;
            cout << "New status (CREATED/ASSIGNED/IN_PROGRESS/COMPLETED/EXPIRED): ";
            cin >> newStatus;

            for (int i=0; i<taskCount; i++)
            {
                if (tasks[i].getId() == taskId)
                {
                    tasks[i].updateStatus(stringToTaskStatus(newStatus));
                    auditLogger->logAction("UPDATE_TASK", "Task status updated", "SUCCESS");
                    break;
                }
            }

            break;
        }
        case 4:
        {
            int taskId;
            string assigneeName;
            cout << "Enter task ID: ";
            cin >> taskId;
            cout << "Assignee username: ";
            cin >> assigneeName;

            for (int i=0; i<taskCount; i++)
            {
                if (tasks[i].getId() == taskId)
                {
                    User *assignee = Authentication::loginUser(assigneeName.c_str(), "");
                    if (assignee && tasks[i].assignTo(currentUser, assignee))
                    {
                        auditLogger->logAction("ASSIGN_TASK", "Task assigned", "SUCCESS");
                    }
                    break;
                }
            }
            break;
        }
        case 5:
        {
            Task *taskArray = new Task [taskCount];
            for (int i = 0; i < taskCount; i++)
            {
                taskArray[i] = tasks[i];
            }
            // Gotta do something about this thing right here
            TimeManager::expireTasks(taskArray, taskCount);
            delete[] taskArray;
            break;
        }
        case 6:
            return;
        }
    }
}

void SystemInterface::handleMessaging()
{
    while (true)
    {
        cout << "\n=== Messages & Notifications ===\n";
        cout << "[1] Send New Message\n";
        cout << "[2] View Inbox\n";
        cout << "[3] Send Notification\n";
        cout << "[4] View Notifications\n";
        cout << "[5] Return to Main Menu\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string recipient, content;
            cout << "Recipient: ";
            cin >> recipient;
            cout << "Message: ";
            cin.ignore();
            getline(cin, content);
            Message msg(currentUser->getName().c_str(), recipient.c_str(), INFO, content.c_str());
            msg.sendMessage();
            break;
        }
            // In handleMessaging() function, replace the TODO in case 2:
        case 2:
        {
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
        case 3:
            if (NotificationSystem::canSendNotification(currentUser, WARNING))
            {
                string message;
                cout << "Notification message: ";
                cin.ignore();
                getline(cin, message);
                NotificationSystem::sendNotification(currentUser, WARNING, message);
            }

            break;
            case 4:
    NotificationSystem::displayNotifications();  // Update to use NotificationSystem
    break;
        case 5:
            return;
        }
    }
}

void SystemInterface::handleSecurity()
{
    while (true)
    {
        cout << "\n=== Security & Audit ===\n";
        cout << "[1] View Audit Logs\n";
        cout << "[2] Generate Anomaly Report\n";
        cout << "[3] Change Password\n";
        cout << "[4] View Security Policies\n";
        cout << "[5] Return to Main Menu\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            AuditLog::readLogs();
            break;
        case 2:
            AuditAnomalyDetector::generateAnomalyReport();
            break;
        case 3:
        {
            string oldPass, newPass;
            cout << "Enter old password: ";
            cin >> oldPass;
            cout << "Enter new password: ";
            cin >> newPass;
            if (Authentication::changePassword(currentUser->getName().c_str(),
                                               oldPass.c_str(),
                                               newPass.c_str()))
            {
                auditLogger->logAction("CHANGE_PASSWORD", "Password changed", "SUCCESS");
                cout << "Password changed successfully.\n";
            }
            else
            {
                auditLogger->logAction("CHANGE_PASSWORD", "Password change failed", "FAILURE");
                cout << "Failed to change password.\n";
            }
            break;
        }
            // In handleSecurity() function, replace the TODO in case 4:
        case 4:
        {
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
        case 5:
            return;
        }
    }
}

void SystemInterface::start()
{
    while (true)
    {
        displayHeader();
        if (!currentUser)
        {
            displayLoginMenu();
            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
            {
                currentUser = loginUser();
                if (currentUser)
                {
                    auditLogger = new AuditLogger(currentUser->getName().c_str());
                    auditLogger->logAction("LOGIN", "User logged in", "SUCCESS");
                }
                break;
            }
                // In start() function, replace the TODO in case 2:
            case 2:
            {
                string username, password;
                int clearanceLevel;
                cout << "Enter new username: ";
                cin >> username;
                cout << "Enter password: ";
                cin >> password;
                cout << "Enter clearance level (1-5): ";
                cin >> clearanceLevel;

                if (clearanceLevel < 1 || clearanceLevel > 5)
                {
                    cout << "Invalid clearance level!\n";
                    break;
                }

                if (Authentication::registerUser(username.c_str(), password.c_str(), clearanceLevel))
                {
                    cout << "User registered successfully!\n";
                    auditLogger = new AuditLogger("SYSTEM");
                    auditLogger->logAction("REGISTER", "New user registered", "SUCCESS");
                    delete auditLogger;
                    auditLogger = nullptr;
                }
                else
                {
                    cout << "Registration failed! Username may already exist.\n";
                }
                break;
            }
            case 3:
                cout << "Exiting system...\n";
                return;
            }
        }
        else
        {
            displayMainMenu();
            int choice;
            cin >> choice;

            switch (choice)
            {
            case 1:
                handleTaskManagement();
                break;
            case 2:
                handleMessaging();
                break;
            case 3:
                handleSecurity();
                break;
                // In start() function, replace the TODO in case 4:
                // Replace the employee management case in start() function

            case 4:
            {
                if (currentUser->getClearanceLevel() >= 3)
                { // Manager or higher
                    cout << "\n=== Employee Management ===\n";
                    cout << "1. View Employee Statistics\n";
                    cout << "2. Generate Performance Report\n";
                    cout << "3. Return to Main Menu\n";
                    cout << "Choice: ";

                    int empChoice;
                    cin >> empChoice;

                    switch (empChoice)
                    {
                    case 1:
                    {
                        ifstream empFile("employees.dat");
                        if (empFile.is_open())
                        {
                            cout << "\n=== Employee Statistics ===\n";
                            EmployeeStats stats;
                            while (empFile >> stats.username >> stats.clearanceLevel)
                            {
                                if (stats.loadStats(stats.username))
                                {
                                    cout << "\n-------------------------\n";
                                    cout << stats;
                                }
                            }
                            empFile.close();
                        }
                        else
                        {
                            cout << "No employee records found.\n";
                            auditLogger->logAction("VIEW_STATS", "Failed to open employee records", "FAILURE");
                        }
                        break; // Missing break statement
                    }
                    case 2:
                    {
                        string empName;
                        cout << "Enter employee username: ";
                        cin >> empName;

                        EmployeeStats stats;
                        if (stats.loadStats(empName))
                        {
                            cout << "\n=== Performance Report ===\n";
                            cout << stats;
                            auditLogger->logAction("PERFORMANCE_CHECK",
                                                   ("Checked " + empName + "'s performance").c_str(),
                                                   "SUCCESS");
                        }
                        else
                        {
                            cout << "Employee not found or no statistics available!\n";
                            auditLogger->logAction("PERFORMANCE_CHECK",
                                                   ("Failed to check " + empName + "'s performance").c_str(),
                                                   "FAILURE");
                        }
                        break;
                    }
                    case 3:
                        break;
                    default:
                        cout << "Invalid choice!\n";
                        break;
                    }
                }
                else
                {
                    cout << "Access denied! Insufficient clearance level.\n";
                    auditLogger->logAction("ACCESS_DENIED",
                                           "Attempted to access employee management",
                                           "FAILURE");
                }
                break;
            }
            case 5:
                delete auditLogger;
                auditLogger = nullptr;
                currentUser = nullptr;
                cout << "Logged out successfully.\n";
                break;
            }
        }
    }
}

int main()
{
    SystemInterface system;
    system.start();
    return 0;
}
