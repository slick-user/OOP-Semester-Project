#ifndef PRIORITY_MANAGER_H
#define PRIORITY_MANAGER_H

#include "Task.h"
#include "User.h"
#include "AuditLogger.h"
#include <cstring>

class PriorityManager {
private:
    static const int MAX_TASKS = 100;
    Task* tasks[MAX_TASKS];
    int taskCount;
    AuditLogger* logger;

    void sortByPriority();
    bool validatePriorityChange(User* user, Task* task, int newPriority) const;

public:
    PriorityManager();
    ~PriorityManager();

    bool addTask(Task* task);
    bool updateTaskPriority(User* user, Task* task, int newPriority);
    Task* getNextHighestPriorityTask() const;
    void displayTasksByPriority() const;
    int getTaskCountByPriority(int priority) const;
    Task** getTasksByPriority(int priority, int* count);
    
    // Additional functionality
    void cleanupExpiredTasks();
    bool hasHighPriorityTasks() const;
    double getAveragePriority() const;
};

#endif
