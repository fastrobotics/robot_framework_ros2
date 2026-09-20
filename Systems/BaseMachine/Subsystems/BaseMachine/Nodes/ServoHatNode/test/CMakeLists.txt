# @compare_tag Node-SourceTestCompile v0.2`
find_package(ament_cmake_ros REQUIRED)
find_package(launch_testing_ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)

add_executable(test_servo_hat_node test_ServoHatNode.cpp)
target_link_libraries(
  test_servo_hat_node
  gtest
  gtest_main
  rclcpp::rclcpp
  std_msgs::std_msgs
  logger
  ${PROJECT_NAME}__rosidl_typesupport_cpp)
target_include_directories(
  test_servo_hat_node PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
register_framework_rpath(test_servo_hat_node logger)

install(TARGETS test_servo_hat_node DESTINATION lib/${PROJECT_NAME})
add_launch_test(${CMAKE_CURRENT_SOURCE_DIR}/run_test_ServoHatNode.py)
