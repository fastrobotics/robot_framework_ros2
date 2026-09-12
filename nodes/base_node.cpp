#include "base_node.hpp"

#include <unistd.h>

#include <chrono>
#include <functional>
using namespace std::chrono_literals;
namespace fast::rf_ros2 {
    BaseNode::BaseNode(const std::string& nodeName)
        : Node(nodeName,
               rclcpp::NodeOptions().allow_undeclared_parameters(true).automatically_declare_parameters_from_overrides(
                   false)) {
        m_baseNodeName = nodeName;
        m_nodeState.state = robot_framework_ros2::msg::NodeState::STATE_UNKNOWN;
    }
    bool BaseNode::baseLoadConfig() {
        std::string verbosityLevel = this->declare_parameter<std::string>("verbosity_level", "NOTICE");
        fast::rf::Level level;
        if (verbosityLevel == "DEBUG") {
            level = fast::rf::Level::DEBUG;
        } else if (verbosityLevel == "INFO") {
            level = fast::rf::Level::INFO;
        } else if (verbosityLevel == "NOTICE") {
            level = fast::rf::Level::NOTICE;
        } else if (verbosityLevel == "WARN") {
            level = fast::rf::Level::WARN;
        } else if (verbosityLevel == "ERROR") {
            level = fast::rf::Level::ERROR;
        } else if (verbosityLevel == "FATAL") {
            level = fast::rf::Level::FATAL;
        } else {
            fast::rf::Logger::logError("Param: verbosity_level: " + verbosityLevel + " Invalid!");
            return false;
        }

        if (!fast::rf::Logger::init(level, this->get_name())) {
            fast::rf::Logger::logError("Unable to initialize Logger!");
            return false;
        }
        return true;
    }
    bool BaseNode::baseInitPubSubs() {
        m_heartbeatPub = this->create_publisher<robot_framework_ros2::msg::Heartbeat>("heartbeat", 10);
        m_readyToArmPub = this->create_publisher<robot_framework_ros2::msg::ReadyToArm>("ready_to_arm", 10);
        return true;
    }
    bool BaseNode::baseInitServices() { return true; }
    bool BaseNode::baseInitDiagnostics() { return true; }
    bool BaseNode::baseInitData() {
        m_heartbeat.hostname = getHostName();
        m_heartbeat.base_nodename = m_baseNodeName;
        m_heartbeat.nodename = this->get_name();
        m_readyToArm.nodename = this->get_name();
        return true;
    }
    bool BaseNode::init() {
        if (!requestNodeStateChange(robot_framework_ros2::msg::NodeState::STATE_INITIALIZING, false)) {
            return false;
        }

        if (!baseLoadConfig()) {
            fast::rf::Logger::logError("Unable to load Base Config! Exiting.");
            return false;
        }
        if (!baseInitPubSubs()) {
            fast::rf::Logger::logError("Unable to init Base Pubs and Subs! Exiting.");
            return false;
        }
        if (!baseInitServices()) {
            fast::rf::Logger::logError("Unable to init Base Services! Exiting.");
            return false;
        }
        if (!baseInitDiagnostics()) {
            fast::rf::Logger::logError("Unable to init Base Diagnostics! Exiting.");
            return false;
        }
        if (!baseInitData()) {
            fast::rf::Logger::logError("Unable to init Base Data! Exiting.");
            return false;
        }
        if (!this->loadConfig()) {
            fast::rf::Logger::logError("Unable to load User Config! Exiting.");
            return false;
        }
        if (!this->initPubSubs()) {
            fast::rf::Logger::logError("Unable to init User Pubs and Subs! Exiting.");
            return false;
        }
        if (!this->initServices()) {
            fast::rf::Logger::logError("Unable to init User Services! Exiting.");
            return false;
        }
        if (!this->initDiagnostics()) {
            fast::rf::Logger::logError("Unable to init User Diagnostics! Exiting.");
            return false;
        }
        if (!this->initData()) {
            fast::rf::Logger::logError("Unable to init User Data! Exiting.");
            return false;
        }
        return true;
    }
    bool BaseNode::start() {
        if (!requestNodeStateChange(robot_framework_ros2::msg::NodeState::STATE_STARTING, false)) {
            return false;
        }
        m_timer100Hz = this->create_wall_timer(10ms, std::bind(&BaseNode::timer100HzCallback, this));
        m_timer10Hz = this->create_wall_timer(100ms, std::bind(&BaseNode::timer10HzCallback, this));
        m_timer1Hz = this->create_wall_timer(1s, std::bind(&BaseNode::timer1HzCallback, this));
        m_timer01Hz = this->create_wall_timer(10s, std::bind(&BaseNode::timer01HzCallback, this));
        m_timer001Hz = this->create_wall_timer(100s, std::bind(&BaseNode::timer001HzCallback, this));

        double loop1Rate = this->declare_parameter<double>("loop1_rate", -1.0);
        if (loop1Rate <= 0.0) {
            fast::rf::Logger::logWarn("Parameter: loop1_rate not present or invalid. Not running User Loop1 Code.");
        } else {
            m_timerLoop1 = this->create_wall_timer(std::chrono::duration<double>(1.0 / loop1Rate),
                                                   std::bind(&BaseNode::timerLoop1Callback, this));
        }
        double loop2Rate = this->declare_parameter<double>("loop2_rate", -1.0);
        if (loop2Rate <= 0.0) {
            fast::rf::Logger::logWarn("Parameter: loop2_rate not present or invalid. Not running User Loop2 Code.");
        } else {
            m_timerLoop2 = this->create_wall_timer(std::chrono::duration<double>(1.0 / loop2Rate),
                                                   std::bind(&BaseNode::timerLoop2Callback, this));
        }
        double loop3Rate = this->declare_parameter<double>("loop3_rate", -1.0);
        if (loop3Rate <= 0.0) {
            fast::rf::Logger::logWarn("Parameter: loop3_rate not present or invalid. Not running User Loop1 Code.");
        } else {
            m_timerLoop3 = this->create_wall_timer(std::chrono::duration<double>(1.0 / loop3Rate),
                                                   std::bind(&BaseNode::timerLoop3Callback, this));
        }

        if (!requestNodeStateChange(robot_framework_ros2::msg::NodeState::STATE_RUNNING, false)) {
            return false;
        }
        return true;
    }
    bool BaseNode::restart() {
        if (!requestNodeStateChange(robot_framework_ros2::msg::NodeState::STATE_STARTING, false)) {
            return false;
        }
        m_timer100Hz->cancel();
        m_timer10Hz->cancel();
        m_timer1Hz->cancel();
        m_timer01Hz->cancel();
        m_timer001Hz->cancel();
        if (m_timerLoop1) {
            m_timerLoop1->cancel();
        }
        if (m_timerLoop2) {
            m_timerLoop2->cancel();
        }
        if (m_timerLoop3) {
            m_timerLoop3->cancel();
        }
        if (!this->initDiagnostics()) {
            fast::rf::Logger::logError("Unable to Re-Initialize User Diagnostics! Exiting.");
            return false;
        }
        if (!this->initData()) {
            fast::rf::Logger::logError("Unable to Re-Initialize User Data! Exiting.");
            return false;
        }
        if (!requestNodeStateChange(robot_framework_ros2::msg::NodeState::STATE_RUNNING, false)) {
            return false;
        }
        m_timer100Hz->reset();
        m_timer10Hz->reset();
        m_timer1Hz->reset();
        m_timer01Hz->reset();
        m_timer001Hz->reset();

        if (m_timerLoop1) {
            m_timerLoop1->reset();
        }
        if (m_timerLoop2) {
            m_timerLoop2->reset();
        }
        if (m_timerLoop3) {
            m_timerLoop3->reset();
        }
        return true;
    }
    void BaseNode::timer100HzCallback() {
        baseRun100Hz();
        this->run100Hz();
    }
    void BaseNode::timer10HzCallback() {
        baseRun10Hz();
        this->run10Hz();
    }

