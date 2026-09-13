# Test: Core Utility
ament_add_gtest(test_coreUtility test_CoreUtility.cpp)
target_include_directories(
  test_coreUtility PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(test_coreUtility coreUtility "${cpp_typesupport_target}")
