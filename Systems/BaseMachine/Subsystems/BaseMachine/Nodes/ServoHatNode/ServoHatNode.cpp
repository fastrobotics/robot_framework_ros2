/**
 * @file ServoHatNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.3
 *
 */
#include "ServoHatNode.hpp"

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2::BaseMachineSystem::BaseMachineSubsystem::HatDriver {
    void ServoHatNode::robotArmCommandCallback(const robot_framework_ros2::msg::ArmCommand::SharedPtr msg) {
        robot_framework_ros2::msg::ArmCommand localMsg = *msg;
        m_process.update_RobotArmCommand(fast::rf_ros2::utils::TranslateUtility::convert(localMsg));
    }
    void ServoHatNode::driveCallback(uint16_t channel, const std_msgs::msg::Float64::SharedPtr msg) {
        uint16_t value = (uint16_t)msg->data;
        bool status = m_process.setServoValue(channel, value);
        if (status == false) {
            fast::rf::Logger::logError("Unable to update Channel: " + std::to_string(channel) +
                                       " with Value: " + std::to_string(value));
        }
    }
    bool ServoHatNode::loadConfig() { return true; }
    bool ServoHatNode::initPubSubs() {
        const auto armCommandTopic = this->declare_parameter<std::string>("topic_arm_command");
        m_armCommandSub = this->create_subscription<robot_framework_ros2::msg::ArmCommand>(
            getNamespacedTopic(armCommandTopic), 10,
            [this](const robot_framework_ros2::msg::ArmCommand::SharedPtr msg) { this->robotArmCommandCallback(msg); });
        const auto leftDriveTopic = this->declare_parameter<std::string>("topic_left_drive");
        m_leftDriveSub = this->create_subscription<std_msgs::msg::Float64>(
            getNamespacedTopic(leftDriveTopic), 10,
            [this, leftDriveTopic](const std_msgs::msg::Float64::SharedPtr msg) { this->driveCallback(0, msg); });

        const auto rightDriveTopic = this->declare_parameter<std::string>("topic_right_drive");
        m_rightDriveSub = this->create_subscription<std_msgs::msg::Float64>(
            getNamespacedTopic(rightDriveTopic), 10,
            [this, rightDriveTopic](const std_msgs::msg::Float64::SharedPtr msg) { this->driveCallback(1, msg); });

        return true;
    }
    bool ServoHatNode::initServices() { return true; }
    bool ServoHatNode::initDiagnostics() { return true; }
    bool ServoHatNode::initData() {
        bool status = m_process.init();
        if (status == false) {
            fast::rf::Logger::logError("Unable to initialize Process!");
            return false;
        }
        m_process.update(this->get_clock()->now().seconds());  // Kick off the Process
        setReadyToArm(m_process.get_ready_to_arm());
        return true;
    }
    void ServoHatNode::run100Hz() {}
    void ServoHatNode::run10Hz() { setReadyToArm(m_process.get_ready_to_arm()); }
    void ServoHatNode::run1Hz() {
        auto diagnostics = m_process.getDiagnostics();
        setDiagnostics(diagnostics);
    }
    void ServoHatNode::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void ServoHatNode::run001Hz() {}
    void ServoHatNode::runLoop1() { m_process.update(this->get_clock()->now().seconds()); }
    void ServoHatNode::runLoop2() {}
    void ServoHatNode::runLoop3() {}
    std::string ServoHatNode::pretty() {
        std::string str = "\n--- ServoHatNode ---\n";
        str += BaseNode::pretty() + "\n";
        str += m_process.pretty();
        return str;
    }
}  // namespace fast::rf_ros2::BaseMachineSystem::BaseMachineSubsystem::HatDriver
namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<fast::rf_ros2::BaseMachineSystem::BaseMachineSubsystem::HatDriver::ServoHatNode>();
    }
}  // namespace fast::rf_ros2
