/**
 * @file base_node.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <Infrastructure/Logger.hpp>

#include "rclcpp/rclcpp.hpp"

// Interface Definitions
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

       protected:
        std::string getHostName();
        // User Timing Loops
        virtual void run10Hz() = 0;
        virtual void run1Hz() = 0;

       private:
        // Properties
        std::string m_baseNodeName{""};
        robot_framework_ros2::msg::NodeState m_nodeState;
        robot_framework_ros2::msg::Heartbeat m_heartbeat;
        robot_framework_ros2::msg::ReadyToArm m_readyToArm;

        // Loop Callbacks and Timers
        virtual void timer10HzCallback();
        virtual void timer1HzCallback();
        rclcpp::TimerBase::SharedPtr m_timer10Hz;
        rclcpp::TimerBase::SharedPtr m_timer1Hz;
        bool baseRun10Hz();
        bool baseRun1Hz();

        // Pubs and Subs
        rclcpp::Publisher<robot_framework_ros2::msg::Heartbeat>::SharedPtr m_heartbeatPub;
        rclcpp::Publisher<robot_framework_ros2::msg::ReadyToArm>::SharedPtr m_readyToArmPub;
    };
}  // namespace fast::rf_ros2