    void BaseNode::timer1HzCallback() {
        baseRun1Hz();
        this->run1Hz();
    }
    void BaseNode::timer01HzCallback() {
        baseRun01Hz();
        this->run01Hz();
    }
    void BaseNode::timer001HzCallback() {
        baseRun001Hz();
        this->run001Hz();
    }
    void BaseNode::timerLoop1Callback() { this->runLoop1(); }
    void BaseNode::timerLoop2Callback() { this->runLoop2(); }
    void BaseNode::timerLoop3Callback() { this->runLoop3(); }
    void BaseNode::baseRun100Hz() {}
    void BaseNode::baseRun10Hz() {
        m_heartbeat.timestamp = this->get_clock()->now();
        m_heartbeat.node_state = m_nodeState;
        m_heartbeatPub->publish(m_heartbeat);
    }
    void BaseNode::baseRun1Hz() { m_readyToArmPub->publish(m_readyToArm); }
    void BaseNode::baseRun01Hz() {}
    void BaseNode::baseRun001Hz() {}
    bool BaseNode::requestNodeStateChange(uint8_t newState, bool override) {
        uint8_t currentState = m_nodeState.state;
        if (currentState == newState) {
            return true;
        }
        bool stateChangeAllowed = false;
        if (override == false) {
            if (currentState == robot_framework_ros2::msg::NodeState::STATE_UNKNOWN) {
                if (newState == robot_framework_ros2::msg::NodeState::STATE_INITIALIZING) {
                    stateChangeAllowed = true;
                }
            } else if (currentState == robot_framework_ros2::msg::NodeState::STATE_INITIALIZING) {
                if (newState == robot_framework_ros2::msg::NodeState::STATE_STARTING) {
                    stateChangeAllowed = true;
                }
            } else if (currentState == robot_framework_ros2::msg::NodeState::STATE_STARTING) {
                if (newState == robot_framework_ros2::msg::NodeState::STATE_RUNNING) {
                    stateChangeAllowed = true;
                }
            } else if (currentState == robot_framework_ros2::msg::NodeState::STATE_RUNNING) {
                if (newState == robot_framework_ros2::msg::NodeState::STATE_STARTING) {
                    stateChangeAllowed = true;
                }
            }
        }
        if (stateChangeAllowed == true) {
            m_nodeState.state = newState;
            return true;
        } else {
            fast::rf::Logger::logError("Node State Change Not Allowed: " + std::to_string(currentState) + " -> " +
                                       std::to_string(newState));
            return false;
        }
    }
    // Util Functions
    std::string BaseNode::getHostName() {
        std::string hostname;
        char hostnameBuffer[256];
        if (gethostname(hostnameBuffer, sizeof(hostnameBuffer)) == 0) {
            // Assuming your Heartbeat.msg has a string field (e.g., string hostname)
            hostname = std::string(hostnameBuffer);
        } else {
            hostname = "";
        }
        return hostname;
    }
}  // namespace fast::rf_ros2
int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = fast::rf_ros2::BaseNode::createNode();
    if (node->init() == false) {
        fast::rf::Logger::logError("Unable to initialize Node: " + std::string(node->get_name()) + "! Exiting.");
        return 1;
    }
    if (node->start() == false) {
        fast::rf::Logger::logError("Unable to start Node: " + std::string(node->get_name()) + "! Exiting.");
        return 1;
    }

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
