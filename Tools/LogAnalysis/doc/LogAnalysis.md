[Tools](../../doc/Tools.md)

- [Log Analysis](#log-analysis)
  - [Overview](#overview)
  - [Log Analysis Script](#log-analysis-script)
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
python Tools/LogAnalysis/scripts/log_analyzer.py <Path to Bag Directory>
```
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