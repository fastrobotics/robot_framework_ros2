# core/utils/TranslateUtility/test/CMakeLists.txt

# 1. Register the Google Test executable target locally
ament_add_gtest(test_translate_utility
                test_TranslateUtilityInfrastructureMsgs.cpp)

# 1. Map your root include folder path
target_include_directories(
  test_translate_utility
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

# 1. Fetch the message type support
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")

# 1. THE FIX: Remove "PUBLIC" so these match the plain signature used by
#   ament_add_gtest
target_link_libraries(test_translate_utility translateUtility
                      "${cpp_typesupport_target}")
