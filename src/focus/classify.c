#include <strings.h>
#include <string.h>

#include "focus/classify.h"

struct AppRule
{
    const char *keyword;
    enum TaskType type;
};

/* Matched case-insensitively against the app name; first match wins.
 * Edit this table to fit your own apps. */
static const struct AppRule rules[] = {
    {"Code", Work},
    {"Xcode", Work},
    {"Terminal", Work},
    {"iTerm", Work},
    {"CLion", Work},
    {"PyCharm", Work},
    {"Word", Work},
    {"Excel", Work},
    {"PowerPoint", Work},
    {"Pages", Work},
    {"Notion", Learning},
    {"Obsidian", Learning},
    {"Anki", Learning},
    {"Preview", Learning},
    {"Books", Learning},
    {"Safari", Other},
    {"Chrome", Other},
    {"Firefox", Other},
    {"Mail", Daily},
    {"Calendar", Daily},
    {"Reminders", Daily},
    {"WeChat", Relax},
    {"QQ", Relax},
    {"Discord", Relax},
    {"Telegram", Relax},
    {"Music", Relax},
    {"Spotify", Relax},
    {"TV", Relax},
    {"Steam", Relax},
    {"bilibili", Relax},
};

static int contains_ignore_case(const char *haystack, const char *needle)
{
    size_t n = strlen(needle);
    for (; *haystack; haystack++)
        if (strncasecmp(haystack, needle, n) == 0)
            return 1;
    return 0;
}

enum TaskType classify_app(const char *app_name)
{
    for (size_t i = 0; i < sizeof(rules) / sizeof(rules[0]); i++)
        if (contains_ignore_case(app_name, rules[i].keyword))
            return rules[i].type;
    return Other;
}

const char *task_type_name(enum TaskType type)
{
    static const char *names[TASK_TYPE_COUNT] = {
        "Learning", "Daily", "Relax", "Work", "Other",
    };
    if (type < 0 || type >= TASK_TYPE_COUNT)
        return "Unknown";
    return names[type];
}
