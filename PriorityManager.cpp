#include "PriorityManager.h"
#include <iostream>

PriorityManager::PriorityManager() : taskCount(0) {
    memset(tasks, 0, sizeof(tasks));
    logger = new AuditLogger("PriorityManager");
}

PriorityManager::~PriorityManager() {
    delete logger;
}

bool PriorityManager::addTask(Task* task) {
    if (taskCount >= MAX_TASKS || !task) return false;
    
    tasks[taskCount++] = task;
    sortByPriority();
    
    logger->logTaskAction("ADD_TASK", 
                         to_string(task->getId()).c_str(),
                         "SUCCESS");
    return true;
}

void PriorityManager::sortByPriority() {
    // Simple bubble sort by priority (HIGH = 0, LOW = 2)
    for (int i = 0; i < taskCount - 1; i++) {
        for (int j = 0; j < taskCount - i - 1; j++) {
            if (tasks[j]->getPriority() > tasks[j + 1]->getPriority()) {
                Task* temp = tasks[j];
                tasks[j] = tasks[j + 1];
                tasks[j + 1] = temp;
            }
        }
    }
}

bool PriorityManager::validatePriorityChange(User* user, Task* task, int newPriority) const {
    // Only task creator or manager and above can change priority
    if (!user || !task) return false;
    
    if (user->getClearanceLevel() >= 3) return true; // Manager and above
    
    return task->getCreator() == user;
}

bool PriorityManager::updateTaskPriority(User* user, Task* task, int newPriority) {
    if (!validatePriorityChange(user, task, newPriority)) {
        logger->logTaskAction("UPDATE_PRIORITY", 
                            to_string(task->getId()).c_str(),
                            "DENIED_PERMISSION");
        return false;
    }

    if (newPriority < TaskPriority::HIGH || newPriority > TaskPriority::LOW) {
        logger->logTaskAction("UPDATE_PRIORITY", 
                            to_string(task->getId()).c_str(),
                            "INVALID_PRIORITY");
        return false;
    }

    task->setPriority(newPriority);
    sortByPriority();
    
    logger->logTaskAction("UPDATE_PRIORITY", 
                         to_string(task->getId()).c_str(),
                         "SUCCESS");
    return true;
}

Task* PriorityManager::getNextHighestPriorityTask() const {
    for (int i = 0; i < taskCount; i++) {
        if (!tasks[i]->isExpired() && 
            tasks[i]->getStatus() != TaskStatus::COMPLETED) {
            return tasks[i];
        }
    }
    return nullptr;
}

void PriorityManager::displayTasksByPriority() const {
    const char* priorityNames[] = {"HIGH", "MEDIUM", "LOW"};
    
    for (int priority = TaskPriority::HIGH; priority <= TaskPriority::LOW; priority++) {
        cout << "\n=== " << priorityNames[priority] << " Priority Tasks ===\n";
        bool found = false;
        
        for (int i = 0; i < taskCount; i++) {
            if (tasks[i]->getPriority() == priority) {
                cout << "Task #" << tasks[i]->getId() << ": "
                     << tasks[i]->getDescription() << "\n"
                     << "Status: " << tasks[i]->getStatusString() << "\n"
                     << "Time to Expiration: " << tasks[i]->getTimeToExpiration() << " seconds\n"
                     << "--------------------\n";
                found = true;
            }
        }
        
        if (!found) {
            cout << "No tasks with " << priorityNames[priority] << " priority.\n";
        }
    }
}

int PriorityManager::getTaskCountByPriority(int priority) const {
    int count = 0;
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i]->getPriority() == priority) {
            count++;
        }
    }
    return count;
}

Task** PriorityManager::getTasksByPriority(int priority, int* count) {
    *count = getTaskCountByPriority(priority);
    if (*count == 0) return nullptr;
    
    Task** priorityTasks = new Task*[*count];
    int index = 0;
    
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i]->getPriority() == priority) {
            priorityTasks[index++] = tasks[i];
        }
    }
    
    return priorityTasks;
}

void PriorityManager::cleanupExpiredTasks() {
    int newCount = 0;
    for (int i = 0; i < taskCount; i++) {
        if (!tasks[i]->isExpired()) {
            tasks[newCount++] = tasks[i];
        }
    }
    taskCount = newCount;
    sortByPriority();
}

bool PriorityManager::hasHighPriorityTasks() const {
    for (int i = 0; i < taskCount; i++) {
        if (tasks[i]->getPriority() == TaskPriority::HIGH &&
            !tasks[i]->isExpired() &&
            tasks[i]->getStatus() != TaskStatus::COMPLETED) {
            return true;
        }
    }
    return false;
}

double PriorityManager::getAveragePriority() const {
    if (taskCount == 0) return 0.0;
    
    double total = 0.0;
    for (int i = 0; i < taskCount; i++) {
        total += tasks[i]->getPriority();
    }
    return total / taskCount;
}
