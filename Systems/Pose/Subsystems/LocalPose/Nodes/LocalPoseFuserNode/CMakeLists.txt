# @compare_tag Node-SourceCompile v0.1`
add_executable(local_pose_fuser_node LocalPoseFuserNode.cpp)
target_include_directories(
  local_pose_fuser_node
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
target_link_libraries(
  local_pose_fuser_node
  PRIVATE baseNode basic_LocalPoseFuserProcess sensor_msgs::sensor_msgs
          geometry_msgs::geometry_msgs nav_msgs::nav_msgs)
register_framework_rpath(local_pose_fuser_node logger
                         basic_LocalPoseFuserProcess)
install(TARGETS local_pose_fuser_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
