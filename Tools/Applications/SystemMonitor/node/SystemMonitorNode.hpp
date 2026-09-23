/**
 * @file SystemMonitorNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once

#include <curses.h>

#include "IWindow.hpp"
#include "robot_framework_ros2/BaseNode.hpp"
#include "robot_framework_ros2/msg/arm_command.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    class SystemMonitorNode : public BaseNode {
       public:
        SystemMonitorNode() : BaseNode("system_monitor") {
            m_autoScanTopicTypes = {"robot_framework_ros2/msg/Heartbeat", "robot_framework_ros2/msg/Diagnostic",
                                    "robot_framework_ros2/msg/ReadyToArm"};
        }
        ~SystemMonitorNode() {
            fast::rf::Logger::logError("Cleaning up");
            m_windows.clear();
            endwin();
        }

       protected:
        void run100Hz() override;
        void run10Hz() override;
        void run1Hz() override;
        void run01Hz() override;
        void run001Hz() override;
        void runLoop1() override;
        void runLoop2() override;
        void runLoop3() override;

        bool loadConfig() override;
        bool initPubSubs() override;
        bool initServices() override;
        bool initDiagnostics() override;
        bool initData() override;

       private:
        bool rescanROSNetwork();
        void armCommandCallback(const robot_framework_ros2::msg::ArmCommand::SharedPtr msg);
        void heartbeatCallback(const robot_framework_ros2::msg::Heartbeat::SharedPtr msg, const std::string& topicName);
        void diagnosticCallback(const robot_framework_ros2::msg::Diagnostic::SharedPtr msg,
                                const std::string& topicName);
        void readyToArmCallback(const robot_framework_ros2::msg::ReadyToArm::SharedPtr msg,
                                const std::string& topicName);
        void subscribeToTopic(const std::string& name, const std::string& type);
        bool initScreen();
        bool initWindows();
        std::string pretty() override;
        std::map<std::string, std::shared_ptr<IWindow>> m_windows;
        std::string m_selectedNode{""};

        std::unordered_set<std::string> m_autoScanTopicTypes;
        std::unordered_map<std::string, rclcpp::SubscriptionBase::SharedPtr> m_autoSubs;
        rclcpp::Subscription<robot_framework_ros2::msg::ArmCommand>::SharedPtr m_armCommandSub;
    };
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor