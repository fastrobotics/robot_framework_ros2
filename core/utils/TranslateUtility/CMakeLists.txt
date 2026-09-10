add_library(translateUtility SHARED TranslateUtilityInfrastructureMsgs.cpp)
target_include_directories(
  translateUtility PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                          $<INSTALL_INTERFACE:include>)
rosidl_get_typesupport_target(cpp_typesupport_target "${PROJECT_NAME}"
                              "rosidl_typesupport_cpp")
target_link_libraries(translateUtility PUBLIC "${cpp_typesupport_target}"
                                              infrastructure_msgs)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
