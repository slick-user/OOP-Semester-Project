#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct EmployeeStats {
    string username;
    int clearanceLevel;
    int totalTasks{0};
    int completedTasks{0};
    int overdueTasks{0};

    EmployeeStats() = default;
    EmployeeStats(const string& name, int level) : username(name), clearanceLevel(level) {}

    double getPerformanceScore() const {
        if (totalTasks == 0) return 0.0;
        return (completedTasks * 100.0) / totalTasks;
    }

    bool loadStats(const string& name);
};

// Add operator<< declaration
ostream& operator<<(ostream& out, const EmployeeStats& stats);

#endif
