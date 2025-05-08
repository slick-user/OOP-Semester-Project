#ifndef AUDITLOGGER_H
#define AUDITLOGGER_H

#include <ctime>
#include <fstream>
#include <cstring>
#include <iostream>

using namespace std;

class AuditLogger {
    char username[50];

public:
    AuditLogger(const char* user);

    void logAction(const char* action, const char* details, const char* status) const;

    // Overload << operator
    friend AuditLogger& operator<<(AuditLogger& logger, const char* entry);
};

#endif
