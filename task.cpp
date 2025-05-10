#include "Task.h"
#include <iostream>
#include "User.h"
#include"AuditLogger.h"
#include"PerformanceReview.h"
#include "AuditAnomalyDetector.h"

int Task::nextId = 1;

Task::Task(User* creator, const char* desc, int ttlSeconds, int priority) {
    this->taskId = nextId++;
    this->creator = creator;
    this->assignee = nullptr;
    this->status = TaskStatus::CREATED;
    this->ttlSeconds = ttlSeconds;
    this->priority = priority;
    
    strncpy(this->description, desc, sizeof(this->description) - 1);
    this->description[sizeof(this->description) - 1] = '\0';
    
    this->createdTime = time(0);
    this->deadline = createdTime + ttlSeconds;
    this->assignedTime = 0;
    this->completedTime = 0;
    
    memset(this->signature, 0, sizeof(this->signature));
}

Task::~Task() {
    // Cleanup if needed
}

bool Task::validateDelegation(User* from, User* to) const {
    if (!from || !to) return false;
    return to->getClearanceLevel() >= from->getClearanceLevel();
}

void Task::recursiveTimeoutCheck() {
    if (status != TaskStatus::COMPLETED && status != TaskStatus::EXPIRED) {
        time_t now = time(0);
        if (now >= deadline) {
            status = TaskStatus::EXPIRED;
            return;
        }
    }
}
// Modify the assignTo method
bool Task::assignTo(User* assigner, User* target) {
    static AuditAnomalyDetector detector;
    AuditLogger logger(assigner->getName().c_str());
    
    if (!assigner || !target) {
        logger.logTaskAction("ASSIGN_TASK", to_string(taskId).c_str(), "FAILED_NULL_USER");
        return false;
    }
    
    if (assigner != creator && assigner != assignee) {
        logger.logTaskAction("ASSIGN_TASK", to_string(taskId).c_str(), "DENIED_UNAUTHORIZED");
        return false;
    }
    
    if (!validateDelegation(assigner, target)) {
        logger.logTaskAction("ASSIGN_TASK", to_string(taskId).c_str(), "DENIED_CLEARANCE");
        return false;
    }

    assignee = target;
    assignedTime = time(0);
    status = TaskStatus::ASSIGNED;
    
    detector.checkTaskReassignment(assigner->getName().c_str(), 
                                 to_string(taskId).c_str());
    
    logger.logTaskAction("ASSIGN_TASK", to_string(taskId).c_str(), "SUCCESS");
    return true;
}


bool Task::delegateTo(User* from, User* to) {
    if (!from || !to) return false;
    if (from != assignee) return false;
    if (!validateDelegation(from, to)) return false;

    assignee = to;
    assignedTime = time(0);
    status = TaskStatus::ASSIGNED;
    return true;
}


// Modify the updateStatus method
void Task::updateStatus(int newStatus) {
    AuditLogger logger(assignee ? assignee->getName().c_str() : creator->getName().c_str());
    
    status = newStatus;
    if (newStatus == TaskStatus::COMPLETED) {
        completedTime = time(0);
        generateSignature();
        logger.logTaskAction("COMPLETE_TASK", to_string(taskId).c_str(), "SUCCESS");
    } else {
        logger.logTaskAction("UPDATE_TASK", to_string(taskId).c_str(), "STATUS_CHANGED");
    }
    if (assignee && (newStatus == TaskStatus::COMPLETED || 
        newStatus == TaskStatus::EXPIRED)) {
    PerformanceReview review(assignee->getName().c_str(), 
      assignee->getClearanceLevel());
      review.addTaskMetric(this);
    }

}

bool Task::completeTask(User* completer) {
    if (!completer || completer != assignee) return false;
    if (status != TaskStatus::IN_PROGRESS) return false;

    completedTime = time(0);
    status = TaskStatus::COMPLETED;
    generateSignature();
    return true;
}

void Task::checkExpiration() {
    recursiveTimeoutCheck();
}

void Task::generateSignature() {
    char buffer[356];
    sprintf(buffer, "%d%s%ld", taskId, creator->getName(), completedTime);
    
    // Simple hash generation
    unsigned long hash = 5381;
    int c;
    char* str = buffer;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    
    sprintf(signature, "%s-%lu", creator->getName().c_str(), hash % 100000);
}

const char* Task::getStatusString() const {
    switch (status) {
        case TaskStatus::CREATED: return "Created";
        case TaskStatus::ASSIGNED: return "Assigned";
        case TaskStatus::IN_PROGRESS: return "In Progress";
        case TaskStatus::COMPLETED: return "Completed";
        case TaskStatus::EXPIRED: return "Expired";
        default: return "Unknown";
    }
}

const char* Task::getPriorityString() const {
    switch (priority) {
        case TaskPriority::HIGH: return "High";
        case TaskPriority::MEDIUM: return "Medium";
        case TaskPriority::LOW: return "Low";
        default: return "Unknown";
    }
}

bool Task::isExpired() const {
    if (status == TaskStatus::COMPLETED) return false;
    return time(0) >= deadline;
}

double Task::getTimeToExpiration() const {
    if (status == TaskStatus::COMPLETED || status == TaskStatus::EXPIRED) 
        return 0.0;
    time_t now = time(0);
    return difftime(deadline, now);
}

Task& Task::operator=(const Task& other) {
    if (this != &other) {
        taskId = nextId++;
        strncpy(description, other.description, sizeof(description));
        creator = other.creator;
        assignee = other.assignee;
        status = other.status;
        priority = other.priority;
        createdTime = other.createdTime;
        assignedTime = other.assignedTime;
        completedTime = other.completedTime;
        deadline = other.deadline;
        ttlSeconds = other.ttlSeconds;
        strncpy(signature, other.signature, sizeof(signature));
    }
    return *this;
}

ostream& operator<<(ostream& os, const Task& task) {
    os << "Task #" << task.taskId << " - " << task.description << "\n"
       << "Status: " << task.getStatusString() << "\n"
       << "Priority: " << task.getPriorityString() << "\n"
       << "Creator: " << task.creator->getName() << "\n"
       << "Assignee: " << (task.assignee ? task.assignee->getName() : "None") << "\n"
       << "Time to expiration: " << task.getTimeToExpiration() << " seconds\n"
       << "Signature: " << task.signature;
    return os;
}




void Task::setPriority(int newPriority) {
    if (newPriority >= TaskPriority::HIGH && newPriority <= TaskPriority::LOW) {
        priority = newPriority;
    }
}

// Add to existing Task class implementation

// bool Task::approveTask(User* approver) {
//     char signature[64];
//     time_t now = time(0);
    
//     DigitalSignature::sign(approver->getName().c_str(), now, signature);
//     strncpy(this->signature, signature, sizeof(this->signature) - 1);
    
//     // Log the approval
//     EncryptedLogger logger;
//     char logEntry[256];
//     snprintf(logEntry, sizeof(logEntry), 
//              "Task #%d approved by %s at %ld", 
//              taskId, approver->getName().c_str(), now);
//     logger.writeLog(logEntry);
    
//     return true;
// }

// bool Task::delegateWithCycleCheck(User* from, User* to) {
//     static CycleDetector detector;
    
//     if (!detector.addDelegation(to->getName().c_str())) {
//         return false;  // Cycle detected or chain too long
//     }
    
//     bool result = delegateTo(from, to);
//     if (!result) {
//         detector.clear();
//     }
    
//     return result;
// }
