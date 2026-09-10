# Test: Standard Msgs
ament_add_gtest(test_translateUtilityStandardMsgs
                test_TranslateUtilityStandardMsgs.cpp)
target_include_directories(
  test_translateUtilityStandardMsgs
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(test_translateUtilityStandardMsgs translateUtility
                      "${cpp_typesupport_target}")

# Test: Infrastructure Msgs
ament_add_gtest(test_translateUtilityInfrastructureMsgs
                test_TranslateUtilityInfrastructureMsgs.cpp)
target_include_directories(
  test_translateUtilityInfrastructureMsgs
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(test_translateUtilityInfrastructureMsgs translateUtility
                      "${cpp_typesupport_target}")

# Test: Geometry Msgs
ament_add_gtest(test_translateUtilityGeometryMsgs
                test_TranslateUtilityGeometryMsgs.cpp)
target_include_directories(
  test_translateUtilityGeometryMsgs
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(test_translateUtilityGeometryMsgs translateUtility
                      "${cpp_typesupport_target}")
