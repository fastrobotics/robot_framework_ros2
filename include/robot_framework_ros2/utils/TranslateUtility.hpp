/**
 * @file TranslateUtility.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-09
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
// Robot Framework Dependencies
#include <Covariance3DMsg.hpp>
#include <Covariance6DMsg.hpp>
#include <DiagnosticMsg.hpp>
#include <Vector3DMsg.hpp>

// Robot Framework ROS2 Message Dependencies
#include "robot_framework_ros2/msg/diagnostic.hpp"

// ROS2 Message Dependencies
#include <geometry_msgs/msg/vector3.hpp>

// General Dependencies
#include <array>

namespace fast::rf_ros2::utils {
    /**
     * @brief Translation Utilities
     *
     */
    class TranslateUtility {
       public:
        // Standard Messages
        /**
         * @brief Convert from a ROS2 message to a Core message of type Vector3
         *
         * @param msg
         * @return fast::rf::messages::StandardMsgs::Vector3DMsg
         */
        static fast::rf::messages::StandardMsgs::Vector3DMsg convert(geometry_msgs::msg::Vector3 msg);

        /**
         * @brief Convert from a Core message to a ROS2 message of type Vector3
         *
         * @param msg
         * @return geometry_msgs::Vector3
         */
        static geometry_msgs::msg::Vector3 convert(fast::rf::messages::StandardMsgs::Vector3DMsg msg);
        /**
         * @brief Convert from a ROS2 Message to a Core Message of type Covariance3D
         *
         * @param msg
         * @return fast::rf::messages::StandardMsgs::Covariance3DMsg
         */
        static fast::rf::messages::StandardMsgs::Covariance3DMsg convert_covariance3D(
            const std::array<double, fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                                         fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION>& msg);

        /**
         * @brief Convert from a Core Message to a ROS2 Message of type Covariance3D
         *
         * @param msg
         * @return std::array<double, Covariance3DMsg::DIMENSION * Covariance3DMsg::DIMENSION>
         */

        static std::array<double, fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                                      fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION>
        convert_covariance3D(const fast::rf::messages::StandardMsgs::Covariance3DMsg& msg);

        /**
         * @brief Convert from a ROS2 Message to a Core Message of type Covariance6D
         *
         * @param msg
         * @return fast::rf::messages::StandardMsgs::Covariance6DMsg
         */
        static fast::rf::messages::StandardMsgs::Covariance6DMsg convert_covariance6D(
            const std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                                         fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>& msg);

        /**
         * @brief Convert from a Core Message to a ROS2 Message of type Covariance6D
         *
         * @param msg
         * @return std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
         * fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>
         */
        static std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                                      fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>
        convert_covariance6D(const fast::rf::messages::StandardMsgs::Covariance6DMsg& msg);

        // Infrastructure Messages

        /**
         * @brief Translate from a ROS message to a Core message of type Diagnostic
         *
         * @param msg
         * @return fast::rf::messages::InfrastructureMsgs::DiagnosticMsg
         */
        static fast::rf::messages::InfrastructureMsgs::DiagnosticMsg convert(robot_framework_ros2::msg::Diagnostic msg);

        /**
         * @brief Translate from a Core Message to a ROS message of type Diagnostic
         *
         * @param msg
         * @return robot_framework_ros::diagnostic
         */
        static robot_framework_ros2::msg::Diagnostic convert(fast::rf::messages::InfrastructureMsgs::DiagnosticMsg msg);
    };
}  // namespace fast::rf_ros2::utils