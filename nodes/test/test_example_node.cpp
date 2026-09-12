#include <gtest/gtest.h>

#include <Infrastructure/Logger.hpp>

#include "../example_node.hpp"
#include "rclcpp/rclcpp.hpp"
class Ros2TestEnvironment : public ::testing::Environment {
   public:
    ~Ros2TestEnvironment() override {}
    void SetUp() override { rclcpp::init(0, nullptr); }
    void TearDown() override { rclcpp::shutdown(); }
};
testing::Environment* const ros2_env = testing::AddGlobalTestEnvironment(new Ros2TestEnvironment);
class ExampleTestFixture : public ::testing::Test {
   protected:
    void SetUp() override {
        test_node = std::make_shared<rclcpp::Node>("example_node_tester_agent");

        m_heartbeatSub = test_node->create_subscription<robot_framework_ros2::msg::Heartbeat>(
            "/heartbeat", 10, [this](const robot_framework_ros2::msg::Heartbeat::SharedPtr msg) {
                fast::rf::Logger::logNotice("Got Heartbeat!");
                m_receivedHeartbeat = true;
                m_latestHeartbeat = std::move(*msg);
            });
    }

    void TearDown() override { test_node.reset(); }

    rclcpp::Node::SharedPtr test_node;
    rclcpp::Subscription<robot_framework_ros2::msg::Heartbeat>::SharedPtr m_heartbeatSub;

    bool m_receivedHeartbeat = false;
    robot_framework_ros2::msg::Heartbeat m_latestHeartbeat;
};
TEST_F(ExampleTestFixture, VerifyHeartbeatReception) {
    // ASSERT_TRUE(false);
    auto start_time = test_node->get_clock()->now();
    double timeout_seconds = 5.0;

    while ((test_node->get_clock()->now() - start_time).seconds() < timeout_seconds) {
        rclcpp::spin_some(test_node);
        rclcpp::sleep_for(std::chrono::milliseconds(100));
    }

    ASSERT_TRUE(m_receivedHeartbeat) << "ERROR: Failed to receive a message on /heartbeat within timeout.";
    ASSERT_EQ(m_latestHeartbeat.node_state.state, robot_framework_ros2::msg::NodeState::STATE_RUNNING);
}
