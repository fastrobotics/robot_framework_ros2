import os
import unittest
import pytest
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import launch_testing
import launch_testing.actions

@pytest.mark.launch_test
def generate_test_description():
    # 1. Locate your main orchestrator launch file
    my_pkg_dir = get_package_share_directory('robot_framework_ros2')
    orchestrator_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(my_pkg_dir, 'launch', 'orchestrator.launch.py')
        ),
        launch_arguments={
            'robot_namespace': 'test'
        }.items()
    )

    # 2. Define the C++ Test Node Executable
    cpp_test_node = Node(
        package='robot_framework_ros2',
        executable='test_smoketest',  # Match CMake target name
        output='screen'
    )

    return LaunchDescription([
        orchestrator_launch,
        cpp_test_node,
        launch_testing.actions.ReadyToTest(),
    ]), {
        'cpp_test_node': cpp_test_node
    }

# test/smoke_test/test_smoketest_orchestrator.launch.py

class TestResultChecker(unittest.TestCase):
    def test_cpp_node_exit_code(self, proc_info, cpp_test_node):
        # 1. Wait up to 10 seconds for the C++ GTest binary to complete its checks
        proc_info.assertWaitForShutdown(process=cpp_test_node, timeout=10.0)
        
        # 2. Assert that it completed successfully (exited with code 0)
        launch_testing.asserts.assertExitCodes(
            proc_info,
            allowable_exit_codes=[0],
            process=cpp_test_node
        )

