#include "Employee.h"

bool EmployeeStats::loadStats(const std::string& name) {
    std::ifstream file(name + "_stats.dat");
    if (!file.is_open()) return false;
    
    file >> username >> clearanceLevel >> totalTasks >> completedTasks >> overdueTasks;
    return true;
}

std::ostream& operator<<(std::ostream& os, const EmployeeStats& stats) {
    os << "Username: " << stats.username << "\n"
       << "Clearance Level: " << stats.clearanceLevel << "\n"
       << "Total Tasks: " << stats.totalTasks << "\n"
       << "Completed Tasks: " << stats.completedTasks << "\n"
       << "Overdue Tasks: " << stats.overdueTasks << "\n"
       << "Performance Score: " << stats.getPerformanceScore() << "%\n";
    return os;
}
