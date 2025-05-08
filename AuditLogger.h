#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include <ctime>
#include <fstream>
#include <cstring>
#include <iostream>

using namespace std;

class AuditLogger {
private:
    static const char* AUDIT_FILE;
    char username[50];
    
    bool writeToFile(const char* action, const char* details, const char* status) const;
    char* getCurrentTimestamp() const;

public:
    explicit AuditLogger(const char* user);
    
    // Core logging functions
    bool logLogin() const;
    bool logLogout() const;
    bool logTaskAction(const char* action, const char* taskId, const char* status) const;
    bool logMessageAction(const char* action, const char* recipient, const char* status) const;
    bool logSecurityEvent(const char* action, const char* details, const char* status) const;

    // Operator overload for simple logging
    AuditLogger& operator<<(const char* entry);
};

#endif