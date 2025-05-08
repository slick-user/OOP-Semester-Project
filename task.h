#ifndef TASK_H
#define TASK_H

#include <cstring>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

struct TaskPriority {
    static const int HIGH = 0;
    static const int MEDIUM = 1;
    static const int LOW = 2;
};

struct TaskStatus {
    static const int CREATED = 0;
    static const int ASSIGNED = 1;
    static const int IN_PROGRESS = 2;
    static const int COMPLETED = 3;
    static const int EXPIRED = 4;
};

class User;  // Forward declaration

class Task {
private:
    static int nextId;
    int priority;
    int taskId;
    char description[256];
    User* creator;
    User* assignee;
    int status;  // Using TaskStatus values
    

    time_t createdTime;
    time_t assignedTime;
    time_t completedTime;
    time_t deadline;
    int ttlSeconds;
    
    char signature[100];
    void generateSignature();
    bool validateDelegation(User* from, User* to) const;
    void recursiveTimeoutCheck();

public:
    Task();
    Task(User* creator, const char* desc, int ttlSeconds, int priority);
    ~Task();

    // Core task management
    bool assignTo(User* assigner, User* target);
    bool delegateTo(User* from, User* to);
    bool completeTask(User* completer);
    void checkExpiration();
    void updateStatus(int newStatus);


    
    const char* getPriorityString() const;
    void setPriority(int newPriority);


    // Getters
    int getId() const { return taskId; }
    int getStatus() const { return status; }
    const char* getStatusString() const;
    User* getAssignee() const { return assignee; }
    User* getCreator() const { return creator; }
    time_t getCreatedTime() const { return createdTime; }
    time_t getDeadline() const { return deadline; }
    int getPriority() const { return priority; }
    
    const char* getSignature() const { return signature; }
    const char* getDescription() const { return description; }
    bool isExpired() const;
    double getTimeToExpiration() const;

    // Task modification
    void setDescription(const char* desc) {
        strncpy(description, desc, sizeof(description) - 1);
        description[sizeof(description) - 1] = '\0';
    }

    // Operators
    Task& operator=(const Task& other);
    friend ostream& operator<<(ostream& os, const Task& task);
};

#endif
