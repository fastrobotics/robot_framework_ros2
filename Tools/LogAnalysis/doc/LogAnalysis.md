[Tools](../../doc/Tools.md)

- [Log Analysis](#log-analysis)
  - [Todo List](#todo-list)
  - [Overview](#overview)
  - [Log Analysis Script](#log-analysis-script)
  - [Log Playback](#log-playback)
- [Log Troubleshooting](#log-troubleshooting)
  - [Issue `Could not find metadata in bag directory ...`](#issue-could-not-find-metadata-in-bag-directory-)

# Log Analysis
## Todo List
- Output markdown file to a temp folder
- Break log analysis script into sections (diagnostics, pose, perception, etc)
- Figure out starttime issue
- Add notes to how to run playback on specific logs automatically
## Overview
These tools provide mechanisms to analyze ROS2 Log Files

## Log Analysis Script

## Log Playback
# Log Troubleshooting
## Issue `Could not find metadata in bag directory ...`
Run either:
```bash
ros2 bag reindex -s mcap <folder>
```

```bash
ros2 bag reindex -s sqlite3 <folder>
```

dependening on the log file format