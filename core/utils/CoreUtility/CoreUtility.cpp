#include "robot_framework_ros2/utils/CoreUtility.hpp"
namespace fast::rf_ros2::utils {
    std::string CoreUtility::pretty(robot_framework_ros2::msg::NodeState state) {
        std::string str;
        switch (state.state) {
            case robot_framework_ros2::msg::NodeState::STATE_UNKNOWN:
                str = "UNKNOWN";
                break;
            case robot_framework_ros2::msg::NodeState::STATE_INITIALIZING:
                str = "INITIALIZING";
                break;
            case robot_framework_ros2::msg::NodeState::STATE_STARTING:
                str = "STARTING";
                break;
            case robot_framework_ros2::msg::NodeState::STATE_RUNNING:
                str = "RUNNING";
                break;
            case robot_framework_ros2::msg::NodeState::STATE_PAUSED:
                str = "PAUSED";
                break;
            case robot_framework_ros2::msg::NodeState::STATE_RESTART:
                str = "RESTART";
                break;
            case robot_framework_ros2::msg::NodeState::STATE_FINISHED:
                str = "FINISHED";
                break;
            default:
                str = "UNKNOWN";
                break;
        }
        return str;
    }
}  // namespace fast::rf_ros2::utils