#include <stdio.h>
#include <string.h>
#include <libproc.h>

#include "proc_monitor/process_read.h"

int get_frontmost_app(char *buf, size_t len)
{
    /* lsappinfo prints: "LSDisplayName"="Safari" */
    FILE *fp = popen("lsappinfo info -only name \"$(lsappinfo front)\" 2>/dev/null", "r");
    if (!fp)
        return -1;

    char line[512];
    int found = -1;
    if (fgets(line, sizeof(line), fp)) {
        char *start = strstr(line, "=\"");
        if (start) {
            start += 2;
            char *end = strchr(start, '"');
            if (end) {
                size_t n = (size_t)(end - start);
                if (n >= len)
                    n = len - 1;
                memcpy(buf, start, n);
                buf[n] = '\0';
                found = 0;
            }
        }
    }
    pclose(fp);
    return found;
}

int is_process_running(const char *name)
{
    pid_t pids[4096];
    int bytes = proc_listpids(PROC_ALL_PIDS, 0, pids, sizeof(pids));
    int count = bytes / (int)sizeof(pid_t);

    char path[PROC_PIDPATHINFO_MAXSIZE];
    for (int i = 0; i < count; i++) {
        if (pids[i] == 0)
            continue;
        if (proc_pidpath(pids[i], path, sizeof(path)) > 0 && strstr(path, name))
            return 1;
    }
    return 0;
}
