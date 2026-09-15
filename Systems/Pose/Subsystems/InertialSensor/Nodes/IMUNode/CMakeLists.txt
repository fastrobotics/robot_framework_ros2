# @compare_tag Node-SourceCompile v0.1`
add_executable(imu_node IMUNode.cpp)
target_include_directories(
  imu_node PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                   $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
target_link_libraries(
  imu_node PRIVATE geometry_msgs::geometry_msgs sensor_msgs::sensor_msgs
                   baseNode imuProcess)
register_framework_rpath(imu_node logger imuProcess)
install(TARGETS imu_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
