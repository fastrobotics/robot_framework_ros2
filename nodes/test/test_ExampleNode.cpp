#include <gtest/gtest.h>

#include <Infrastructure/Logger.hpp>

#include "../ExampleNode.hpp"
#include "rclcpp/rclcpp.hpp"
class Ros2TestEnvironment : public ::testing::Environment {
   public:
    ~Ros2TestEnvironment() override {}
    void SetUp() override { rclcpp::init(0, nullptr); }
    void TearDown() override { rclcpp::shutdown(); }
};
testing::Environment* const ros2_env = testing::AddGlobalTestEnvironment(new Ros2TestEnvironment);
std::string robotNamespace = "test";
std::string nodeNamespace = "pose/inertial_sensor/imu";
std::string nodeUnderTest = "example_node";
class ExampleTestFixture : public ::testing::Test {
   protected:
    void SetUp() override {
        test_node = std::make_shared<rclcpp::Node>("example_node_tester_agent", nodeNamespace);

        m_heartbeatSub = test_node->create_subscription<robot_framework_ros2::msg::Heartbeat>(
            "/" + robotNamespace + "/" + nodeNamespace + "/" + nodeUnderTest + "/heartbeat", 10,
            [this](const robot_framework_ros2::msg::Heartbeat::SharedPtr msg) {
                m_receivedHeartbeatRxCount++;
                m_latestHeartbeat = std::move(*msg);
            });
        m_readyToArmSub = test_node->create_subscription<robot_framework_ros2::msg::ReadyToArm>(
            "/" + robotNamespace + "/" + nodeNamespace + "/" + nodeUnderTest + "/ready_to_arm", 10,
            [this](const robot_framework_ros2::msg::ReadyToArm::SharedPtr msg) {
                m_receivedReadyToArmRxCount++;
                m_latestReadyToArm = std::move(*msg);
            });
    }

    void TearDown() override { test_node.reset(); }

    rclcpp::Node::SharedPtr test_node;
    rclcpp::Subscription<robot_framework_ros2::msg::Heartbeat>::SharedPtr m_heartbeatSub;
    rclcpp::Subscription<robot_framework_ros2::msg::ReadyToArm>::SharedPtr m_readyToArmSub;

    uint64_t m_receivedHeartbeatRxCount = 0;
    robot_framework_ros2::msg::Heartbeat m_latestHeartbeat;
    uint64_t m_receivedReadyToArmRxCount = 0;
    robot_framework_ros2::msg::ReadyToArm m_latestReadyToArm;
};
TEST_F(ExampleTestFixture, VerifyHeartbeatReception) {
    auto start_time = test_node->get_clock()->now();
    double timeout_seconds = 5.0;

    while ((test_node->get_clock()->now() - start_time).seconds() < timeout_seconds) {
        rclcpp::spin_some(test_node);
        rclcpp::sleep_for(std::chrono::milliseconds(100));
    }
    ASSERT_TRUE(m_receivedHeartbeatRxCount > 0) << "ERROR: Failed to receive a message on /heartbeat within timeout.";
    ASSERT_EQ(m_latestHeartbeat.node_state.state, robot_framework_ros2::msg::NodeState::STATE_RUNNING);

    ASSERT_TRUE(m_receivedReadyToArmRxCount > 0)
        << "ERROR: Failed to receive a message on /ready_to_arm within timeout.";
    ASSERT_EQ(m_latestReadyToArm.ready_to_arm, true) << "ERROR: Node is not able to Arm.";
}
