#!/usr/bin/env python3
import pytest
import unittest
import launch_testing.actions
import launch_testing.asserts
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch_xml.launch_description_sources import XMLLaunchDescriptionSource

@pytest.mark.launch_test
def generate_test_description():
    package_share = FindPackageShare('robot_framework_ros2')
    production_node_xml = IncludeLaunchDescription(
        XMLLaunchDescriptionSource([
            package_share, '/launch/example_node.launch.xml'
        ]),
        launch_arguments={
            'robot_namespace': '/test/'
        }.items()
    )
    tester_agent_node = Node(
        package='robot_framework_ros2',
        executable='test_example_node',
        name='example_tester_agent',
        output='screen',
        emulate_tty=True
    )
    return LaunchDescription([
        production_node_xml,
        tester_agent_node,
        launch_testing.actions.ReadyToTest()
    ]), {'tester_agent': tester_agent_node}
class TestXMLHarness(unittest.TestCase):
    def test_tester_agent_success(self, proc_info, tester_agent):
        proc_info.assertWaitForShutdown(process=tester_agent, timeout=10.0)

        launch_testing.asserts.assertExitCodes(
            proc_info, 
            process=tester_agent
        )
