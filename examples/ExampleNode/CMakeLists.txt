add_executable(example_node ExampleNode.cpp)
target_include_directories(
  example_node
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
target_link_libraries(example_node
                      PRIVATE baseNode basic_ExampleProcess)
register_framework_rpath(example_node logger
                         basic_ExampleProcess)
install(TARGETS example_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
