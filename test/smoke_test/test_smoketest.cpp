#include <gtest/gtest.h>

#include <Infrastructure/Logger.hpp>
#include <chrono>
#include <rclcpp/rclcpp.hpp>
#include <string>
#include <vector>

#include "robot_framework_ros2/msg/diagnostic.hpp"
#include "robot_framework_ros2/msg/heartbeat.hpp"
#include "robot_framework_ros2/msg/ready_to_arm.hpp"

using namespace std::chrono_literals;
struct NodeSignals {
    uint64_t heartbeatRxCount = 0;
    uint64_t diagnostiRxCount = 0;
    uint64_t readyToArmRxCount = 0;
};

class MultiNodeIntegrationFixture : public ::testing::Test {
   protected:
    static void SetUpTestSuite() { rclcpp::init(0, nullptr); }
    static void TearDownTestSuite() { rclcpp::shutdown(); }

    void SetUp() override {
        node = std::make_shared<rclcpp::Node>("smoketest_signal_checker");

        // Define your target fully qualified node names/paths
        targetNodes = {"/test/basemachine/basemachine/hatdriver/servo_hat_node",
                       "/test/pose/inertial_sensor/imu/imu_node1",
                       "/test/pose/localpose/inertialsensorfuser/inertial_sensor_fuser_node",
                       "/test/pose/localpose/localposefuser/local_pose_fuser_node",
                       "/test/safety/modemanager/armedstatemanager/armed_state_manager_node"};

        // Initialize tracking map and dynamically create subscriptions
        for (const auto& nodePath : targetNodes) {
            // Instantiate entry in tracker map
            signalTracker[nodePath] = NodeSignals{};

            std::string heartbeatTopic = nodePath + "/heartbeat";
            heartbeatSubs.push_back(node->create_subscription<robot_framework_ros2::msg::Heartbeat>(
                heartbeatTopic, 10, [this, nodePath](const robot_framework_ros2::msg::Heartbeat::SharedPtr msg) {
                    (void)msg;
                    signalTracker[nodePath].heartbeatRxCount++;
                }));

            std::string diagnosticTopic = nodePath + "/diagnostic";
            diagnosticSubs.push_back(node->create_subscription<robot_framework_ros2::msg::Diagnostic>(
                diagnosticTopic, 10, [this, nodePath](const robot_framework_ros2::msg::Diagnostic::SharedPtr msg) {
                    (void)msg;
                    signalTracker[nodePath].diagnostiRxCount++;
                }));

            std::string readyToArmTopic = nodePath + "/ready_to_arm";
            readyToArmSubs.push_back(node->create_subscription<robot_framework_ros2::msg::ReadyToArm>(
                readyToArmTopic, 10, [this, nodePath](const robot_framework_ros2::msg::ReadyToArm::SharedPtr msg) {
                    (void)msg;
                    signalTracker[nodePath].readyToArmRxCount++;
                }));
        }
    }

    // Helper function to check if every expected signal has updated to true
    bool all_signals_received() {
        for (const auto& nodePath : targetNodes) {
            const auto& signals = signalTracker[nodePath];
            if ((signals.heartbeatRxCount == 0) || (signals.diagnostiRxCount == 0) ||
                (signals.readyToArmRxCount == 0)) {
                return false;
            }
        }
        return true;
    }

    rclcpp::Node::SharedPtr node;
    std::vector<std::string> targetNodes;
    std::map<std::string, NodeSignals> signalTracker;

    // Retain subscription shared pointers so they don't fall out of scope
    std::vector<rclcpp::Subscription<robot_framework_ros2::msg::Heartbeat>::SharedPtr> heartbeatSubs;
    std::vector<rclcpp::Subscription<robot_framework_ros2::msg::Diagnostic>::SharedPtr> diagnosticSubs;
    std::vector<rclcpp::Subscription<robot_framework_ros2::msg::ReadyToArm>::SharedPtr> readyToArmSubs;
};
TEST_F(MultiNodeIntegrationFixture, TestAllSystemDataReceived) {
    // Spin the node loop for up to 5 seconds waiting for all topics to report data
    auto start_time = node->get_clock()->now();
    auto timeout = 5s;

    while ((node->get_clock()->now() - start_time) < timeout) {
        rclcpp::spin_some(node);
        if (all_signals_received()) {
            break;
        }
    }

    // Run explicit verification and generate precise error streams per node failure
    for (const auto& nodePath : targetNodes) {
        const auto& signals = signalTracker[nodePath];

        EXPECT_GT(signals.heartbeatRxCount, 0) << "Missing [heartbeat] data stream from target: " << nodePath;
        EXPECT_GT(signals.diagnostiRxCount, 0) << "Missing [diagnostic] data stream from target: " << nodePath;
        EXPECT_GT(signals.readyToArmRxCount, 0) << "Missing [ready_to_arm] data stream from target: " << nodePath;
    }
}
