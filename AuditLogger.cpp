#include "AuditLogger.h"

AuditLogger::AuditLogger(const char* user) {
    strncpy(username, user, sizeof(username));
    username[sizeof(username) - 1] = '\0'; // Null-terminate
}

void AuditLogger::logAction(const char* action, const char* details, const char* status) const {
    ofstream logFile("audit.txt", ios::app); // Append-only

    if (!logFile.is_open()) return;

    time_t now = time(0);
    char* dt = ctime(&now);
    dt[strlen(dt) - 1] = '\0'; // Remove newline

    logFile << "[" << dt << "] "
            << username << " "
            << action << " "
            << details << " "
            << status << "\n";

    logFile.close();
}

AuditLogger& operator<<(AuditLogger& logger, const char* entry) {
    logger.logAction("ACTION", entry, "OK");
    return logger;
}
