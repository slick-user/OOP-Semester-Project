#ifndef AUDIT_ANOMALY_DETECTOR_H
#define AUDIT_ANOMALY_DETECTOR_H

#include <ctime>
#include <cstring>
#include "User.h"

struct AnomalyThresholds {
    static const int MAX_LOGIN_ATTEMPTS = 3;
    static const int MAX_TASK_REASSIGNMENTS = 5;
    static const int SUSPICIOUS_TIME_START = 23;  // 11 PM
    static const int SUSPICIOUS_TIME_END = 5;     // 5 AM
};

struct AnomalyRecord {
    char username[50];
    char eventType[20];
    char details[100];
    time_t timestamp;
    int severity;  // 1-low, 2-medium, 3-high
};

class AuditAnomalyDetector {
private:
    static const int MAX_RECORDS = 1000;
    AnomalyRecord records[MAX_RECORDS];
    int recordCount;
    
    char logFile[100];
    time_t lastCheck;
    
    bool isLoginTimeUnusual(time_t loginTime) const;
    bool isReassignmentRateHigh(const char* taskId) const;
    void addAnomalyRecord(const char* user, const char* event, 
                         const char* details, int severity);
    void writeAnomalyReport() const;

public:
    AuditAnomalyDetector();
    
    void checkLoginPattern(const char* username, time_t loginTime);
    void checkTaskReassignment(const char* username, const char* taskId);
    void checkMessagePattern(const char* sender, const char* recipient, time_t sendTime);
    void generateAnomalyReport() const;
    
    // Getters
    int getAnomalyCount() const { return recordCount; }
    bool hasHighSeverityAnomalies() const;
};

#endif