#ifndef HISTORY_H
#define HISTORY_H

#include <time.h>

#include "process/enum/Task_type.h"

struct Session
{
    time_t start;
    int planned_minutes;
    long elapsed;
    long totals[TASK_TYPE_COUNT];
    int distractions;
    int completed;
};

/* Path of the history CSV: $FOCUS_HISTORY, or ~/.help_me_concentrate/history.csv */
const char *history_path(void);

/* Append one finished session to the history file. Returns 0 on success. */
int history_append(const struct Session *s);

/* Print today / last 7 days / all-time stats, streak and recent sessions. */
int history_print_stats(void);

#endif
