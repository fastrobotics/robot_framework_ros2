find_package(rclcpp REQUIRED)
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
add_library(baseNode SHARED BaseNode.cpp)
target_include_directories(
  baseNode
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include> # Absolute path from
                                                          # repo root
         $<INSTALL_INTERFACE:include>)

target_link_libraries(baseNode PUBLIC logger rclcpp::rclcpp
                                      ${PROJECT_NAME}__rosidl_typesupport_cpp)
install(
  TARGETS baseNode
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
