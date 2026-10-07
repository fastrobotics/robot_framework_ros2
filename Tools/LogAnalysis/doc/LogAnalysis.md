[Tools](../../doc/Tools.md)

- [Log Analysis](#log-analysis)
  - [Overview](#overview)
  - [Log Analysis Script](#log-analysis-script)
    - [Configuration](#configuration)
    - [Generated Files](#generated-files)
  - [Log Playback](#log-playback)
    - [Setup](#setup)
      - [Robot Name](#robot-name)
      - [Environment](#environment)
- [Log Troubleshooting](#log-troubleshooting)
  - [Issue `Could not find metadata in bag directory ...`](#issue-could-not-find-metadata-in-bag-directory-)

# Log Analysis
## Overview
These tools provide mechanisms to analyze ROS2 Log Files

## Log Analysis Script
```bash
python Tools/LogAnalysis/scripts/log_analyzer.py <bag-directory-or-parent-folder>
```

The script accepts one bag directory or a parent directory containing multiple bags. It recursively finds bag directories containing `metadata.yaml` or `.mcap` files. Each discovered bag is analyzed independently. Worker output is streamed while analysis runs and prefixed with the bag's relative path; folder-level progress reports how many bags are complete and how many remain.

Bag folders are processed in parallel using separate worker processes. The current default limit is eight simultaneous bags. Change `MAX_PARALLEL_BAGS` near the top of `scripts/log_analyzer.py` to adjust this; use `1` to process sequentially or reduce resource usage.

### Configuration
At the top of `scripts/log_analyzer.py`:
- `ANALYZER_PLUGIN_CONFIG` enables or disables each analyzer plugin by class name. Unlisted plugins are disabled.
- `TIMESTAMP_ANALYSIS_TOPICS` selects topic names for timestamp CSV and rate/plot analysis. These outputs are produced only when `TimestampRateAnalyzer` is enabled.
- `MAX_PARALLEL_BAGS` sets the maximum number of bag folders analyzed at once.

### Generated Files
Each bag’s output is written inside its bag directory:
- `rosbag_metadata_summary.md` contains bag metadata, message statistics, plugin reports, and timestamp analysis when enabled.
- `rosout.txt` contains the complete rosout text when `RosoutTextAnalyzer` is enabled. The Markdown report summarizes severity counts and includes warning-or-higher messages.
- `csv/` contains one selected-topic timestamp CSV per topic when timestamp analysis is enabled.
- `plots/` contains per-topic timestamp-delta PNG plots embedded in the Markdown report. Plot generation requires Matplotlib.

If an MCAP bag has no `metadata.yaml`, the script attempts to reindex it with `ros2 bag reindex -s mcap` before analysis.

## Log Playback
### Setup
#### Robot Name
- Determine the robot name used for the log.  This will be used later as <robot_name>

#### Environment
If not already done, add this to your ~/.bashrc
```bash
set_ros_domain() {
    if [ -z "$1" ]; then
        echo "Error: Please provide a ROS Domain ID. Example: setros 42"
    else
        export ROS_DOMAIN_ID=$1
        echo "ROS_DOMAIN_ID set to $ROS_DOMAIN_ID"
    fi
}
```
Then source your environment `source ~/.bashrc`

Then set your environment: `set_ros_domain <Number>` # <Number> is used for every terminal for playback, but importantly should be different than any other ROS system that is actively running on your network.

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