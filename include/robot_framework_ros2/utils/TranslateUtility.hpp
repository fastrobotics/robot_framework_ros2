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
#include <DiagnosticMsg.hpp>

#include "robot_framework_ros2/msg/diagnostic.hpp"

namespace fast::rf_ros2::utils {
    /**
     * @brief Translation Utilities
     *
     */
    class TranslateUtility {
       public:
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