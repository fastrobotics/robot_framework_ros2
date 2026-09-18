# @compare_tag Node-SourceTestCompile v0.2`
find_package(ament_cmake_ros REQUIRED)
find_package(launch_testing_ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)

add_executable(test_imu_node test_IMUNode.cpp)
target_link_libraries(
  test_imu_node
  gtest
  gtest_main
  geometry_msgs::geometry_msgs
  sensor_msgs::sensor_msgs
  rclcpp::rclcpp
  logger
  ${PROJECT_NAME}__rosidl_typesupport_cpp)
target_include_directories(
  test_imu_node PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
register_framework_rpath(test_imu_node logger)

install(TARGETS test_imu_node DESTINATION lib/${PROJECT_NAME})
add_launch_test(${CMAKE_CURRENT_SOURCE_DIR}/run_test_IMUNode.py)
