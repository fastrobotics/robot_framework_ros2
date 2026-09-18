# @compare_tag Node-SourceTestCompile v0.2`
find_package(ament_cmake_ros REQUIRED)
find_package(launch_testing_ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)

add_executable(test_inertial_sensor_fuser_node test_InertialSensorFuserNode.cpp)
target_link_libraries(
  test_inertial_sensor_fuser_node
  gtest
  gtest_main
  rclcpp::rclcpp
  logger
  sensor_msgs::sensor_msgs
  ${PROJECT_NAME}__rosidl_typesupport_cpp)
target_include_directories(
  test_inertial_sensor_fuser_node
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
register_framework_rpath(test_inertial_sensor_fuser_node logger)

install(TARGETS test_inertial_sensor_fuser_node DESTINATION lib/${PROJECT_NAME})
add_launch_test(${CMAKE_CURRENT_SOURCE_DIR}/run_test_InertialSensorFuserNode.py)
