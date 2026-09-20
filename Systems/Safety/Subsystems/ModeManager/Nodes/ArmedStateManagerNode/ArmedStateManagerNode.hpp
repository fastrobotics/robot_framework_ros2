/**
 * @file ArmedStateManagerNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once
#include <ArmedStateManagerProcess.hpp>

#include "robot_framework_ros2/BaseNode.hpp"
#include "robot_framework_ros2/msg/arm_command.hpp"
#include "robot_framework_ros2/srv/arm_state_change.hpp"
namespace fast::rf_ros2::SafetySystem::ModeManagerSubsystem::ArmedStateManager {
    class ArmedStateManagerNode : public BaseNode {
       public:
        ArmedStateManagerNode() : BaseNode("armed_state_manager_node") {}

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
        void readyToArmCallback(const robot_framework_ros2::msg::ReadyToArm::SharedPtr msg);
        void armStateChangeService(const std::shared_ptr<robot_framework_ros2::srv::ArmStateChange::Request> request,
                                   std::shared_ptr<robot_framework_ros2::srv::ArmStateChange::Response> response);
        std::string pretty() override;
        fast::rf::SafetySystem::ModeManagerSubsystem::ArmedStateManager::ArmedStateManagerProcess m_process;
        std::vector<std::string> m_nodesToMonitor;

        // Pubs & Subs
        std::vector<rclcpp::Subscription<robot_framework_ros2::msg::ReadyToArm>::SharedPtr> m_readyToArmSubs;
        rclcpp::Publisher<robot_framework_ros2::msg::ArmCommand>::SharedPtr m_armCommandPub;

        // Services
        rclcpp::Service<robot_framework_ros2::srv::ArmStateChange>::SharedPtr m_armStateChangeSrvServer;
    };
}  // namespace fast::rf_ros2::SafetySystem::ModeManagerSubsystem::ArmedStateManager