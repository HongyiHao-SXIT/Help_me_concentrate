#ifndef PROCESS_READ_H
#define PROCESS_READ_H

#include <stddef.h>

/* Copy the name of the app currently in front into buf.
 * Returns 0 on success, -1 on failure. */
int get_frontmost_app(char *buf, size_t len);

/* Returns 1 if any running process path contains name, 0 otherwise. */
int is_process_running(const char *name);

#endif
