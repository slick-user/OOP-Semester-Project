#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>
#include "User.h"

using namespace std;

enum TaskPriority {
    HIGH,
    MEDIUM,
    LOW
};

enum TaskStatus {
    CREATED,
    ASSIGNED,
    IN_PROGRESS,
    COMPLETED,
    EXPIRED
};

class Task {
private:
    static int nextId;
    int taskId;
    string description;
    User* creator;
    User* assignee;
    TaskStatus status;
    TaskPriority priority;

    time_t createdTime;
    time_t assignedTime;
    time_t completedTime;
    time_t deadline;

    int ttlSeconds;
    char signature[100];
    
    void generateSignature();

public:
    Task();
    Task(User* creator, int ttlSeconds, TaskPriority priority);
    ~Task();

    bool assignTo(User* assigner, User* target);
    void updateStatus(TaskStatus newStatus);
    void checkAndExpire();
    bool completeTask();
    bool isOverdue() const;
    double completionTime() const;

    // Operators
    Task& operator=(const Task& other);
    friend ostream& operator<<(ostream& os, const Task& task);

    // Getters
    int getId() const;
    TaskStatus getStatus() const;
    string getStatusString() const;
    User* getAssignee() const;
    User* getCreator() const;
    time_t getCreatedTime() const;
    TaskPriority getPriority() const;
    string getPriorityString() const;
    const char* getSignature() const;
    const string& getDescription() const { return description; }
};

#endif
