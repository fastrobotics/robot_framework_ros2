/**
 * @file ExampleNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.2
 *
 */
#include "ExampleNode.hpp"
namespace fast::rf_ros2 {
    bool ExampleNode::loadConfig() {
        std::string paramExampleParameter = "example_parameter";
        this->declare_parameter<double>(paramExampleParameter, -1.0);
        double exampleParameter = this->get_parameter(paramExampleParameter).as_double();
        fast::rf::Logger::logNotice("Example Parameter: " + std::to_string(exampleParameter));
        return true;
    }
    bool ExampleNode::initPubSubs() { return true; }
    bool ExampleNode::initServices() { return true; }
    bool ExampleNode::initDiagnostics() { return true; }
    bool ExampleNode::initData() {
        m_process.update(this->get_clock()->now().seconds());  // Kick off the Process
        setReadyToArm(m_process.get_ready_to_arm());
        return true;
    }
    void ExampleNode::run100Hz() { fast::rf::Logger::logDebug("100 Hz"); }
    void ExampleNode::run10Hz() {
        setReadyToArm(process.get_ready_to_arm());
        fast::rf::Logger::logDebug("10 Hz");
    }
    void ExampleNode::run1Hz() {
        auto diagnostics = m_process.getDiagnostics();
        setDiagnostics(diagnostics);
        fast::rf::Logger::logDebug("1 Hz");
    }
    void ExampleNode::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void ExampleNode::run001Hz() { fast::rf::Logger::logDebug("0.01 Hz"); }
    void ExampleNode::runLoop1() {
        process.update(this->get_clock()->now().seconds());
        fast::rf::Logger::logDebug("Loop1");
    }
    void ExampleNode::runLoop2() { fast::rf::Logger::logDebug("Loop2"); }
    void ExampleNode::runLoop3() { fast::rf::Logger::logDebug("Loop3"); }
    std::string ExampleNode::pretty() {
        std::string str = "\n--- ExampleNode ---\n";
        str += BaseNode::pretty() + "\n";
        str += m_process.pretty();
        return str;
    }

    std::shared_ptr<BaseNode> BaseNode::createNode() { return std::make_shared<ExampleNode>(); }
}  // namespace fast::rf_ros2
