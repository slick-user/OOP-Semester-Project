#include "AuditAnomalyDetector.h"
#include <iostream>
#include <fstream>

AuditAnomalyDetector::AuditAnomalyDetector() : recordCount(0) {
    strcpy(logFile, "anomaly_report.txt");
    lastCheck = time(0);
}

bool AuditAnomalyDetector::isLoginTimeUnusual(time_t loginTime) const {
    struct tm* timeInfo = localtime(&loginTime);
    int hour = timeInfo->tm_hour;
    
    return (hour >= AnomalyThresholds::SUSPICIOUS_TIME_START || 
            hour <= AnomalyThresholds::SUSPICIOUS_TIME_END);
}

bool AuditAnomalyDetector::isReassignmentRateHigh(const char* taskId) const {
    int reassignments = 0;
    
    for (int i = 0; i < recordCount; i++) {
        if (strstr(records[i].details, taskId) && 
            strcmp(records[i].eventType, "TASK_REASSIGN") == 0) {
            reassignments++;
        }
    }
    
    return reassignments >= AnomalyThresholds::MAX_TASK_REASSIGNMENTS;
}

void AuditAnomalyDetector::checkLoginPattern(const char* username, time_t loginTime) {
    if (isLoginTimeUnusual(loginTime)) {
        char details[100];
        snprintf(details, sizeof(details), 
                "Unusual login time detected for user: %s", username);
        addAnomalyRecord(username, "UNUSUAL_LOGIN", details, 2);
    }
}

void AuditAnomalyDetector::checkTaskReassignment(const char* username, const char* taskId) {
    if (isReassignmentRateHigh(taskId)) {
        char details[100];
        snprintf(details, sizeof(details), 
                "High reassignment rate for task %s by user %s", taskId, username);
        addAnomalyRecord(username, "TASK_REASSIGN", details, 3);
    }
}

void AuditAnomalyDetector::checkMessagePattern(const char* sender, 
                                              const char* recipient, 
                                              time_t sendTime) {
    if (isLoginTimeUnusual(sendTime)) {
        char details[100];
        snprintf(details, sizeof(details), 
                "Unusual message time from %s to %s", sender, recipient);
        addAnomalyRecord(sender, "UNUSUAL_MESSAGE", details, 1);
    }
}

void AuditAnomalyDetector::addAnomalyRecord(const char* user, const char* event, 
                                           const char* details, int severity) {
    if (recordCount >= MAX_RECORDS) return;
    
    AnomalyRecord& record = records[recordCount++];
    strncpy(record.username, user, sizeof(record.username) - 1);
    strncpy(record.eventType, event, sizeof(record.eventType) - 1);
    strncpy(record.details, details, sizeof(record.details) - 1);
    record.timestamp = time(0);
    record.severity = severity;
    
    writeAnomalyReport();
}

void AuditAnomalyDetector::writeAnomalyReport() const {
    ofstream report(logFile, ios::app);
    if (!report.is_open()) return;
    
    const AnomalyRecord& record = records[recordCount - 1];
    char timestamp[26];
    ctime_s(timestamp, sizeof(timestamp), &record.timestamp);
    timestamp[24] = '\0';  // Remove newline
    
    report << "[" << timestamp << "] "
           << "ANOMALY DETECTED - Severity: " << record.severity << "\n"
           << "User: " << record.username << "\n"
           << "Event: " << record.eventType << "\n"
           << "Details: " << record.details << "\n"
           << "-------------------------------------------\n";
    
    report.close();
}

void AuditAnomalyDetector::generateAnomalyReport() const {
    cout << "\n=== Anomaly Detection Report ===\n";
    cout << "Total Anomalies Detected: " << recordCount << "\n\n";
    
    int severityCounts[3] = {0};
    
    for (int i = 0; i < recordCount; i++) {
        severityCounts[records[i].severity - 1]++;
    }
    
    cout << "Severity Breakdown:\n"
         << "Low: " << severityCounts[0] << "\n"
         << "Medium: " << severityCounts[1] << "\n"
         << "High: " << severityCounts[2] << "\n\n";
    
    cout << "Recent Anomalies:\n";
    for (int i = max(0, recordCount - 5); i < recordCount; i++) {
        char timestamp[26];
        ctime_s(timestamp, sizeof(timestamp), &records[i].timestamp);
        timestamp[24] = '\0';
        
        cout << "[" << timestamp << "] "
             << records[i].eventType << " - "
             << records[i].details << "\n";
    }
    cout << "===============================\n";
}

bool AuditAnomalyDetector::hasHighSeverityAnomalies() const {
    for (int i = 0; i < recordCount; i++) {
        if (records[i].severity == 3) return true;
    }
    return false;
}
