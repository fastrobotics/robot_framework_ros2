# @compare_tag Node-SourceCompile v0.1`
add_executable(armed_state_manager_node ArmedStateManagerNode.cpp)
target_include_directories(
  armed_state_manager_node
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
target_link_libraries(armed_state_manager_node
                      PRIVATE baseNode basic_ArmedStateManagerProcess)
register_framework_rpath(armed_state_manager_node logger
                         basic_ArmedStateManagerProcess)
install(TARGETS armed_state_manager_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
