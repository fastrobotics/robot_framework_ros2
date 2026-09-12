find_package(ament_cmake_ros REQUIRED)
find_package(launch_testing_ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)

# 1. Compile the C++ Tester Agent binary
add_executable(test_example_node test_example_node.cpp)
target_link_libraries(test_example_node gtest gtest_main rclcpp::rclcpp logger
                      ${PROJECT_NAME}__rosidl_typesupport_cpp)
register_framework_rpath(test_example_node logger)

install(TARGETS test_example_node DESTINATION lib/${PROJECT_NAME})
add_launch_test(run_test_example_node.py)
