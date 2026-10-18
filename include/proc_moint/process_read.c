#include <stdio.h>
#include <string.h>
#include <libproc.h>

int main(void) {
    pid_t pids[4096];
    int bytes = proc_listpids(PROC_ALL_PIDS, 0, pids, sizeof(pids));
    int count = bytes / sizeof(pid_t);

    char path[PROC_PIDPATHINFO_MAXSIZE];
    for (int i = 0; i < count; i++) {
        if (pids[i] == 0) continue;
        if (proc_pidpath(pids[i], path, sizeof(path)) > 0) {
            if (strstr(path, "Safari"))
                printf("%d  %s\n", pids[i], path);
        }
    }
    return 0;
}