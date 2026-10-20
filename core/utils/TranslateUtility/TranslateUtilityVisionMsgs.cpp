

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2::utils {
    fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg TranslateUtility::convert(
        vision_msgs::msg::ObjectHypothesisWithPose msg) {
        fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg data;
        data.score = msg.hypothesis.score;
        return data;
    }

    vision_msgs::msg::ObjectHypothesisWithPose TranslateUtility::convert(
        fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg msg) {
        vision_msgs::msg::ObjectHypothesisWithPose data;
        data.hypothesis.score = data.score;
        return data;
    }
}  // namespace fast::rf_ros2::utils