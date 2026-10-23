// Robot Framework Includes
#include "robot_framework_ros2/BaseNode.hpp"

#include "robot_framework_ros2/utils/CoreUtility.hpp"
#include "robot_framework_ros2/utils/TranslateUtility.hpp"
// ROS2 Includes

// C++ Includes
#include <unistd.h>

#include <chrono>
#include <functional>

using namespace std::chrono_literals;
namespace fast::rf_ros2 {
    namespace {
        class TimerLoopTiming {
           public:
            static void run(const std::string& loopName, double rate, const std::function<void()>& callback,
                            bool& loopOk, fast::rf::messages::InfrastructureMsgs::DiagnosticMsg& diagnostic) {
                const auto start = std::chrono::steady_clock::now();
                callback();
                const double elapsedSeconds =
                    std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
                const double expectedPeriodSeconds = 1.0 / rate;
                const double slowDownRate = elapsedSeconds / expectedPeriodSeconds;

                if (slowDownRate > 1.0) {
                    loopOk = false;
                    diagnostic.level = fast::rf::Level::WARN;
                    diagnostic.diagnosticMessage = fast::rf::DiagnosticDefinition::DiagnosticMessage::DIAGNOSTIC_FAILED;
                    diagnostic.description = loopName + " Slow: Rate: " + std::to_string(slowDownRate) + "X " +
                                             std::to_string(elapsedSeconds) +
                                             " (sec) Expected: " + std::to_string(expectedPeriodSeconds);
                    fast::rf::Logger::logDiagnostic(diagnostic);
                } else {
                    loopOk = true;
                }
            }
        };
    }  // namespace

    BaseNode::BaseNode(const std::string& nodeName)
        : Node(nodeName,
               rclcpp::NodeOptions().allow_undeclared_parameters(true).automatically_declare_parameters_from_overrides(
                   false)) {
        m_baseNodeName = nodeName;
        m_nodeState.state = robot_framework_ros2::msg::NodeState::STATE_UNKNOWN;
        // Initialize Base Node Diagnostics

        m_timingDiagnostic.diagnosticType = fast::rf::DiagnosticDefinition::DiagnosticType::TIMING;
        m_timingDiagnostic.level = fast::rf::Level::INFO;
        m_timingDiagnostic.diagnosticMessage = fast::rf::DiagnosticDefinition::DiagnosticMessage::INITIALIZING;
        m_timingDiagnostic.description = "Initializing Diagnostic";
    }
    void BaseNode::changeLoggerLevelService(
        const std::shared_ptr<robot_framework_ros2::srv::ChangeLoggerLevel::Request> request,
        std::shared_ptr<robot_framework_ros2::srv::ChangeLoggerLevel::Response> response) {
        if (fast::rf::Logger::changeLoggerLevel((fast::rf::Level)request->verbosity_level) == true) {
            response->request_approved = true;
        } else {
            response->request_approved = false;
        }
    }
    std::string BaseNode::getNamespacedTopic(const std::string& topic) const {
        std::string robotNamespace = m_robotNamespace;
        while (!robotNamespace.empty() && robotNamespace.back() == '/') {
            robotNamespace.pop_back();
        }

        const auto topicStart = topic.find_first_not_of('/');
        const std::string relativeTopic = topicStart == std::string::npos ? std::string{} : topic.substr(topicStart);

        if (relativeTopic.empty()) {
            return robotNamespace.empty() ? std::string{"/"} : robotNamespace;
        }

        if (robotNamespace.empty()) {
            return std::string{"/"} + relativeTopic;
        }

        return robotNamespace + "/" + relativeTopic;
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
        m_robotNamespace = this->declare_parameter<std::string>("robot_namespace", "");
        while (m_robotNamespace.size() > 1 && m_robotNamespace.back() == '/') {
            m_robotNamespace.pop_back();
        }
        if (m_robotNamespace == "/") {
            m_robotNamespace.clear();
        } else if (m_robotNamespace.empty() || m_robotNamespace.front() != '/') {
            m_robotNamespace.insert(0, "/");
        }
        m_nodeConfigNamespace = this->declare_parameter<std::string>("node_namespace", "");
        while (!m_nodeConfigNamespace.empty() && m_nodeConfigNamespace.front() == '/') {
            m_nodeConfigNamespace.erase(0, 1);
        }
        for (char& character : m_nodeConfigNamespace) {
            if (character == '/') {
                character = '.';
            }
        }
        return true;
    }
    bool BaseNode::baseInitPubSubs() {
        m_heartbeatPub =
            this->create_publisher<robot_framework_ros2::msg::Heartbeat>("~/heartbeat", 10);  // Under node name
        m_readyToArmPub =
            this->create_publisher<robot_framework_ros2::msg::ReadyToArm>("~/ready_to_arm", 10);  // Under node name
        m_diagnosticPub =
            this->create_publisher<robot_framework_ros2::msg::Diagnostic>("~/diagnostic", 10);  // Under node name
        return true;
    }
    bool BaseNode::baseInitServices() {
        m_changeLoggerLevelSrvServer = this->create_service<robot_framework_ros2::srv::ChangeLoggerLevel>(
            "~/change_logger_level",
            std::bind(&BaseNode::changeLoggerLevelService, this, std::placeholders::_1, std::placeholders::_2));
        return true;
    }
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

