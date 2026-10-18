"""Find when you focus best, from the history saved by ./focus.

Usage: python3 src/ana/analyze.py [path/to/history.csv]
"""

import csv
import os
import sys
from collections import defaultdict
from datetime import datetime

WEEKDAYS = ["Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"]


def default_path():
    return os.environ.get("FOCUS_HISTORY") or os.path.expanduser(
        "~/.help_me_concentrate/history.csv"
    )


def load(path):
    with open(path, newline="") as f:
        for row in csv.DictReader(f):
            elapsed = int(row["elapsed"])
            if elapsed <= 0:
                continue
            yield {
                "start": datetime.fromtimestamp(int(row["start"])),
                "elapsed": elapsed,
                "focused": int(row["work"]) + int(row["learning"]),
                "distractions": int(row["distractions"]),
                "completed": row["completed"] == "1",
            }


def summarize(sessions, key):
    groups = defaultdict(lambda: {"elapsed": 0, "focused": 0, "count": 0, "done": 0})
    for s in sessions:
        g = groups[key(s)]
        g["elapsed"] += s["elapsed"]
        g["focused"] += s["focused"]
        g["count"] += 1
        g["done"] += s["completed"]
    return groups


def print_table(title, groups, label):
    print(f"\n{title}")
    for k in sorted(groups):
        g = groups[k]
        pct = 100 * g["focused"] // g["elapsed"]
        bar = "#" * (pct // 5)
        print(f"  {label(k):>6}  {pct:3d}% {bar:<20}  {g['count']} sessions, {g['done']} done")


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else default_path()
    if not os.path.exists(path):
        print(f"No history at {path}. Run ./focus first.")
        return 1

    sessions = list(load(path))
    if not sessions:
        print("History is empty. Run ./focus first.")
        return 1

    by_hour = summarize(sessions, lambda s: s["start"].hour)
    by_day = summarize(sessions, lambda s: s["start"].weekday())

    print_table("Focus % by start hour", by_hour, lambda h: f"{h:02d}:00")
    print_table("Focus % by weekday", by_day, lambda d: WEEKDAYS[d])

    best = max(by_hour, key=lambda h: by_hour[h]["focused"] / by_hour[h]["elapsed"])
    print(f"\nYou focus best when you start around {best:02d}:00. "
          "Try to plan your hardest work then.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
