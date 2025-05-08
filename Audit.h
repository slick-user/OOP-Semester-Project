#ifndef AUDIT_H
#define AUDIT_H

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <map>

using namespace std;

class AuditLog {
public:
    static void log(const string& username, const string& action, const string& details, const string& status);
    static void readLogs();
    static void generateAnomalyReport();
};

class AuditAnomalyDetector {
public:
    static void checkLoginTimes(const string& username, time_t loginTime);
    static void checkTaskReassignments(const string& taskId, const string& assignedBy);
    static void generateAnomalyReport();

private:
    static map<string, int> loginTimes;  // Track login times per user
    static map<string, int> taskReassignments;  // Track task reassignment counts
    static const int REASSIGNMENT_THRESHOLD = 5;  // Max reassignments in a short period
    static const int LOGIN_HOUR_THRESHOLD_START = 22;  // 10 PM for login time anomaly
    static const int LOGIN_HOUR_THRESHOLD_END = 6;  // 6 AM for login time anomaly
};
#endif
