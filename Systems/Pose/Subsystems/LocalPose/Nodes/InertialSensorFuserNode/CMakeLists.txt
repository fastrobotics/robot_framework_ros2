# @compare_tag Node-SourceCompile v0.1`
add_executable(inertial_sensor_fuser_node InertialSensorFuserNode.cpp)
target_include_directories(
  inertial_sensor_fuser_node
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
target_link_libraries(
  inertial_sensor_fuser_node PRIVATE baseNode sensor_msgs::sensor_msgs
                                     basic_InertialSensorFuserProcess)
register_framework_rpath(inertial_sensor_fuser_node logger
                         basic_InertialSensorFuserProcess)
install(TARGETS inertial_sensor_fuser_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
