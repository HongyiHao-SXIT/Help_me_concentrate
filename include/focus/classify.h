#ifndef CLASSIFY_H
#define CLASSIFY_H

#include "process/enum/Task_type.h"

/* Decide which TaskType an app belongs to, based on its name. */
enum TaskType classify_app(const char *app_name);

#endif
