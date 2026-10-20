find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(pcl_conversions REQUIRED)

add_library(
  translateUtility SHARED
  TranslateUtilityStandardMsgs.cpp TranslateUtilityInfrastructureMsgs.cpp
  TranslateUtilityGeometryMsgs.cpp TranslateUtilitySensorMsgs.cpp
  TranslateUtilityVisionMsgs.cpp)
target_include_directories(
  translateUtility
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include> ${pcl_conversions_INCLUDE_DIRS})
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(
  translateUtility
  PUBLIC "${cpp_typesupport_target}"
         geometry_msgs::geometry_msgs
         ${nav_msgs_TARGETS}
         standard_msgs
         infrastructure_msgs
         geometry_msgs
         sensor_msgs
         ${sensor_msgs_TARGETS}
         vision_msgs::vision_msgs
         ${vision_msgs_TARGETS}
         ${pcl_conversions_LIBRARIES}
         tf2::tf2
         tf2_geometry_msgs::tf2_geometry_msgs
         rclcpp::rclcpp)
install(
  TARGETS translateUtility
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
