# @compare_tag Node-SourceCompile v0.1`
add_executable(depthcamera_pipeline_node DepthCameraPipelineNode.cpp)
target_include_directories(
  depthcamera_pipeline_node
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
target_link_libraries(depthcamera_pipeline_node PRIVATE baseNode)
register_framework_rpath(depthcamera_pipeline_node logger)
install(TARGETS depthcamera_pipeline_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
