/**
 * @file SystemMonitorNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.3
 *
 */
#include "SystemMonitorNode.hpp"

#include "Windows/DiagnosticWindow.hpp"
#include "Windows/HeaderWindow.hpp"
#include "Windows/NodeInfoWindow.hpp"
#include "Windows/StatusWindow.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    void SystemMonitorNode::armCommandCallback(const robot_framework_ros2::msg::ArmCommand::SharedPtr msg) {
        robot_framework_ros2::msg::ArmCommand localMsg = *msg;
        for (const auto& window : m_windows) {
            window.second->newArmCommandMsg(localMsg);
        }
    }
    void SystemMonitorNode::heartbeatCallback(const robot_framework_ros2::msg::Heartbeat::SharedPtr msg) {
        robot_framework_ros2::msg::Heartbeat localMsg = *msg;
        for (const auto& window : m_windows) {
            window.second->newHeartbeatMsg(localMsg);
        }
    }
    void SystemMonitorNode::diagnosticCallback(const robot_framework_ros2::msg::Diagnostic::SharedPtr msg) {
        robot_framework_ros2::msg::Diagnostic localMsg = *msg;
        for (const auto& window : m_windows) {
            window.second->newDiagnosticMsg(localMsg);
        }
    }
    void SystemMonitorNode::readyToArmCallback(const robot_framework_ros2::msg::ReadyToArm::SharedPtr msg) {
        robot_framework_ros2::msg::ReadyToArm localMsg = *msg;
        for (const auto& window : m_windows) {
            window.second->newReadyToArmMsg(localMsg);
        }
    }
    bool SystemMonitorNode::loadConfig() { return true; }
    bool SystemMonitorNode::initPubSubs() {
        m_armCommandSub = this->create_subscription<robot_framework_ros2::msg::ArmCommand>(
            getRobotNamespace() + "arm_command", 10,
            [this](const robot_framework_ros2::msg::ArmCommand::SharedPtr msg) { this->armCommandCallback(msg); });
        return true;
    }
    bool SystemMonitorNode::initServices() { return true; }
    bool SystemMonitorNode::initDiagnostics() { return true; }
    bool SystemMonitorNode::initData() {
        if (initScreen() == false) {
            fast::rf::Logger::logError("Unable to initialize Screen!");
            return false;
        }
        return true;
    }
    void SystemMonitorNode::run100Hz() {}
    void SystemMonitorNode::run10Hz() {
        int keyPressed = getch();
        if ((keyPressed == Key::KEY_q) || (keyPressed == Key::KEY_Q)) {
            rclcpp::shutdown();
        }
        // Update all Windows
        for (const auto& window : m_windows) {
            if (window.second->hasFocus()) {
                auto output = window.second->newKeyEvent(keyPressed);
                if (output.message.level > fast::rf::Level::NOTICE) {
                    fast::rf::Logger::logWarn(output.message.text);
                }
            }
            window.second->update(this->get_clock()->now().seconds());

            if (window.second->getName() == "node_info_window") {
                auto node_info_window = std::dynamic_pointer_cast<NodeInfoWindow>(window.second);
                m_selectedNode = node_info_window->get_m_selectedNode();
            }
            if (window.second->getName() == "diagnostic_window") {
                auto diagnostic_window = std::dynamic_pointer_cast<DiagnosticWindow>(window.second);
                diagnostic_window->setNodeToMonitor(m_selectedNode);
            }
        }
        flushinp();
    }
    void SystemMonitorNode::run1Hz() {}
    void SystemMonitorNode::run01Hz() {
        if (rescanROSNetwork() == false) {
            fast::rf::Logger::logError("Unable to Scan ROS2 Network!");
        }
    }
    void SystemMonitorNode::run001Hz() {}
    void SystemMonitorNode::runLoop1() {}
    void SystemMonitorNode::runLoop2() {}
    void SystemMonitorNode::runLoop3() {}
    std::string SystemMonitorNode::pretty() {
        std::string str = "\n--- SystemMonitorNode ---\n";
        str += BaseNode::pretty() + "\n";
        return str;
    }
    bool SystemMonitorNode::initScreen() {
        setlocale(LC_ALL, "");
        mousemask(ALL_MOUSE_EVENTS, NULL);
        initscr();
        timeout(0);
        keypad(stdscr, TRUE);
        clear();
        if (has_colors() == FALSE) {
            endwin();
            // logger->enable_consoleprint();
            fast::rf::Logger::logError("Terminal does not support colors. Exiting.");
            return false;
        }
        curs_set(0);
        noecho();
        raw();

        start_color();
        init_color(COLOR_BLACK, 0, 0, 0);
        init_color(COLOR_GREEN, 0, 600, 0);
        init_color(10, 500, 0, 500);
        init_pair((uint8_t)Color::NO_COLOR, COLOR_WHITE, COLOR_BLACK);
        init_pair((uint8_t)Color::WHITE_COLOR, COLOR_BLACK, COLOR_WHITE);
        init_pair((uint8_t)Color::RED_COLOR, COLOR_WHITE, COLOR_RED);
        init_pair((uint8_t)Color::YELLOW_COLOR, COLOR_WHITE, COLOR_YELLOW);
        init_pair((uint8_t)Color::GREEN_COLOR, COLOR_WHITE, COLOR_GREEN);
        init_pair((uint8_t)Color::BLUE_COLOR, COLOR_WHITE, COLOR_BLUE);
        init_pair((uint8_t)Color::PURPLE_COLOR, COLOR_WHITE, 10);

        bool status = initWindows();
        if (status == false) {
            fast::rf::Logger::logError("Unable to initialize Windows!");
            return false;
        }
        return true;
    }
    bool SystemMonitorNode::initWindows() {
        uint16_t mainWindowWidth, mainWindowHeight;
        getmaxyx(stdscr, mainWindowHeight, mainWindowWidth);
        {
            auto window = std::make_shared<HeaderWindow>(-1, mainWindowHeight, mainWindowWidth);
            m_windows[window->getName()] = window;
        }
        {
            auto window = std::make_shared<NodeInfoWindow>(-1, mainWindowHeight, mainWindowWidth);
            window->setFocus(true);
            m_windows[window->getName()] = window;
        }
        {
            auto window = std::make_shared<StatusWindow>(-1, mainWindowHeight, mainWindowWidth);
            m_windows[window->getName()] = window;
        }
        {
            auto window = std::make_shared<DiagnosticWindow>(-1, mainWindowHeight, mainWindowWidth);
            m_windows[window->getName()] = window;
        }

        return true;
    }
    bool SystemMonitorNode::rescanROSNetwork() {
        std::map<std::string, std::vector<std::string>> topicInfo = this->get_topic_names_and_types();
        std::unordered_set<std::string> discoveredTargetTopics;

        // Get this node's identity details for filtering
        std::string myName = this->get_name();
        std::string myNamespace = this->get_namespace();

        for (const auto& [topicName, topicTypes] : topicInfo) {
            for (const auto& type : topicTypes) {
                if (m_autoScanTopicTypes.count(type) > 0) {
                    // ---- FILTER: Check if "this" node is publishing this topic ----
                    auto publishers = this->get_publishers_info_by_topic(topicName);
                    bool publishedBySelf = false;
                    for (const auto& pub_info : publishers) {
                        if (pub_info.node_name() == myName && pub_info.node_namespace() == myNamespace) {
                            publishedBySelf = true;
                            break;
                        }
                    }

                    if (publishedBySelf) {
                        continue;
                    }

                    discoveredTargetTopics.insert(topicName);
                    if (m_autoSubs.count(topicName) == 0) {
                        subscribeToTopic(topicName, type);
                    }
                    break;
                }
            }
        }

        for (auto it = m_autoSubs.begin(); it != m_autoSubs.end();) {
            if (discoveredTargetTopics.count(it->first) == 0) {
                it = m_autoSubs.erase(it);
            } else {
                ++it;
            }
        }

        return true;
    }
    void SystemMonitorNode::subscribeToTopic(const std::string& name, const std::string& type) {
        if (type == "robot_framework_ros2/msg/Heartbeat") {
            m_autoSubs[name] = this->create_subscription<robot_framework_ros2::msg::Heartbeat>(
                name, 10, std::bind(&SystemMonitorNode::heartbeatCallback, this, std::placeholders::_1));
        } else if (type == "robot_framework_ros2/msg/Diagnostic") {
            m_autoSubs[name] = this->create_subscription<robot_framework_ros2::msg::Diagnostic>(
                name, 10, std::bind(&SystemMonitorNode::diagnosticCallback, this, std::placeholders::_1));
        } else if (type == "robot_framework_ros2/msg/ReadyToArm") {
            m_autoSubs[name] = this->create_subscription<robot_framework_ros2::msg::ReadyToArm>(
                name, 10, std::bind(&SystemMonitorNode::readyToArmCallback, this, std::placeholders::_1));
        }
    }
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor
namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<fast::rf_ros2::Tools::Applications::SystemMonitor::SystemMonitorNode>();
    }
}  // namespace fast::rf_ros2
