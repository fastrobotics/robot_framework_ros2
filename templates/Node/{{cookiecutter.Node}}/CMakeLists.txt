# @compare_tag Node-SourceCompile v0.1`
add_executable({{cookiecutter.NodeBinary}} {{cookiecutter.Node}}.cpp)
target_include_directories(
  {{cookiecutter.NodeBinary}}
  PRIVATE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
          $<BUILD_INTERFACE:${robot_framework_SOURCE_DIR}/include>)
target_link_libraries({{cookiecutter.NodeBinary}}
                      PRIVATE baseNode basic_{{cookiecutter.Process}}Process)
register_framework_rpath({{cookiecutter.NodeBinary}} logger
                         basic_{{cookiecutter.Process}}Process)
install(TARGETS {{cookiecutter.NodeBinary}} DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch config DESTINATION share/${PROJECT_NAME}/)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
