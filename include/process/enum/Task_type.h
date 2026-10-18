#ifndef TASK_TYPE_H
#define TASK_TYPE_H

enum TaskType
{
    Learning,
    Daily,
    Relax,
    Work,
    Other,
    TASK_TYPE_COUNT,
};

const char *task_type_name(enum TaskType type);

#endif
