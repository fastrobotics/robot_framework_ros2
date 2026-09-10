[![Build and Test CI](https://github.com/fastrobotics/robot_framework_ros2/actions/workflows/build-test.yml/badge.svg)](https://github.com/fastrobotics/robot_framework_ros2/actions/workflows/build-test.yml)

# FAST Robotics - Robot Framework: ROS v2 Middleware

- [FAST Robotics - Robot Framework: ROS v2 Middleware](#fast-robotics---robot-framework-ros-v2-middleware)
- [Architecture](#architecture)
- [Interfaces](#interfaces)
- [Systems](#systems)
- [Features](#features)
- [Setup](#setup)
- [Build](#build)
  - [Build and run Unit Tests](#build-and-run-unit-tests)



# Architecture
![](Legend.png)

# Interfaces
![](msg/puml/RobotFrameworkROS2MessageDiagram.png)
![](srv/puml/RobotFrameworkROS2ServiceDiagram.png)

# Systems

# Features
| Status | Feature                                          |
| ------ | ------------------------------------------------ |
| DRAFT  | [Core](include/robot_framework_ros2/doc/Core.md) |

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

## Build and run Unit Tests
```bash
cd <workspace>
colcon build
source install/setup.bash
colcon test --event-handlers console_cohesion+
```