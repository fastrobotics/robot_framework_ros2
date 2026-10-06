# @compare_tag Node-SourceCompile v0.1`
find_package(sensor_msgs REQUIRED)
add_executable(depthcamera_pipeline_node DepthCameraPipelineNode.cpp)
target_include_directories(
  depthcamera_pipeline_node
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(
  depthcamera_pipeline_node
  PUBLIC "${cpp_typesupport_target}" sensor_msgs ${sensor_msgs_TARGETS}
  PRIVATE baseNode depthCameraPipelineSubsystem)
register_framework_rpath(depthcamera_pipeline_node logger)
install(TARGETS depthcamera_pipeline_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
