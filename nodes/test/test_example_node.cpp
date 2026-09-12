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
            "/heartbeat", 10, [this](const robot_framework_ros2::msg::Heartbeat::SharedPtr) {
                fast::rf::Logger::logNotice("Got Heartbeat!");
                received_heartbeat = true;
            });
    }

    void TearDown() override { test_node.reset(); }

    rclcpp::Node::SharedPtr test_node;
    rclcpp::Subscription<robot_framework_ros2::msg::Heartbeat>::SharedPtr m_heartbeatSub;

    // Test flags
    bool received_heartbeat = false;
};
TEST_F(ExampleTestFixture, VerifyHeartbeatReception) {
    // ASSERT_TRUE(false);
    auto start_time = test_node->get_clock()->now();
    double timeout_seconds = 5.0;

    while ((test_node->get_clock()->now() - start_time).seconds() < timeout_seconds) {
        rclcpp::spin_some(test_node);
        rclcpp::sleep_for(std::chrono::milliseconds(100));
    }

    ASSERT_TRUE(received_heartbeat) << "ERROR: Failed to receive a message on /heartbeat within timeout.";
}
