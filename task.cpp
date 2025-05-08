#include "Task.h"
#include "PolicyEngine.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>

int Task::nextId = 1;

Task::Task() {
  this->taskId = 0;
  this->creator = nullptr;
  this->assignee = nullptr; 
}

Task::Task(User* creator, int ttlSeconds, TaskPriority priority) {
    this->taskId = nextId++;
    this->creator = creator;
    this->assignee = nullptr;
    this->status = CREATED;
    this->createdTime = time(0);
    this->ttlSeconds = ttlSeconds;
    this->assignedTime = 0;
    this->completedTime = 0;
    this->priority = priority;

    // Initialize signature to an empty string initially
    memset(signature, 0, sizeof(signature));
}

Task::~Task() {
    // Assignee and Creator are not deleted since they're managed elsewhere
}

// For Assigning Task to someone
bool Task::assignTo(User* assigner, User* target) {
    if (!assigner || !target) return false;

    if (PolicyEngine::canAccess(target, assigner->getClearanceLevel())) {
        assignee = target;
        assignedTime = time(0);
        status = ASSIGNED;
        return true;
    }
    return false;
}

void Task::updateStatus(TaskStatus newStatus) {
    status = newStatus;
    if (newStatus == COMPLETED) {
        completedTime = time(0);
        generateSignature();  // Generate digital signature on task completion
    }
}

void Task::checkAndExpire() {
    time_t now = time(0);
    double elapsedTime = difftime(now, createdTime);

    // If TTL has passed and the task isn't completed yet, expire the task
    if (status != COMPLETED && elapsedTime > ttlSeconds) {
        status = EXPIRED;
    }
}

Task& Task::operator=(const Task& other) {
    if (this != &other) {
        creator = other.creator;
        assignee = other.assignee;
        status = other.status;
        createdTime = other.createdTime;
        assignedTime = other.assignedTime;
        completedTime = other.completedTime;
        ttlSeconds = other.ttlSeconds;
        taskId = nextId++;
        priority = other.priority;
        // Copy the signature as well
        strncpy_s(signature, other.signature, sizeof(signature));
    }
    return *this;
}

ostream& operator<<(ostream& out, const Task& task) {
    out << "Task ID: " << task.getId() << "\n"
       << "Creator: " << task.getCreator()->getName() << "\n"
       << "Assignee: " << (task.getAssignee() ? task.getAssignee()->getName() : "None") << "\n"
       << "Status: " << task.getStatusString() << "\n"
       << "Priority: " << task.getPriorityString() << "\n"
       << "Signature: " << task.getSignature() << "\n";
    return out;
}

// GETTERS
int Task::getId() const { return taskId; }

TaskStatus Task::getStatus() const { return status; }

string Task::getStatusString() const {
    switch (status) {
        case CREATED: return "Created";
        case ASSIGNED: return "Assigned";
        case IN_PROGRESS: return "In Progress";
        case COMPLETED: return "Completed";
        case EXPIRED: return "Expired";
        default: return "Unknown";
    }
}

User* Task::getAssignee() const { return assignee; }
User* Task::getCreator() const { return creator; }

time_t Task::getCreatedTime() const { return createdTime; }

TaskPriority Task::getPriority() const { return priority; }

string Task::getPriorityString() const {
    switch (priority) {
        case HIGH: return "High";
        case MEDIUM: return "Medium";
        case LOW: return "Low";
        default: return "Unknown";
    }
}

void Task::generateSignature() {
    string data = to_string(taskId) + creator->getName();
    size_t hash = 0;
    for (size_t i = 0; i < data.length(); i++) {
        hash = (hash * 31 + data[i]) % 100000;
    }
    snprintf(signature, sizeof(signature), "%s-%05zu", creator->getName().c_str(), hash);
}

const char* Task::getSignature() const { return signature; }


// Add to Task.cpp
bool Task::isOverdue() const {
    if (status == COMPLETED) {
        return completedTime > deadline;
    }
    return time(0) > deadline;
}

bool Task::completeTask() {
    if (status != IN_PROGRESS) return false;
    status = COMPLETED;
    completedTime = time(0);
    generateSignature();
    return true;
}

double Task::completionTime() const {
    if (status != COMPLETED) return 0.0;
    return difftime(completedTime, createdTime);
}
