catkin_add_gtest(test_HeaderWindow test_HeaderWindow.cpp)
target_link_libraries(test_HeaderWindow ${catkin_LIBRARIES} ${Boost_LIBRARIES}
                      headerWindow)
add_dependencies(test_HeaderWindow ${robot_framework_ros_EXPORTED_TARGETS})

catkin_add_gtest(test_StatusWindow test_StatusWindow.cpp)
target_link_libraries(test_StatusWindow ${catkin_LIBRARIES} ${Boost_LIBRARIES}
                      statusWindow)
add_dependencies(test_StatusWindow ${robot_framework_ros_EXPORTED_TARGETS})

catkin_add_gtest(test_NodeInfoWindow test_NodeInfoWindow.cpp)
target_link_libraries(test_NodeInfoWindow ${catkin_LIBRARIES}
                      ${Boost_LIBRARIES} nodeInfoWindow)
add_dependencies(test_NodeInfoWindow ${robot_framework_ros_EXPORTED_TARGETS})

catkin_add_gtest(test_DiagnosticWindow test_DiagnosticWindow.cpp)
target_link_libraries(test_DiagnosticWindow ${catkin_LIBRARIES}
                      ${Boost_LIBRARIES} diagnosticWindow)
add_dependencies(test_DiagnosticWindow ${robot_framework_ros_EXPORTED_TARGETS})
