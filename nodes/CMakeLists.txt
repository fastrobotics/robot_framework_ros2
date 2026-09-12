find_package(rclcpp REQUIRED)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_library(baseNode SHARED base_node.cpp)
target_include_directories(
  baseNode PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                  $<INSTALL_INTERFACE:include>)

target_link_libraries(baseNode PUBLIC logger rclcpp::rclcpp
                                      ${PROJECT_NAME}__rosidl_typesupport_cpp)
install(
  TARGETS baseNode
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

add_executable(example_node example_node.cpp)
target_link_libraries(example_node PUBLIC baseNode)
register_framework_rpath(example_node logger)
install(TARGETS example_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
