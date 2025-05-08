#include "Audit.h"

// Static member initialization for anomaly detection
map<string, int> AuditAnomalyDetector::loginTimes;
map<string, int> AuditAnomalyDetector::taskReassignments;

void AuditLog::log(const string& username, const string& action, const string& details, const string& status) {
    ofstream auditFile("audit.txt", ios::app);
    if (auditFile.is_open()) {
        time_t now = time(0);
        char* dt = ctime(&now);
        auditFile << "[" << dt << "] " << username << " " << action << " " << details << " " << status << endl;
        auditFile.close();

        // Check for anomalies after logging
        if (action == "Task Reassigned") {
            AuditAnomalyDetector::checkTaskReassignments(details, username);
        }
        if (action == "Login") {
            AuditAnomalyDetector::checkLoginTimes(username, now);
        }
    }
}

void AuditLog::readLogs() {
    ifstream auditFile("audit.txt");
    string line;
    while (getline(auditFile, line)) {
        cout << line << endl;
    }
}

void AuditAnomalyDetector::checkLoginTimes(const string& username, time_t loginTime) {
    struct tm* localTime = localtime(&loginTime);
    int hour = localTime->tm_hour;

    // Check if the login occurred at an unusual hour (e.g., 10 PM - 6 AM)
    if (hour >= LOGIN_HOUR_THRESHOLD_START || hour < LOGIN_HOUR_THRESHOLD_END) {
        cout << "ALERT: Unusual login time for " << username << " at hour " << hour << endl;
        // Log anomaly report
        ofstream anomalyReport("anomaly_report.txt", ios::app);
        anomalyReport << "Unusual login time detected for " << username << " at hour " << hour << endl;
        anomalyReport.close();
    }
    // Track login count per user
    loginTimes[username]++;
}

void AuditAnomalyDetector::checkTaskReassignments(const string& taskId, const string& assignedBy) {
    taskReassignments[taskId]++;

    // Check if task reassignment exceeds threshold
    if (taskReassignments[taskId] > REASSIGNMENT_THRESHOLD) {
        cout << "ALERT: High task reassignment rate detected for Task ID: " << taskId << endl;
        // Log anomaly report
        ofstream anomalyReport("anomaly_report.txt", ios::app);
        anomalyReport << "High task reassignment rate detected for Task ID: " << taskId << endl;
        anomalyReport.close();
    }
}

void AuditAnomalyDetector::generateAnomalyReport() {
    // Placeholder for generating the full anomaly report.
    // Can implement more sophisticated anomaly detection algorithms as needed.
    cout << "Generating anomaly report...\n";
    ifstream anomalyReport("anomaly_report.txt");
    string line;
    while (getline(anomalyReport, line)) {
        cout << line << endl;
    }
}
