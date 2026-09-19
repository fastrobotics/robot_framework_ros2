# @compare_tag Node-SourceCompile v0.1`
add_executable(servo_hat_node ServoHatNode.cpp)
target_include_directories(
  servo_hat_node
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
target_link_libraries(servo_hat_node PRIVATE baseNode basic_HatDriverProcess)
register_framework_rpath(servo_hat_node logger basic_HatDriverProcess)
install(TARGETS servo_hat_node DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
