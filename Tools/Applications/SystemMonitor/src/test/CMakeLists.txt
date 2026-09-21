catkin_add_gtest(test_CommonWindowUtility test_CommonWindowUtility.cpp)
target_link_libraries(test_CommonWindowUtility ${catkin_LIBRARIES}
                      ${Boost_LIBRARIES} commonWindowUtility)
add_dependencies(test_CommonWindowUtility
                 ${robot_framework_ros_EXPORTED_TARGETS})

catkin_add_gtest(test_BaseWindow test_BaseWindow.cpp)
target_link_libraries(test_BaseWindow ${catkin_LIBRARIES} ${Boost_LIBRARIES}
                      baseWindow logger)
add_dependencies(test_BaseWindow ${robot_framework_ros_EXPORTED_TARGETS})
