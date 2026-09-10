cmake_minimum_required(VERSION 3.8)
project(robot_framework_ros2)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

include(FetchContent)

# Declare the robot_framework dependency
FetchContent_Declare(
  robot_framework
  GIT_REPOSITORY https://github.com/fastrobotics/robot_framework.git
  GIT_SHALLOW    ON
  GIT_TAG        devops5713/C++CompilerMigration
)

# Force tests off if you don't want to build robot_framework's internal tests
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

# ACTUALLY FETCH AND MAKE AVAILABLE (Missing in your snippet)
FetchContent_MakeAvailable(robot_framework)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
