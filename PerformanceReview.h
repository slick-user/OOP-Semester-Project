#ifndef PERFORMANCE_REVIEW_H
#define PERFORMANCE_REVIEW_H

#include <ctime>
#include <cstring>
#include "User.h"
#include "Task.h"

struct PerformanceMetrics {
    int totalTasks;
    int completedTasks;
    int expiredTasks;
    int messagesHandled;
    double averageResponseTime;
    char lastReviewDate[26];
    
    PerformanceMetrics() {
        totalTasks = 0;
        completedTasks = 0;
        expiredTasks = 0;
        messagesHandled = 0;
        averageResponseTime = 0.0;
        memset(lastReviewDate, 0, sizeof(lastReviewDate));
    }
};

class PerformanceReview {
private:
    char employeeName[50];
    int clearanceLevel;
    PerformanceMetrics metrics;
    char reviewPeriod[20];
    
    void calculateScore();
    bool saveToFile() const;
    bool loadFromFile();
    void updateMetrics(const Task* task);

public:
    PerformanceReview(const char* name, int level);
    
    // Core functionality
    void addTaskMetric(const Task* task);
    void addMessageMetric(time_t responseTime);
    double getPerformanceScore() const;
    void generateReport() const;
    
    // Getters
    const char* getEmployeeName() const { return employeeName; }
    int getClearanceLevel() const { return clearanceLevel; }
    const PerformanceMetrics& getMetrics() const { return metrics; }
    
    // Operator overloading for report generation
    friend ostream& operator<<(ostream& os, const PerformanceReview& review);
};

#endif