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
#include <ReadyToArmStatusMsg.hpp>

#include "rclcpp/rclcpp.hpp"

// Interface Definitions
#include <robot_framework_ros2/msg/diagnostic.hpp>
#include <robot_framework_ros2/msg/heartbeat.hpp>
#include <robot_framework_ros2/msg/node_state.hpp>
#include <robot_framework_ros2/msg/ready_to_arm.hpp>
#include <robot_framework_ros2/srv/change_logger_level.hpp>

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

        /**
         * @brief Restart the Node
         *
         * @return true
         * @return false
         */
        bool reset();

       protected:
        std::string getHostName();
        std::string getBaseNodeName() const { return m_baseNodeName; }
        std::string getRobotNamespace() { return m_robotNamespace; }
        std::string getNamespacedTopic(const std::string& topic) const;
        std::string getNodeConfigNamespace() { return m_nodeConfigNamespace; }
        void setDiagnostics(std::vector<fast::rf::messages::InfrastructureMsgs::DiagnosticMsg> diagnostics) {
            m_diagnostics = diagnostics;
        }
        void setReadyToArm(fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg readyToArm) {
            m_systemID = readyToArm.systemID;
            m_subsystemID = readyToArm.subsystemID;
            m_processID = readyToArm.processID;
            m_readyToArm.system_id = readyToArm.systemID;
            m_readyToArm.subsystem_id = readyToArm.subsystemID;
            m_readyToArm.process_id = readyToArm.processID;
            m_readyToArm.ready_to_arm = readyToArm.ready_to_arm;
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

        virtual std::string pretty() = 0;

       private:
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
        bool m_loop100HzOk{true};
        virtual void timer10HzCallback();
        bool m_loop10HzOk{true};
        virtual void timer1HzCallback();
        bool m_loop1HzOk{true};
        virtual void timer01HzCallback();
        bool m_loop_01HzOk{true};
        virtual void timer001HzCallback();
        bool m_loop001HzOk{true};
        virtual void timerLoop1Callback();
        bool m_loop1Ok{true};
        virtual void timerLoop2Callback();
        bool m_loop2Ok{true};
        virtual void timerLoop3Callback();
        bool m_loop3Ok{true};
        rclcpp::TimerBase::SharedPtr m_timer100Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timer10Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timer1Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timer01Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timer001Hz{nullptr};
        rclcpp::TimerBase::SharedPtr m_timerLoop1{nullptr};
        double m_loop1Rate{-1.0};
        rclcpp::TimerBase::SharedPtr m_timerLoop2{nullptr};
        double m_loop2Rate{-1.0};
        rclcpp::TimerBase::SharedPtr m_timerLoop3{nullptr};
        double m_loop3Rate{-1.0};

        void baseRun100Hz();
        void baseRun10Hz();
        void baseRun1Hz();
        void baseRun01Hz();
        void baseRun001Hz();

        // Pubs and Subs
        rclcpp::Publisher<robot_framework_ros2::msg::Heartbeat>::SharedPtr m_heartbeatPub;
        rclcpp::Publisher<robot_framework_ros2::msg::Diagnostic>::SharedPtr m_diagnosticPub;
        rclcpp::Publisher<robot_framework_ros2::msg::ReadyToArm>::SharedPtr m_readyToArmPub;

        // Services
        void changeLoggerLevelService(
            const std::shared_ptr<robot_framework_ros2::srv::ChangeLoggerLevel::Request> request,
            std::shared_ptr<robot_framework_ros2::srv::ChangeLoggerLevel::Response> response);
        rclcpp::Service<robot_framework_ros2::srv::ChangeLoggerLevel>::SharedPtr m_changeLoggerLevelSrvServer;

        // Data
        uint8_t m_systemID{0};
        uint8_t m_subsystemID{0};
        uint8_t m_processID{0};
        std::vector<fast::rf::messages::InfrastructureMsgs::DiagnosticMsg> m_diagnostics;
        std::string m_robotNamespace;
        std::string m_nodeConfigNamespace;
        fast::rf::messages::InfrastructureMsgs::DiagnosticMsg m_timingDiagnostic;
    };
}  // namespace fast::rf_ros2
