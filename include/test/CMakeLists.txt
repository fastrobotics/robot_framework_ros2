find_package(ament_cmake_ros REQUIRED)
find_package(launch_testing_ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)

# 1. Compile the C++ Tester Agent binary
add_executable(test_example_node test_ExampleNode.cpp)
target_link_libraries(test_example_node gtest gtest_main rclcpp::rclcpp logger
                      ${PROJECT_NAME}__rosidl_typesupport_cpp)
target_include_directories(
  test_example_node PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
register_framework_rpath(test_example_node logger)

install(TARGETS test_example_node DESTINATION lib/${PROJECT_NAME})
add_launch_test(run_test_ExampleNode.py)
