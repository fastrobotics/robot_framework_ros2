[![Build and Test CI](https://github.com/fastrobotics/robot_framework_ros2/actions/workflows/build-test.yml/badge.svg)](https://github.com/fastrobotics/robot_framework_ros2/actions/workflows/build-test.yml)

# FAST Robotics - Robot Framework: ROS v2 Middleware

- [FAST Robotics - Robot Framework: ROS v2 Middleware](#fast-robotics---robot-framework-ros-v2-middleware)
- [Architecture](#architecture)
- [Setup](#setup)
- [Build](#build)



# Architecture
![](Legend.png)


# Setup

Pre-Requisites:

- Ubuntu system running 24.04 LTS

1. Clone this repo using:
```
git clone --recurse-submodules https://github.com/fastrobotics/robot_framework_ros2.git
cd robot_framework_ros2
git submodule update --remote
```
2. Run the following:

```
cd <repo>
./scripts/setup_ide.sh
./scripts/setup_robot.sh
```

# Build
To build, run the following:
```
cd <workspace>
colcon build
```

To refresh the cmake cache, instead run:
```
colcon build --cmake-clean-cache
```