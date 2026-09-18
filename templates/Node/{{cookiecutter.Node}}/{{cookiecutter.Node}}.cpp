/**
 * @file {{cookiecutter.Node}}.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2026-09-13
 * 
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.3
 * 
 */
#include "{{cookiecutter.Node}}.hpp"
namespace fast::rf_ros2::{{cookiecutter.System}}System::{{cookiecutter.Subsystem}}Subsystem::{{cookiecutter.Process}} {
    bool {{cookiecutter.Node}}::loadConfig() {
        std::string paramExampleParameter = "example_parameter";
        this->declare_parameter<double>(paramExampleParameter, -1.0);
        double exampleParameter = this->get_parameter(paramExampleParameter).as_double();
        fast::rf::Logger::logNotice("Example Parameter: " + std::to_string(exampleParameter));
        return true;
    }
    bool {{cookiecutter.Node}}::initPubSubs() { return true; }
    bool {{cookiecutter.Node}}::initServices() { return true; }
    bool {{cookiecutter.Node}}::initDiagnostics() { return true; }
    bool {{cookiecutter.Node}}::initData() {
        m_process.update(this->get_clock()->now().seconds());  // Kick off the Process
        setReadyToArm(m_process.get_ready_to_arm());
        return true;
    }
    void {{cookiecutter.Node}}::run100Hz() { fast::rf::Logger::logDebug("100 Hz"); }
    void {{cookiecutter.Node}}::run10Hz() {setReadyToArm(m_process.get_ready_to_arm());fast::rf::Logger::logDebug("10 Hz");}
    void {{cookiecutter.Node}}::run1Hz() {
        auto diagnostics = m_process.getDiagnostics();setDiagnostics(diagnostics);
        fast::rf::Logger::logDebug("1 Hz");
    }
    void {{cookiecutter.Node}}::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void {{cookiecutter.Node}}::run001Hz() { fast::rf::Logger::logDebug("0.01 Hz"); }
    void {{cookiecutter.Node}}::runLoop1() {
        m_process.update(this->get_clock()->now().seconds());
        fast::rf::Logger::logDebug("Loop1");
    }
    void {{cookiecutter.Node}}::runLoop2() { fast::rf::Logger::logDebug("Loop2"); }
    void {{cookiecutter.Node}}::runLoop3() { fast::rf::Logger::logDebug("Loop3"); }
    std::string {{cookiecutter.Node}}::pretty() {
        std::string str = "\n--- {{cookiecutter.Node}} ---\n";
        str += BaseNode::pretty() + "\n";
        str += m_process.pretty();return str;
    }

    std::shared_ptr<BaseNode> BaseNode::createNode() { return std::make_shared<{{cookiecutter.Node}}>(); }
}  // namespace fast::rf_ros2::{{cookiecutter.System}}System::{{cookiecutter.Subsystem}}Subsystem::{{cookiecutter.Process}}
namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<fast::rf_ros2::{{cookiecutter.System}}System::{{cookiecutter.Subsystem}}Subsystem::{{cookiecutter.Process}}::{{cookiecutter.Node}}>();
    }
}  // namespace fast::rf_ros2
