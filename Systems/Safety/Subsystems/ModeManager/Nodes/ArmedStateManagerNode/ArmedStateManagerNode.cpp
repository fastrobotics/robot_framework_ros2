/**
 * @file ArmedStateManagerNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.3
 *
 */
#include "ArmedStateManagerNode.hpp"

#include <robot_framework_ros2/utils/TranslateUtility.hpp>
namespace fast::rf_ros2::SafetySystem::ModeManagerSubsystem::ArmedStateManager {
    void ArmedStateManagerNode::readyToArmCallback(const robot_framework_ros2::msg::ReadyToArm::SharedPtr msg) {
        robot_framework_ros2::msg::ReadyToArm localMsg = *msg;
        if (m_process.new_ReadyToArmStatus(fast::rf_ros2::utils::TranslateUtility::convert(localMsg)) == false) {
            fast::rf::Logger::logError("Node: " + localMsg.nodename + " Unable to process Ready To Arm Msg");
        }
    }
    void ArmedStateManagerNode::armStateChangeService(
        const std::shared_ptr<robot_framework_ros2::srv::ArmStateChange::Request> request,
        std::shared_ptr<robot_framework_ros2::srv::ArmStateChange::Response> response) {
        robot_framework_ros2::srv::ArmStateChange::Request localRequest = *request;
        auto req = fast::rf_ros2::utils::TranslateUtility::convert(localRequest);
        auto resp = m_process.request_armstate_change(req);
        robot_framework_ros2::srv::ArmStateChange::Response localResponse =
            fast::rf_ros2::utils::TranslateUtility::convert(resp);
        *response = localResponse;
        fast::rf::Logger::logWarn("Got Service Request!");
    }
    bool ArmedStateManagerNode::loadConfig() {
        std::string nodeMonitorListParam = "nodes_to_monitor";
        this->declare_parameter<std::vector<std::string> >(nodeMonitorListParam);
        m_nodesToMonitor = this->get_parameter(nodeMonitorListParam).as_string_array();
        return true;
    }
    bool ArmedStateManagerNode::initPubSubs() {
        for (auto nodeTopic : m_nodesToMonitor) {
            std::string monitorTopic = getNamespacedTopic(nodeTopic) + "/ready_to_arm";
            auto sub = this->create_subscription<robot_framework_ros2::msg::ReadyToArm>(
                monitorTopic, 10,
                [this](const robot_framework_ros2::msg::ReadyToArm::SharedPtr msg) { this->readyToArmCallback(msg); });
            m_readyToArmSubs.push_back(sub);
        }
        const auto armCommandTopic = this->declare_parameter<std::string>("topic_arm_command");
        m_armCommandPub =
            this->create_publisher<robot_framework_ros2::msg::ArmCommand>(getNamespacedTopic(armCommandTopic), 10);
        return true;
    }
    bool ArmedStateManagerNode::initServices() {
        m_armStateChangeSrvServer = this->create_service<robot_framework_ros2::srv::ArmStateChange>(
            getRobotNamespace() + "/ready_to_arm", std::bind(&ArmedStateManagerNode::armStateChangeService, this,
                                                             std::placeholders::_1, std::placeholders::_2));
        return true;
    }
    bool ArmedStateManagerNode::initDiagnostics() { return true; }
    bool ArmedStateManagerNode::initData() {
        fast::rf::SafetySystem::ModeManagerSubsystem::ArmedStateManager::ArmedStateManagerProcessConfig config;
        config.expected_arm_signals = (uint8_t)m_nodesToMonitor.size();
        if (m_process.set_config(config) == false) {
            fast::rf::Logger::logError("Config not Valid! " + config.pretty());
            return false;
        }

        m_process.update(this->get_clock()->now().seconds());  // Kick off the Process
        setReadyToArm(m_process.get_ready_to_arm());
        return true;
    }
    void ArmedStateManagerNode::run100Hz() {}
    void ArmedStateManagerNode::run10Hz() { setReadyToArm(m_process.get_ready_to_arm()); }
    void ArmedStateManagerNode::run1Hz() {
        auto diagnostics = m_process.getDiagnostics();
        setDiagnostics(diagnostics);
    }
    void ArmedStateManagerNode::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void ArmedStateManagerNode::run001Hz() {}
    void ArmedStateManagerNode::runLoop1() { m_process.update(this->get_clock()->now().seconds()); }
    void ArmedStateManagerNode::runLoop2() {
        m_armCommandPub->publish(fast::rf_ros2::utils::TranslateUtility::convert(m_process.get_ArmCommandMsg()));
    }
    void ArmedStateManagerNode::runLoop3() {}
    std::string ArmedStateManagerNode::pretty() {
        std::string str = "\n--- ArmedStateManagerNode ---\n";
        str += BaseNode::pretty() + "\n";
        str += m_process.pretty();
        return str;
    }
}  // namespace fast::rf_ros2::SafetySystem::ModeManagerSubsystem::ArmedStateManager
namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<
            fast::rf_ros2::SafetySystem::ModeManagerSubsystem::ArmedStateManager::ArmedStateManagerNode>();
    }
}  // namespace fast::rf_ros2
