#include "enum/Task_type.h"
#include <time.h>

struct Task
{
    int id;
    char task_name[256];
    enum TaskType task_type;
    time_t total_task_time;

    time_t start_task_time;
    time_t end_task_time;
};