/**
 * @file test_LocalPoseFuserNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Test v0.1
 *
 */
#include <gtest/gtest.h>

#include <Infrastructure/Logger.hpp>

#include "../LocalPoseFuserNode.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"

using namespace std::chrono_literals;

class Ros2TestEnvironment : public ::testing::Environment {
   public:
    ~Ros2TestEnvironment() override {}
    void SetUp() override { rclcpp::init(0, nullptr); }
    void TearDown() override { rclcpp::shutdown(); }
};
testing::Environment* const ros2_env = testing::AddGlobalTestEnvironment(new Ros2TestEnvironment);
std::string robotNamespace = "test";
std::string nodeNamespace = "pose/localpose/localposefuser";
std::string nodeUnderTest = "local_pose_fuser_node";
class LocalPoseFuserNodeTestFixture : public ::testing::Test {
   protected:
    void SetUp() override {
        test_node = std::make_shared<rclcpp::Node>("local_pose_fuser_node_tester_agent", nodeNamespace);

        m_heartbeatSub = test_node->create_subscription<robot_framework_ros2::msg::Heartbeat>(
            "/" + robotNamespace + "/" + nodeNamespace + "/" + nodeUnderTest + "/heartbeat", 10,
            [this](const robot_framework_ros2::msg::Heartbeat::SharedPtr msg) {
                m_receivedHeartbeatRxCount++;
                m_latestHeartbeat = std::move(*msg);
            });
        m_diagnosticSub = test_node->create_subscription<robot_framework_ros2::msg::Diagnostic>(
            "/" + robotNamespace + "/" + nodeNamespace + "/" + nodeUnderTest + "/diagnostic", 10,
            [this](const robot_framework_ros2::msg::Diagnostic::SharedPtr msg) {
                m_receivedDiagnosticRxCount++;
                m_latestDiagnostic = std::move(*msg);
            });
        m_readyToArmSub = test_node->create_subscription<robot_framework_ros2::msg::ReadyToArm>(
            "/" + robotNamespace + "/" + nodeNamespace + "/" + nodeUnderTest + "/ready_to_arm", 10,
            [this](const robot_framework_ros2::msg::ReadyToArm::SharedPtr msg) {
                m_receivedReadyToArmRxCount++;
                m_latestReadyToArm = std::move(*msg);
            });

        m_localPoseSub = test_node->create_subscription<nav_msgs::msg::Odometry>(
            "/" + robotNamespace + "/local_pose", 10, [this](const nav_msgs::msg::Odometry::SharedPtr msg) {
                m_receivedLocalPoseRxCount++;
                m_latestLocalPose = std::move(*msg);
            });

        m_machineInertialPub =
            test_node->create_publisher<sensor_msgs::msg::Imu>("/" + robotNamespace + "/machine_imu", 10);
    }

    void TearDown() override { test_node.reset(); }

    rclcpp::Node::SharedPtr test_node;
    rclcpp::Subscription<robot_framework_ros2::msg::Heartbeat>::SharedPtr m_heartbeatSub;
    rclcpp::Subscription<robot_framework_ros2::msg::Diagnostic>::SharedPtr m_diagnosticSub;
    rclcpp::Subscription<robot_framework_ros2::msg::ReadyToArm>::SharedPtr m_readyToArmSub;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr m_localPoseSub;
    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr m_machineInertialPub;

    uint64_t m_receivedHeartbeatRxCount = 0;
    robot_framework_ros2::msg::Heartbeat m_latestHeartbeat;
    uint64_t m_receivedDiagnosticRxCount = 0;
    robot_framework_ros2::msg::Diagnostic m_latestDiagnostic;
    uint64_t m_receivedReadyToArmRxCount = 0;
    robot_framework_ros2::msg::ReadyToArm m_latestReadyToArm;
    uint64_t m_receivedLocalPoseRxCount = 0;
    nav_msgs::msg::Odometry m_latestLocalPose;
};
TEST_F(LocalPoseFuserNodeTestFixture, VerifyHeartbeatReception) {
    auto start_time = test_node->get_clock()->now();
    double timeout_seconds = 5.0;

    while ((test_node->get_clock()->now() - start_time).seconds() < timeout_seconds) {
        rclcpp::spin_some(test_node);
        rclcpp::sleep_for(std::chrono::milliseconds(100));
    }
    ASSERT_TRUE(m_receivedHeartbeatRxCount > 0) << "ERROR: Failed to receive a message on /heartbeat within timeout.";
    ASSERT_EQ(m_latestHeartbeat.node_state.state, robot_framework_ros2::msg::NodeState::STATE_RUNNING);

    ASSERT_TRUE(m_receivedDiagnosticRxCount > 0) << "ERROR: Failed to receive a message on /diagnostic within timeout.";

    ASSERT_TRUE(m_receivedReadyToArmRxCount > 0)
        << "ERROR: Failed to receive a message on /ready_to_arm within timeout.";
    ASSERT_GT(m_latestReadyToArm.system_id, 0);
    ASSERT_GT(m_latestReadyToArm.subsystem_id, 0);
    ASSERT_GT(m_latestReadyToArm.process_id, 0);
    ASSERT_EQ(m_latestReadyToArm.ready_to_arm, true) << "ERROR: Node is not able to Arm.";
}

TEST_F(LocalPoseFuserNodeTestFixture, VerifyLocalPoseReception) {
    sensor_msgs::msg::Imu imu_msg;
    imu_msg.header.stamp = test_node->get_clock()->now();
    imu_msg.header.frame_id = "imu_input";
    imu_msg.orientation_covariance[0] = -1.0;
    imu_msg.angular_velocity.x = 0.5;
    imu_msg.angular_velocity.y = 1.5;
    imu_msg.angular_velocity.z = -0.25;
    imu_msg.linear_acceleration.x = 1.0;
    imu_msg.linear_acceleration.y = 2.0;
    imu_msg.linear_acceleration.z = 3.0;

    const auto start_time = test_node->get_clock()->now();
    const double timeout_seconds = 5.0;

    while ((test_node->get_clock()->now() - start_time).seconds() < timeout_seconds) {
        rclcpp::spin_some(test_node);
        m_machineInertialPub->publish(imu_msg);
        rclcpp::sleep_for(100ms);

        if (m_receivedLocalPoseRxCount > 0) {
            break;
        }
    }

    ASSERT_GT(m_receivedLocalPoseRxCount, 0u) << "ERROR: Failed to receive Local Pose output within timeout.";
    EXPECT_EQ(m_latestLocalPose.header.frame_id, imu_msg.header.frame_id);
}
