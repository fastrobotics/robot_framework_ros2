cmake_minimum_required(VERSION 3.8)
project(robot_framework_ros2)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage -fpermissive ${WARNING_FLAGS}"
)
if(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL armv7l)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARCHITECTURE_ARMV7L")
  add_compile_options(-Wno-psabi)
endif()
add_definitions(-Wall -Wextra -Wno-switch -Werror)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
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

function(register_framework_rpath TARGET_NAME)
  # ARGN captures all arguments passed after TARGET_NAME (the list of
  # dependencies)
  set(RPATH_LIST "")

  foreach(dependency ${ARGN})
    # Dynamically build the generator expression for each dependency
    list(APPEND RPATH_LIST "$<TARGET_FILE_DIR:${dependency}>")
  endforeach()

  # Apply the combined list to your target properties
  set_target_properties(${TARGET_NAME} PROPERTIES INSTALL_RPATH "${RPATH_LIST}")
endfunction()
function(add_ros_isolated_launch_test path)
  set(RUNNER "${ament_cmake_ros_DIR}/run_test_isolated.py")
  add_launch_test("${path}" RUNNER "${RUNNER}" ${ARGN})
endfunction()
include(FetchContent)

FetchContent_Declare(
  robot_framework
  GIT_REPOSITORY https://github.com/fastrobotics/robot_framework.git
  GIT_SHALLOW ON
  GIT_TAG devops5716/BuildBaseTemplateNode)
set(BUILD_TESTING
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(robot_framework)
file(GLOB_RECURSE ENTIRE_HEADER_LIST "${robot_framework_SOURCE_DIR}/msgs/*.hpp"
     "${robot_framework_SOURCE_DIR}/Systems/*.hpp"
     "${robot_framework_SOURCE_DIR}/Systems/*.h")
set(RECURSIVE_INCLUDE_DIRS "")
foreach(HEADER_FILE ${ENTIRE_HEADER_LIST})
  get_filename_component(HEADER_DIR ${HEADER_FILE} DIRECTORY)
  list(APPEND RECURSIVE_INCLUDE_DIRS ${HEADER_DIR})
endforeach()

# 1. Clean up any duplicate directory tracks to keep the compiler fast
if(RECURSIVE_INCLUDE_DIRS)
  list(REMOVE_DUPLICATES RECURSIVE_INCLUDE_DIRS)
endif()
include_directories(include ${robot_framework_SOURCE_DIR}
                    ${RECURSIVE_INCLUDE_DIRS})
set(BUILD_TESTING
    ON
    CACHE BOOL "" FORCE)

add_subdirectory(core)
add_subdirectory(nodes)

ament_package()
