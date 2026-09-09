

# FAST Robotics - Robot Framework: ROS v2 Middleware

- [FAST Robotics - Robot Framework: ROS v2 Middleware](#fast-robotics---robot-framework-ros-v2-middleware)
- [Requirements](#requirements)
- [Architecture](#architecture)
- [Setup](#setup)

# Requirements
[Requirements](doc/Requirements/Requirements.md)

# Architecture
![](Legend.png)


# Setup

Pre-Requisites:

- Ubuntu system running 24.04 LTS

1. Clone this repo using:
```bash
git clone --recurse-submodules https://github.com/fastrobotics/robot_framework_ros2.git
cd robot_framework_ros2
git submodule update --remote
```
2. Run the following:

```bash
cd <repo>
./scripts/setup_ide.sh
./scripts/setup_robot.sh
```
