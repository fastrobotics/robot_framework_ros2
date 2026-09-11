cmake_minimum_required(VERSION 3.8)
project(robot_framework_ros2)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
option(BUILD_TESTING "Build the testing tree." ON)
option(CMAKE_EXPORT_COMPILE_COMMANDS "Export compilationcommands." ON)
# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
rosidl_generate_interfaces(
  ${PROJECT_NAME}
  "msg/ArmCommand.msg"
  "msg/ArmedState.msg"
  "msg/Diagnostic.msg"
  "msg/Heartbeat.msg"
  "msg/NodeState.msg"
  "msg/ReadyToArm.msg"
  "srv/ArmStateChange.srv"
  DEPENDENCIES
  builtin_interfaces)
include(FetchContent)

FetchContent_Declare(
  robot_framework
  GIT_REPOSITORY https://github.com/fastrobotics/robot_framework.git
  GIT_SHALLOW ON
  GIT_TAG master)
set(BUILD_TESTING
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(robot_framework)
set(BUILD_TESTING
    ON
    CACHE BOOL "" FORCE)

add_subdirectory(core)
ament_package()
