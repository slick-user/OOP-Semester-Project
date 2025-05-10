#ifndef AUDIT_H
#define AUDIT_H

#include <iostream>  // For `cout` (remove if not needed)
#include <ctime>     // For `time_t`

// Forward declarations (if needed)
struct AuditEntry;
struct LoginTracker;
struct TaskReassignmentTracker;

class AuditLog {
public:
    static void log(const char* username, const char* action, const char* details, const char* status);
    static void readLogs();
    static void generateAnomalyReport();
};

class AuditAnomalyDetectors {
public:
    static void checkLoginTimes(const char* username, time_t loginTime);
    static void checkTaskReassignments(const char* taskId, const char* assignedBy);
    static void generateAnomalyReport();

private:
    static LoginTracker* loginTrackers;       // Dynamic array instead of `map<string, int>`
    static int loginTrackerCount;
    static int loginTrackerCapacity;

    static TaskReassignmentTracker* taskReassignTrackers;  // Dynamic array instead of `map<string, int>`
    static int taskReassignTrackerCount;
    static int taskReassignTrackerCapacity;

    static const int REASSIGNMENT_THRESHOLD = 5;
    static const int LOGIN_HOUR_THRESHOLD_START = 22;  // 10 PM
    static const int LOGIN_HOUR_THRESHOLD_END = 6;     // 6 AM

    // Helper functions for dynamic array management
    static void resizeLoginTrackers();
    static void resizeTaskReassignTrackers();
    static int findLoginTracker(const char* username);
    static int findTaskReassignmentTracker(const char* taskId);
};

// Structs to replace `map` entries
struct LoginTracker {
    char* username;
    int loginCount;
};

struct TaskReassignmentTracker {
    char* taskId;
    int reassignmentCount;
};

#endif
