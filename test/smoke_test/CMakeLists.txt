find_package(ament_cmake_gtest REQUIRED)
find_package(launch_testing_ament_cmake REQUIRED)

ament_add_gtest_executable(test_smoketest test_smoketest.cpp)
target_link_libraries(test_smoketest rclcpp::rclcpp logger
                      ${PROJECT_NAME}__rosidl_typesupport_cpp)
ament_target_dependencies(test_smoketest rclcpp)
register_framework_rpath(test_smoketest logger)
install(TARGETS test_smoketest DESTINATION lib/${PROJECT_NAME})

add_launch_test(test_smoketest_orchestrator.launch.py)
