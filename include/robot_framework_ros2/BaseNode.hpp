/**
 * @file BaseNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <DiagnosticMsg.hpp>
#include <Infrastructure/Logger.hpp>

#include "rclcpp/rclcpp.hpp"

// Interface Definitions
#include <robot_framework_ros2/msg/diagnostic.hpp>
#include <robot_framework_ros2/msg/heartbeat.hpp>
#include <robot_framework_ros2/msg/node_state.hpp>
#include <robot_framework_ros2/msg/ready_to_arm.hpp>

namespace fast::rf_ros2 {
    class BaseNode : public rclcpp::Node {
       public:
        BaseNode(const std::string& node_name);
        virtual ~BaseNode() = default;

        // Factory method declaration
        static std::shared_ptr<BaseNode> createNode();

        /**
         * @brief Initialize the Node
         *
         * @return true
         * @return false
         */
        bool init();

        /**
         * @brief Start the Node
         *
         * @return true
         * @return false
         */
        bool start();

       protected:
        std::string getHostName();
        void setDiagnostics(std::vector<fast::rf::messages::InfrastructureMsgs::DiagnosticMsg> diagnostics) {
            m_diagnostics = diagnostics;
        }
        // User Timing Loops
        virtual void run100Hz() = 0;
        virtual void run10Hz() = 0;
        virtual void run1Hz() = 0;
        virtual void run01Hz() = 0;
        virtual void run001Hz() = 0;
        virtual void runLoop1() = 0;
        virtual void runLoop2() = 0;
        virtual void runLoop3() = 0;

        /**
         * @brief General Config Loader.  Anything can load config for specific config needs.
         *
         * @return true
         * @return false
         */
        virtual bool loadConfig() = 0;
        virtual bool initPubSubs() = 0;
        virtual bool initServices() = 0;
        virtual bool initDiagnostics() = 0;
        virtual bool initData() = 0;

       private:
        bool restart();
        /**
         * @brief Load and set Data Members Config.
         *
         * @return true
         * @return false
         */
        bool baseLoadConfig();
        bool baseInitPubSubs();
        bool baseInitServices();
        bool baseInitDiagnostics();
        bool baseInitData();

        bool requestNodeStateChange(uint8_t newState, bool override = false);
        // Properties
        std::string m_baseNodeName{""};
        robot_framework_ros2::msg::NodeState m_nodeState;
        robot_framework_ros2::msg::Heartbeat m_heartbeat;
        robot_framework_ros2::msg::ReadyToArm m_readyToArm;

        // Loop Callbacks and Timers
        virtual void timer100HzCallback();
        virtual void timer10HzCallback();
        virtual void timer1HzCallback();
        virtual void timer01HzCallback();
        virtual void timer001HzCallback();
        virtual void timerLoop1Callback();
        virtual void timerLoop2Callback();
        virtual void timerLoop3Callback();
        rclcpp::TimerBase::SharedPtr m_timer100Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timer10Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timer1Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timer01Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timer001Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timerLoop1{nullptr};
        rclcpp::TimerBase::SharedPtr m_timerLoop2{nullptr};
        rclcpp::TimerBase::SharedPtr m_timerLoop3{nullptr};
        void baseRun100Hz();
        void baseRun10Hz();
        void baseRun1Hz();
        void baseRun01Hz();
        void baseRun001Hz();

        // Pubs and Subs
        rclcpp::Publisher<robot_framework_ros2::msg::Heartbeat>::SharedPtr m_heartbeatPub;
        rclcpp::Publisher<robot_framework_ros2::msg::ReadyToArm>::SharedPtr m_readyToArmPub;
        rclcpp::Publisher<robot_framework_ros2::msg::Diagnostic>::SharedPtr m_diagnosticPub;

        // Data
        std::vector<fast::rf::messages::InfrastructureMsgs::DiagnosticMsg> m_diagnostics;
    };
}  // namespace fast::rf_ros2
