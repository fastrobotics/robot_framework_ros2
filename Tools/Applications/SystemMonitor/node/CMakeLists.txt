# @compare_tag Node-SourceCompile v0.1`
add_executable(system_monitor SystemMonitorNode.cpp)
target_include_directories(
  system_monitor
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
target_link_libraries(system_monitor PRIVATE baseNode headerWindow)
register_framework_rpath(system_monitor logger)
install(TARGETS system_monitor DESTINATION lib/${PROJECT_NAME})
