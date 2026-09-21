/**
 * @file IWindow.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-08-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <string>

#include "Definitions.hpp"
#include "robot_framework_ros2/msg/arm_command.hpp"
#include "robot_framework_ros2/msg/diagnostic.hpp"
#include "robot_framework_ros2/msg/heartbeat.hpp"
#include "robot_framework_ros2/msg/ready_to_arm.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    /**
     * @brief A generic Window interface
     *
     */
    class IWindow {
       public:
        virtual ~IWindow() {}
        /**
         * @brief Get the name
         *
         * @return std::string
         */
        virtual std::string getName() = 0;
        /**
         * @brief Check if initialized
         *
         * @return true
         * @return false
         */
        virtual bool isInitialized() = 0;
        /**
         * @brief Update the object
         *
         * @param currentTimeSec
         * @return true
         * @return false
         */
        virtual bool update(double currentTimeSec) = 0;
        /**
         * @brief Pretty print human readable data
         *
         * @return std::string
         */
        virtual std::string pretty() = 0;

        /**
         * @brief Check if the window is currently focused
         *
         * @return true
         * @return false
         */
        virtual bool hasFocus() = 0;

        /**
         * @brief Set the focus
         *
         * @param cmdFocus
         * @return true
         * @return false
         */
        virtual bool setFocus(bool cmdFocus) = 0;

        virtual KeyEventContainer newKeyEvent(int key) = 0;

        virtual void newArmCommandMsg(robot_framework_ros2::msg::ArmCommand msg) = 0;

        virtual void newHeartbeatMsg(robot_framework_ros2::msg::Heartbeat msg) = 0;

        virtual void newReadyToArmMsg(robot_framework_ros2::msg::ReadyToArm msg) = 0;

        virtual void newDiagnosticMsg(robot_framework_ros2::msg::Diagnostic msg) = 0;
    };
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor