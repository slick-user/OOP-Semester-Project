#include "AuditLogger.h"

const char* AuditLogger::AUDIT_FILE = "audit.txt";

AuditLogger::AuditLogger(const char* user) {
    strncpy(username, user, sizeof(username) - 1);
    username[sizeof(username) - 1] = '\0';
}

char* AuditLogger::getCurrentTimestamp() const {
    static char timestamp[26];
    time_t now = time(0);
    ctime_s(timestamp, sizeof(timestamp), &now);
    timestamp[24] = '\0'; // Remove newline
    return timestamp;
}

bool AuditLogger::writeToFile(const char* action, const char* details, const char* status) const {
    ofstream audit(AUDIT_FILE, ios::app);
    if (!audit.is_open()) return false;

    audit << "[" << getCurrentTimestamp() << "] "
          << username << " "
          << action << " "
          << details << " "
          << status << "\n";

    audit.close();
    return true;
}

bool AuditLogger::logLogin() const {
    return writeToFile("LOGIN", "User login attempt", "SUCCESS");
}

bool AuditLogger::logLogout() const {
    return writeToFile("LOGOUT", "User session ended", "SUCCESS");
}

bool AuditLogger::logTaskAction(const char* action, const char* taskId, const char* status) const {
    char details[100];
    snprintf(details, sizeof(details), "TaskID:%s", taskId);
    return writeToFile(action, details, status);
}

bool AuditLogger::logMessageAction(const char* action, const char* recipient, const char* status) const {
    char details[100];
    snprintf(details, sizeof(details), "To:%s", recipient);
    return writeToFile(action, details, status);
}

bool AuditLogger::logSecurityEvent(const char* action, const char* details, const char* status) const {
    return writeToFile(action, details, status);
}

AuditLogger& AuditLogger::operator<<(const char* entry) {
    writeToFile("ACTION", entry, "INFO");
    return *this;
}
