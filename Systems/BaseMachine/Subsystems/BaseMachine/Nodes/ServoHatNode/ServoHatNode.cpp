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
namespace fast::rf_ros2::BaseMachineSystem::BaseMachineSubsystem::HatDriver {
    bool ServoHatNode::loadConfig() {
        std::string paramExampleParameter = "example_parameter";
        this->declare_parameter<double>(paramExampleParameter, -1.0);
        double exampleParameter = this->get_parameter(paramExampleParameter).as_double();
        fast::rf::Logger::logNotice("Example Parameter: " + std::to_string(exampleParameter));
        return true;
    }
    bool ServoHatNode::initPubSubs() { return true; }
    bool ServoHatNode::initServices() { return true; }
    bool ServoHatNode::initDiagnostics() { return true; }
    bool ServoHatNode::initData() {
        m_process.update(this->get_clock()->now().seconds());  // Kick off the Process
        setReadyToArm(m_process.get_ready_to_arm());
        return true;
    }
    void ServoHatNode::run100Hz() { fast::rf::Logger::logDebug("100 Hz"); }
    void ServoHatNode::run10Hz() {
        setReadyToArm(m_process.get_ready_to_arm());
        fast::rf::Logger::logDebug("10 Hz");
    }
    void ServoHatNode::run1Hz() {
        auto diagnostics = m_process.getDiagnostics();
        setDiagnostics(diagnostics);
        fast::rf::Logger::logDebug("1 Hz");
    }
    void ServoHatNode::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void ServoHatNode::run001Hz() { fast::rf::Logger::logDebug("0.01 Hz"); }
    void ServoHatNode::runLoop1() {
        m_process.update(this->get_clock()->now().seconds());
        fast::rf::Logger::logDebug("Loop1");
    }
    void ServoHatNode::runLoop2() { fast::rf::Logger::logDebug("Loop2"); }
    void ServoHatNode::runLoop3() { fast::rf::Logger::logDebug("Loop3"); }
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
