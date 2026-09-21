# HeaderWindow
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

# DiagnosticWindow
add_library(diagnosticWindow SHARED DiagnosticWindow.cpp)
target_include_directories(
  diagnosticWindow
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
         $<INSTALL_INTERFACE:include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(diagnosticWindow PUBLIC baseWindow)
install(
  TARGETS diagnosticWindow
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# NodeInfoWindow
add_library(nodeInfoWindow SHARED NodeInfoWindow.cpp)
target_include_directories(
  nodeInfoWindow
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
         $<INSTALL_INTERFACE:include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(nodeInfoWindow PUBLIC baseWindow)
install(
  TARGETS nodeInfoWindow
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# StatusWindow
add_library(statusWindow SHARED StatusWindow.cpp)
target_include_directories(
  statusWindow
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
         $<INSTALL_INTERFACE:include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(statusWindow PUBLIC baseWindow)
install(
  TARGETS statusWindow
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
