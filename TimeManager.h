#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "Task.h"

class TimeManager {
public:
    static void expireTasks(Task** tasks, int taskCount);
};

#endif
