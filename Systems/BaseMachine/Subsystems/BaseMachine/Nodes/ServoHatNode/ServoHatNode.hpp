/**
 * @file ServoHatNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once
#include <ServoHatDriverProcess.hpp>
#include <robot_framework_ros2/msg/arm_command.hpp>

// Messages
#include <std_msgs/msg/float64.hpp>

#include "robot_framework_ros2/BaseNode.hpp"
namespace fast::rf_ros2::BaseMachineSystem::BaseMachineSubsystem::HatDriver {
    class ServoHatNode : public BaseNode {
       public:
        ServoHatNode() : BaseNode("servo_hat_node") {}

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
        void robotArmCommandCallback(const robot_framework_ros2::msg::ArmCommand::SharedPtr msg);
        void driveCallback(uint16_t channel, const std_msgs::msg::Float64::SharedPtr msg);
        std::string pretty() override;
        fast::rf::BaseMachineSystem::BaseMachineSubsystem::HatDriver::ServoHatDriverProcess m_process;

        // Pubs & Subs
        rclcpp::Subscription<robot_framework_ros2::msg::ArmCommand>::SharedPtr m_armCommandSub;
        rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr m_leftDriveSub;
        rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr m_rightDriveSub;
    };
}  // namespace fast::rf_ros2::BaseMachineSystem::BaseMachineSubsystem::HatDriver