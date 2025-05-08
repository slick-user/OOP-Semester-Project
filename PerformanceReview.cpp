#include "PerformanceReview.h"
#include <iostream>
#include <fstream>


PerformanceReview::PerformanceReview(const char* name, int level) {
    strncpy(employeeName, name, sizeof(employeeName) - 1);
    employeeName[sizeof(employeeName) - 1] = '\0';
    clearanceLevel = level;
    
    time_t now = time(0);
    strftime(reviewPeriod, sizeof(reviewPeriod), "%Y-%m", localtime(&now));
    
    loadFromFile();
}

void PerformanceReview::addTaskMetric(const Task* task) {
    if (!task) return;
    
    metrics.totalTasks++;
    
    if (task->getStatus() == TaskStatus::COMPLETED) {
        metrics.completedTasks++;
    }
    else if (task->getStatus() == TaskStatus::EXPIRED) {
        metrics.expiredTasks++;
    }
    
    saveToFile();
}

void PerformanceReview::addMessageMetric(time_t responseTime) {
    metrics.messagesHandled++;
    double oldTotal = metrics.averageResponseTime * (metrics.messagesHandled - 1);
    metrics.averageResponseTime = (oldTotal + responseTime) / metrics.messagesHandled;
    
    saveToFile();
}

double PerformanceReview::getPerformanceScore() const {
    double taskScore = 0.0;
    if (metrics.totalTasks > 0) {
        taskScore = (metrics.completedTasks * 100.0) / metrics.totalTasks;
        taskScore -= (metrics.expiredTasks * 10.0);  // Penalty for expired tasks
    }
    
    double responseScore = 0.0;
    if (metrics.messagesHandled > 0) {
        responseScore = 100.0 * (1.0 - (metrics.averageResponseTime / 86400.0)); // 24 hours baseline
    }
    
    // Weight: 70% tasks, 30% response time
    return (taskScore * 0.7) + (responseScore * 0.3);
}

bool PerformanceReview::saveToFile() const {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_%s_performance.dat", 
             employeeName, reviewPeriod);
    
    ofstream file(filename, ios::binary);
    if (!file.is_open()) return false;
    
    file.write((char*)&metrics, sizeof(PerformanceMetrics));
    file.close();
    return true;
}

bool PerformanceReview::loadFromFile() {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_%s_performance.dat", 
             employeeName, reviewPeriod);
    
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return false;
    
    file.read((char*)&metrics, sizeof(PerformanceMetrics));
    file.close();
    return true;
}

void PerformanceReview::generateReport() const {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_%s_report.txt", 
             employeeName, reviewPeriod);
    
    ofstream report(filename);
    if (!report.is_open()) return;
    
    report << *this;
    report.close();
}

ostream& operator<<(ostream& os, const PerformanceReview& review) {
    os << "=== Performance Review Report ===\n"
       << "Employee: " << review.employeeName << "\n"
       << "Clearance Level: " << review.clearanceLevel << "\n"
       << "Review Period: " << review.reviewPeriod << "\n\n"
       
       << "Task Metrics:\n"
       << "- Total Tasks: " << review.metrics.totalTasks << "\n"
       << "- Completed Tasks: " << review.metrics.completedTasks << "\n"
       << "- Expired Tasks: " << review.metrics.expiredTasks << "\n"
       << "- Completion Rate: " 
       << (review.metrics.totalTasks > 0 ? 
           (review.metrics.completedTasks * 100.0) / review.metrics.totalTasks : 0)
       << "%\n\n"
       
       << "Communication Metrics:\n"
       << "- Messages Handled: " << review.metrics.messagesHandled << "\n"
       << "- Average Response Time: " 
       << (review.metrics.averageResponseTime / 3600.0) << " hours\n\n"
       
       << "Overall Performance Score: " << review.getPerformanceScore() << "%\n"
       << "==============================\n";
    
    return os;
}