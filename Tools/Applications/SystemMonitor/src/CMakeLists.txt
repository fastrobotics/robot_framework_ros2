add_library(commonWindowUtility SHARED CommonWindowUtility.cpp)
target_include_directories(
  commonWindowUtility
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
         $<INSTALL_INTERFACE:include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(
  commonWindowUtility PUBLIC "${cpp_typesupport_target}" rclcpp::rclcpp
                             infrastructure_msgs ${CURSES_LIBRARIES})
ament_target_dependencies(commonWindowUtility PUBLIC rclcpp geometry_msgs)
install(
  TARGETS commonWindowUtility
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

add_library(baseWindow SHARED BaseWindow.cpp)
target_include_directories(
  baseWindow PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
                    $<INSTALL_INTERFACE:include>)
target_link_libraries(
  baseWindow
  PUBLIC "${cpp_typesupport_target}"
         rclcpp::rclcpp
         infrastructure_msgs
         ${CURSES_LIBRARIES}
         coreUtility
         translateUtility
         commonWindowUtility
         logger
         ${geometry_msgs_TARGETS}
         ${nav_msgs_TARGETS}
         ${sensor_msgs_TARGETS})
install(
  TARGETS baseWindow
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
add_subdirectory(Windows)
