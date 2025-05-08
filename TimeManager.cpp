#include "TimeManager.h"

void TimeManager::expireTasks(Task** tasks, int taskCount) {
    for (int i = 0; i < taskCount; ++i) {
        if (tasks[i]) {
            tasks[i]->checkExpiration();  // Changed from checkAndExpire to checkExpiration
        }
    }
}