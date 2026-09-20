add_library(headerWindow SHARED HeaderWindow.cpp)
target_include_directories(
  headerWindow
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
         $<INSTALL_INTERFACE:include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(headerWindow PUBLIC baseWindow)
install(
  TARGETS headerWindow
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
