#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "focus/classify.h"
#include "proc_monitor/process_read.h"

#define POLL_SECONDS 2
#define DISTRACTION_GRACE_SECONDS 30
#define DEFAULT_FOCUS_MINUTES 25

static volatile sig_atomic_t stop_requested = 0;

static void on_sigint(int sig)
{
    (void)sig;
    stop_requested = 1;
}

static void notify(const char *title, const char *message)
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
             "osascript -e 'display notification \"%s\" with title \"%s\" sound name \"Ping\"' >/dev/null 2>&1",
             message, title);
    if (system(cmd) != 0)
        fprintf(stderr, "(notification failed)\n");
}

static void print_duration(long seconds)
{
    printf("%3ldm %02lds", seconds / 60, seconds % 60);
}

static void print_summary(const long totals[TASK_TYPE_COUNT], long elapsed, int distractions)
{
    printf("\n===== Focus summary =====\n");
    for (int t = 0; t < TASK_TYPE_COUNT; t++) {
        printf("  %-9s ", task_type_name((enum TaskType)t));
        print_duration(totals[t]);
        if (elapsed > 0)
            printf("  (%2ld%%)", totals[t] * 100 / elapsed);
        printf("\n");
    }
    long focused = totals[Work] + totals[Learning];
    printf("  ------------------------\n");
    printf("  Focused   ");
    print_duration(focused);
    printf("\n  Distraction warnings: %d\n", distractions);
}

static void usage(const char *prog)
{
    printf("Usage: %s [minutes]\n", prog);
    printf("  Start a focus session (default %d minutes). Press Ctrl+C to stop early.\n",
           DEFAULT_FOCUS_MINUTES);
}

int main(int argc, char *argv[])
{
    int minutes = DEFAULT_FOCUS_MINUTES;
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            usage(argv[0]);
            return 0;
        }
        minutes = atoi(argv[1]);
        if (minutes <= 0) {
            usage(argv[0]);
            return 1;
        }
    }

    signal(SIGINT, on_sigint);

    long totals[TASK_TYPE_COUNT] = {0};
    long relax_streak = 0;
    int distractions = 0;
    char current_app[256] = "";

    time_t start = time(NULL);
    time_t end = start + (time_t)minutes * 60;
    time_t last = start;

    printf("Focus session started: %d minutes. Ctrl+C to stop.\n", minutes);

    while (!stop_requested && time(NULL) < end) {
        sleep(POLL_SECONDS);

        time_t now = time(NULL);
        long delta = (long)(now - last);
        last = now;

        char app[256];
        if (get_frontmost_app(app, sizeof(app)) != 0)
            continue;

        enum TaskType type = classify_app(app);
        totals[type] += delta;

        if (strcmp(app, current_app) != 0) {
            strncpy(current_app, app, sizeof(current_app) - 1);
            long left = (long)(end - now);
            printf("[%2ld:%02ld left] %-20s -> %s\n", left / 60, left % 60, app, task_type_name(type));
        }

        if (type == Relax) {
            relax_streak += delta;
            if (relax_streak >= DISTRACTION_GRACE_SECONDS) {
                char msg[300];
                snprintf(msg, sizeof(msg), "You've been on %s for a while. Back to focus!", app);
                notify("Help me concentrate", msg);
                distractions++;
                relax_streak = 0;
            }
        } else {
            relax_streak = 0;
        }
    }

    if (!stop_requested)
        notify("Help me concentrate", "Focus session complete. Take a short break!");

    print_summary(totals, (long)(time(NULL) - start), distractions);
    return 0;
}
