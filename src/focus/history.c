#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "focus/history.h"

#define HEADER "start,planned_minutes,elapsed,learning,daily,relax,work,other,distractions,completed\n"
#define MAX_SESSIONS 10000
#define RECENT_COUNT 5

const char *history_path(void)
{
    static char path[1024];
    const char *env = getenv("FOCUS_HISTORY");
    if (env && *env)
        return env;

    const char *home = getenv("HOME");
    snprintf(path, sizeof(path), "%s/.help_me_concentrate/history.csv", home ? home : ".");
    return path;
}

static void ensure_parent_dir(const char *path)
{
    char dir[1024];
    strncpy(dir, path, sizeof(dir) - 1);
    dir[sizeof(dir) - 1] = '\0';
    char *slash = strrchr(dir, '/');
    if (!slash || slash == dir)
        return;
    *slash = '\0';
    if (mkdir(dir, 0755) != 0 && errno != EEXIST)
        perror(dir);
}

int history_append(const struct Session *s)
{
    const char *path = history_path();
    ensure_parent_dir(path);

    struct stat st;
    int is_new = stat(path, &st) != 0 || st.st_size == 0;

    FILE *fp = fopen(path, "a");
    if (!fp) {
        perror(path);
        return -1;
    }
    if (is_new)
        fputs(HEADER, fp);

    fprintf(fp, "%ld,%d,%ld", (long)s->start, s->planned_minutes, s->elapsed);
    for (int t = 0; t < TASK_TYPE_COUNT; t++)
        fprintf(fp, ",%ld", s->totals[t]);
    fprintf(fp, ",%d,%d\n", s->distractions, s->completed);

    fclose(fp);
    return 0;
}

static int parse_line(const char *line, struct Session *s)
{
    long start;
    int n = sscanf(line, "%ld,%d,%ld,%ld,%ld,%ld,%ld,%ld,%d,%d",
                   &start, &s->planned_minutes, &s->elapsed,
                   &s->totals[Learning], &s->totals[Daily], &s->totals[Relax],
                   &s->totals[Work], &s->totals[Other],
                   &s->distractions, &s->completed);
    s->start = (time_t)start;
    return n == 10 ? 0 : -1;
}

/* Local calendar day number, so sessions group by the user's own midnight. */
static long local_day(time_t t)
{
    struct tm tm;
    localtime_r(&t, &tm);
    return (long)((t + tm.tm_gmtoff) / 86400);
}

struct Stats
{
    int sessions;
    int completed;
    long elapsed;
    long focused;
    int distractions;
};

static void add_to(struct Stats *st, const struct Session *s)
{
    st->sessions++;
    st->completed += s->completed;
    st->elapsed += s->elapsed;
    st->focused += s->totals[Work] + s->totals[Learning];
    st->distractions += s->distractions;
}

static void print_stats_row(const char *label, const struct Stats *st)
{
    long pct = st->elapsed > 0 ? st->focused * 100 / st->elapsed : 0;
    printf("  %-10s %4d sessions (%d done)  focused %4ldh %02ldm  (%3ld%%)  warnings %d\n",
           label, st->sessions, st->completed,
           st->focused / 3600, (st->focused % 3600) / 60, pct, st->distractions);
}

static int compute_streak(const struct Session *list, int count, long today)
{
    /* Mark days (counting back from today) that have a completed session. */
    enum { WINDOW = 366 };
    char has[WINDOW] = {0};
    for (int i = 0; i < count; i++) {
        long back = today - local_day(list[i].start);
        if (list[i].completed && back >= 0 && back < WINDOW)
            has[back] = 1;
    }

    /* A streak still counts if today has no session yet but yesterday does. */
    int d = has[0] ? 0 : 1;
    int streak = 0;
    while (d < WINDOW && has[d]) {
        streak++;
        d++;
    }
    return streak;
}

int history_print_stats(void)
{
    const char *path = history_path();
    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("No sessions yet. Run ./focus to start your first one!\n");
        return 0;
    }

    struct Session *list = malloc(sizeof(struct Session) * MAX_SESSIONS);
    if (!list) {
        fclose(fp);
        return -1;
    }

    int count = 0;
    char line[512];
    while (count < MAX_SESSIONS && fgets(line, sizeof(line), fp))
        if (parse_line(line, &list[count]) == 0)
            count++;
    fclose(fp);

    long today = local_day(time(NULL));
    struct Stats day = {0}, week = {0}, all = {0};
    for (int i = 0; i < count; i++) {
        long back = today - local_day(list[i].start);
        if (back == 0)
            add_to(&day, &list[i]);
        if (back >= 0 && back < 7)
            add_to(&week, &list[i]);
        add_to(&all, &list[i]);
    }

    printf("===== Focus stats =====\n");
    print_stats_row("Today", &day);
    print_stats_row("Last 7d", &week);
    print_stats_row("All time", &all);

    int streak = compute_streak(list, count, today);
    printf("\n  Streak: %d day%s", streak, streak == 1 ? "" : "s");
    printf(streak > 0 ? "  - keep it going!\n" : "  - finish a session today to start one.\n");

    if (count > 0) {
        printf("\n  Recent sessions:\n");
        for (int i = count > RECENT_COUNT ? count - RECENT_COUNT : 0; i < count; i++) {
            char when[32];
            struct tm tm;
            localtime_r(&list[i].start, &tm);
            strftime(when, sizeof(when), "%Y-%m-%d %H:%M", &tm);
            long focused = list[i].totals[Work] + list[i].totals[Learning];
            printf("    %s  %3d min planned  focused %3ldm  warnings %d  %s\n",
                   when, list[i].planned_minutes, focused / 60, list[i].distractions,
                   list[i].completed ? "done" : "stopped early");
        }
    }

    printf("\n  History file: %s\n", path);
    free(list);
    return 0;
}
