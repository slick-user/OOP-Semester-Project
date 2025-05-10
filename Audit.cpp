#include "Audit.h"
#include <cstring>
#include <fstream>

using namespace std;

// Initialize static members
LoginTracker* AuditAnomalyDetectors::loginTrackers = nullptr;
int AuditAnomalyDetectors::loginTrackerCount = 0;
int AuditAnomalyDetectors::loginTrackerCapacity = 0;

TaskReassignmentTracker* AuditAnomalyDetectors::taskReassignTrackers = nullptr;
int AuditAnomalyDetectors::taskReassignTrackerCount = 0;
int AuditAnomalyDetectors::taskReassignTrackerCapacity = 0;

// ===== Helper Functions (Dynamic Array Management) =====
void AuditAnomalyDetectors::resizeLoginTrackers() {
    int newCapacity = loginTrackerCapacity == 0 ? 4 : loginTrackerCapacity * 2;
    LoginTracker* newTrackers = new LoginTracker[newCapacity];

    for (int i = 0; i < loginTrackerCount; i++) {
        newTrackers[i].username = new char[strlen(loginTrackers[i].username) + 1];
        strcpy(newTrackers[i].username, loginTrackers[i].username);
        newTrackers[i].loginCount = loginTrackers[i].loginCount;
        delete[] loginTrackers[i].username;  // Free old memory
    }

    delete[] loginTrackers;
    loginTrackers = newTrackers;
    loginTrackerCapacity = newCapacity;
}

void AuditAnomalyDetectors::resizeTaskReassignTrackers() {
    int newCapacity = taskReassignTrackerCapacity == 0 ? 4 : taskReassignTrackerCapacity * 2;
    TaskReassignmentTracker* newTrackers = new TaskReassignmentTracker[newCapacity];

    for (int i = 0; i < taskReassignTrackerCount; i++) {
        newTrackers[i].taskId = new char[strlen(taskReassignTrackers[i].taskId) + 1];
        strcpy(newTrackers[i].taskId, taskReassignTrackers[i].taskId);
        newTrackers[i].reassignmentCount = taskReassignTrackers[i].reassignmentCount;
        delete[] taskReassignTrackers[i].taskId;  // Free old memory
    }

    delete[] taskReassignTrackers;
    taskReassignTrackers = newTrackers;
    taskReassignTrackerCapacity = newCapacity;
}


void AuditLog::readLogs() {
    ifstream auditFile("audit.txt");
    string line;
    while (getline(auditFile, line)) {
        cout << line << endl;
    }
}

int AuditAnomalyDetectors::findLoginTracker(const char* username) {
    for (int i = 0; i < loginTrackerCount; i++) {
        if (strcmp(loginTrackers[i].username, username) == 0) {
            return i;
        }
    }
    return -1;  // Not found
}

int AuditAnomalyDetectors::findTaskReassignmentTracker(const char* taskId) {
    for (int i = 0; i < taskReassignTrackerCount; i++) {
        if (strcmp(taskReassignTrackers[i].taskId, taskId) == 0) {
            return i;
        }
    }
    return -1;  // Not found
}

// ===== Core Functions =====
void AuditLog::log(const char* username, const char* action, const char* details, const char* status) {
    // Open file in append mode
    FILE* auditFile = fopen("audit.txt", "a");
    if (auditFile) {
        time_t now = time(0);
        char* dt = ctime(&now);
        fprintf(auditFile, "[%s] %s %s %s %s\n", dt, username, action, details, status);
        fclose(auditFile);

        // Check for anomalies
        if (strcmp(action, "Task Reassigned") == 0) {
            AuditAnomalyDetectors::checkTaskReassignments(details, username);
        }
        if (strcmp(action, "Login") == 0) {
            AuditAnomalyDetectors::checkLoginTimes(username, now);
        }
    }
}

void AuditAnomalyDetectors::checkLoginTimes(const char* username, time_t loginTime) {
    struct tm* localTime = localtime(&loginTime);
    int hour = localTime->tm_hour;

    // Check for unusual login time (10 PM - 6 AM)
    if (hour >= LOGIN_HOUR_THRESHOLD_START || hour < LOGIN_HOUR_THRESHOLD_END) {
        printf("ALERT: Unusual login time for %s at hour %d\n", username, hour);
        
        // Log anomaly report
        FILE* anomalyReport = fopen("anomaly_report.txt", "a");
        if (anomalyReport) {
            fprintf(anomalyReport, "Unusual login time detected for %s at hour %d\n", username, hour);
            fclose(anomalyReport);
        }
    }

    // Update login count (replaces `loginTimes[username]++`)
    int index = findLoginTracker(username);
    if (index == -1) {
        // New user - add to trackers
        if (loginTrackerCount == loginTrackerCapacity) {
            resizeLoginTrackers();
        }
        loginTrackers[loginTrackerCount].username = new char[strlen(username) + 1];
        strcpy(loginTrackers[loginTrackerCount].username, username);
        loginTrackers[loginTrackerCount].loginCount = 1;
        loginTrackerCount++;
    } else {
        loginTrackers[index].loginCount++;
    }
}

void AuditAnomalyDetectors::checkTaskReassignments(const char* taskId, const char* assignedBy) {
    // Update reassignment count (replaces `taskReassignments[taskId]++`)
    int index = findTaskReassignmentTracker(taskId);
    if (index == -1) {
        // New task - add to trackers
        if (taskReassignTrackerCount == taskReassignTrackerCapacity) {
            resizeTaskReassignTrackers();
        }
        taskReassignTrackers[taskReassignTrackerCount].taskId = new char[strlen(taskId) + 1];
        strcpy(taskReassignTrackers[taskReassignTrackerCount].taskId, taskId);
        taskReassignTrackers[taskReassignTrackerCount].reassignmentCount = 1;
        taskReassignTrackerCount++;
    } else {
        taskReassignTrackers[index].reassignmentCount++;
    }

    // Check threshold
    if (taskReassignTrackers[index].reassignmentCount > REASSIGNMENT_THRESHOLD) {
        printf("ALERT: High task reassignment rate detected for Task ID: %s\n", taskId);
        
        // Log anomaly report
        FILE* anomalyReport = fopen("anomaly_report.txt", "a");
        if (anomalyReport) {
            fprintf(anomalyReport, "High task reassignment rate detected for Task ID: %s\n", taskId);
            fclose(anomalyReport);
        }
    }
}
