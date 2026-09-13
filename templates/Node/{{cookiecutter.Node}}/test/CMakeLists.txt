# @compare_tag Node-SourceTestCompile v0.1`
find_package(ament_cmake_ros REQUIRED)
find_package(launch_testing_ament_cmake REQUIRED)
find_package(ament_cmake_gtest REQUIRED)

# 1. Compile the C++ Tester Agent binary
add_executable(test_{{cookiecutter.NodeBinary}} test_{{cookiecutter.Node}}.cpp)
target_link_libraries(
  test_{{cookiecutter.NodeBinary}} gtest gtest_main rclcpp::rclcpp logger
  ${PROJECT_NAME}__rosidl_typesupport_cpp)
target_include_directories(
  test_{{cookiecutter.NodeBinary}}
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
register_framework_rpath(test_{{cookiecutter.NodeBinary}} logger)

install(TARGETS test_{{cookiecutter.NodeBinary}}
        DESTINATION lib/${PROJECT_NAME})
add_launch_test(run_test_{{cookiecutter.Node}}.py)
