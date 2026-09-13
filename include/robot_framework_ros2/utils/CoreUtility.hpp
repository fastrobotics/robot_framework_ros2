/**
 * @file CoreUtility.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <robot_framework_ros2/msg/node_state.hpp>

namespace fast::rf_ros2 {
    namespace utils {
        class CoreUtility {
           public:
            static std::string pretty(robot_framework_ros2::msg::NodeState state);
        };
    }  // namespace utils
}  // namespace fast::rf_ros2