        m_loop1Rate = this->declare_parameter<double>("loop1_rate", -1.0);
        if (m_loop1Rate <= 0.0) {
            fast::rf::Logger::logWarn("Parameter: loop1_rate not present or invalid. Not running User Loop1 Code.");
        } else {
            m_timerLoop1 = this->create_wall_timer(std::chrono::duration<double>(1.0 / m_loop1Rate),
                                                   std::bind(&BaseNode::timerLoop1Callback, this));
        }
        m_loop2Rate = this->declare_parameter<double>("loop2_rate", -1.0);
        if (m_loop2Rate <= 0.0) {
            fast::rf::Logger::logWarn("Parameter: loop2_rate not present or invalid. Not running User Loop2 Code.");
        } else {
            m_timerLoop2 = this->create_wall_timer(std::chrono::duration<double>(1.0 / m_loop2Rate),
                                                   std::bind(&BaseNode::timerLoop2Callback, this));
        }
        double loop3Rate = this->declare_parameter<double>("loop3_rate", -1.0);
        if (loop3Rate <= 0.0) {
            fast::rf::Logger::logWarn("Parameter: loop3_rate not present or invalid. Not running User Loop3 Code.");
        } else {
            m_loop3Rate = loop3Rate;
            m_timerLoop3 = this->create_wall_timer(std::chrono::duration<double>(1.0 / loop3Rate),
                                                   std::bind(&BaseNode::timerLoop3Callback, this));
        }

