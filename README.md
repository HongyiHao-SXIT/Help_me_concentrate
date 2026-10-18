# Help me concentrate

A small macOS command-line tool that helps people (including people with ADHD) stay focused.

It runs a timed focus session (Pomodoro style, 25 minutes by default), checks which app you are using every 2 seconds, and sorts it into a category: **Work**, **Learning**, **Daily**, **Relax** or **Other**. If a distracting (Relax) app stays in front for more than 30 seconds, you get a gentle notification. When the session ends, it shows how your time was spent.

## Build and run

```sh
make
./focus        # 25-minute session
./focus 50     # 50-minute session
```

Press `Ctrl+C` to end a session early and see the summary.

## Example output

```
Focus session started: 25 minutes. Ctrl+C to stop.
[24:58 left] Code                 -> Work
[12:10 left] WeChat               -> Relax

===== Focus summary =====
  Learning    0m 00s  ( 0%)
  Daily       0m 00s  ( 0%)
  Relax       1m 04s  ( 4%)
  Work       23m 56s  (95%)
  Other       0m 00s  ( 0%)
  ------------------------
  Focused    23m 56s
  Distraction warnings: 1
```

## Customizing

Edit the `rules` table in [src/focus/classify.c](src/focus/classify.c) to choose which apps count as Work, Learning, Relax and so on, then run `make` again.

## Project layout

- `src/main.c`: focus session loop, notifications and summary
- `src/focus/classify.c`: app-to-category rules
- `src/proc_monitor/process_read.c`: reads the frontmost app and running processes on macOS
- `include/`: headers
