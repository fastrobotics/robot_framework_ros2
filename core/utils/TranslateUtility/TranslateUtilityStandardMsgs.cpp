#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2::utils {
    fast::rf::messages::StandardMsgs::Vector3DMsg TranslateUtility::convert(geometry_msgs::msg::Vector3 msg) {
        fast::rf::messages::StandardMsgs::Vector3DMsg data;
        data.x = msg.x;
        data.y = msg.y;
        data.z = msg.z;
        return data;
    }

    geometry_msgs::msg::Vector3 TranslateUtility::convert(fast::rf::messages::StandardMsgs::Vector3DMsg msg) {
        geometry_msgs::msg::Vector3 data;
        data.x = msg.x;
        data.y = msg.y;
        data.z = msg.z;
        return data;
    }
    fast::rf::messages::StandardMsgs::Covariance3DMsg TranslateUtility::convert_covariance3D(
        const std::array<double, fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                                     fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION>& msg) {
        fast::rf::messages::StandardMsgs::Covariance3DMsg data;
        data.covariance.reserve(msg.size());
        data.covariance.assign(msg.begin(), msg.end());
        return data;
    }
    std::array<double, fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                           fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION>
    TranslateUtility::convert_covariance3D(const fast::rf::messages::StandardMsgs::Covariance3DMsg& msg) {
        std::array<double, fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                               fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION>
            data;
        std::copy(msg.covariance.begin(), msg.covariance.end(), data.begin());
        return data;
    }

    fast::rf::messages::StandardMsgs::Covariance6DMsg TranslateUtility::convert_covariance6D(
        const std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                                     fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>& msg) {
        fast::rf::messages::StandardMsgs::Covariance6DMsg data;
        data.covariance.assign(msg.begin(), msg.end());
        return data;
    }

    std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                           fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>
    TranslateUtility::convert_covariance6D(const fast::rf::messages::StandardMsgs::Covariance6DMsg& msg) {
        std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                               fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>
            data;
        std::copy(msg.covariance.begin(), msg.covariance.end(), data.begin());
        return data;
    }
}  // namespace fast::rf_ros2::utils