        if (!requestNodeStateChange(robot_framework_ros2::msg::NodeState::STATE_RUNNING, false)) {
            return false;
        }
        return true;
    }
    bool BaseNode::reset() {
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
        TimerLoopTiming::run("Loop100Hz", 100.0, [this]() { this->run100Hz(); }, m_loop100HzOk, m_timingDiagnostic);
    }
    void BaseNode::timer10HzCallback() {
        baseRun10Hz();
        TimerLoopTiming::run("Loop10Hz", 10.0, [this]() { this->run10Hz(); }, m_loop10HzOk, m_timingDiagnostic);
    }

    void BaseNode::timer1HzCallback() {
        baseRun1Hz();
        TimerLoopTiming::run("Loop1Hz", 1.0, [this]() { this->run1Hz(); }, m_loop1HzOk, m_timingDiagnostic);
    }
    void BaseNode::timer01HzCallback() {
        baseRun01Hz();
        TimerLoopTiming::run("Loop01Hz", 0.1, [this]() { this->run01Hz(); }, m_loop_01HzOk, m_timingDiagnostic);
    }
    void BaseNode::timer001HzCallback() {
        baseRun001Hz();
        TimerLoopTiming::run("Loop001Hz", 0.01, [this]() { this->run001Hz(); }, m_loop001HzOk, m_timingDiagnostic);
    }
    void BaseNode::timerLoop1Callback() {
        TimerLoopTiming::run("Loop1", m_loop1Rate, [this]() { this->runLoop1(); }, m_loop1Ok, m_timingDiagnostic);
    }
    void BaseNode::timerLoop2Callback() {
        TimerLoopTiming::run("Loop2", m_loop2Rate, [this]() { this->runLoop2(); }, m_loop2Ok, m_timingDiagnostic);
    }
    void BaseNode::timerLoop3Callback() {
        TimerLoopTiming::run("Loop3", m_loop3Rate, [this]() { this->runLoop3(); }, m_loop3Ok, m_timingDiagnostic);
    }
    void BaseNode::baseRun100Hz() {}
    void BaseNode::baseRun10Hz() {
        m_heartbeat.timestamp = this->get_clock()->now();
        m_heartbeat.node_state = m_nodeState;
        m_heartbeatPub->publish(m_heartbeat);
    }
    void BaseNode::baseRun1Hz() {
        if (m_diagnostics.size() > 0) {
            for (auto diagnostic : m_diagnostics) {
                fast::rf::Logger::logDiagnostic(diagnostic);
                robot_framework_ros2::msg::Diagnostic diagnosticMsg =
                    fast::rf_ros2::utils::TranslateUtility::convert(diagnostic);

                diagnosticMsg.timestamp = this->get_clock()->now();
                diagnosticMsg.node_name = this->get_name();
                m_diagnosticPub->publish(diagnosticMsg);
            }
        }
        // Other Base Node Diagnostics
        bool timingDiagnosticsOk = true;
        {
            if (m_loop100HzOk && m_loop10HzOk && m_loop1HzOk && m_loop_01HzOk && m_loop001HzOk && m_loop1Ok &&
                m_loop2Ok && m_loop3Ok) {
                m_timingDiagnostic.level = fast::rf::Level::INFO;
                m_timingDiagnostic.diagnosticMessage = fast::rf::DiagnosticDefinition::DiagnosticMessage::NOERROR;
                m_timingDiagnostic.description = "No Loop Timing Issues";
                m_timingDiagnostic.systemID = m_systemID;
                m_timingDiagnostic.subsystemID = m_subsystemID;
                m_timingDiagnostic.processID = m_processID;
                robot_framework_ros2::msg::Diagnostic diagnosticMsg =
                    fast::rf_ros2::utils::TranslateUtility::convert(m_timingDiagnostic);

                diagnosticMsg.timestamp = this->get_clock()->now();
                diagnosticMsg.node_name = this->get_name();
                m_diagnosticPub->publish(diagnosticMsg);
            } else {
                timingDiagnosticsOk = false;
            }
        }
        if (timingDiagnosticsOk == false) {  // Timing Diagnostic
            m_timingDiagnostic.systemID = m_systemID;
            m_timingDiagnostic.subsystemID = m_subsystemID;
            m_timingDiagnostic.processID = m_processID;
            robot_framework_ros2::msg::Diagnostic diagnosticMsg =
                fast::rf_ros2::utils::TranslateUtility::convert(m_timingDiagnostic);

            diagnosticMsg.timestamp = this->get_clock()->now();
            diagnosticMsg.node_name = this->get_name();
            m_diagnosticPub->publish(diagnosticMsg);
        }
        m_readyToArmPub->publish(m_readyToArm);
    }
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
    std::string BaseNode::pretty() {
        std::string str = "Node State: " + fast::rf_ros2::utils::CoreUtility::pretty(m_nodeState);
        return str;
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
