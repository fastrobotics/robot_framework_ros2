

#include <rclcpp/rclcpp.hpp>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2::utils {
    fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg TranslateUtility::convert(
        vision_msgs::msg::ObjectHypothesisWithPose msg) {
        fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg data;
        data.score = msg.hypothesis.score;
        data.classification_id = msg.hypothesis.class_id;
        data.pose = convert(msg.pose);
        return data;
    }

    vision_msgs::msg::ObjectHypothesisWithPose TranslateUtility::convert(
        fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg msg) {
        vision_msgs::msg::ObjectHypothesisWithPose data;
        data.hypothesis.score = msg.score;
        data.hypothesis.class_id = msg.classification_id;
        data.pose = convert(msg.pose);
        return data;
    }
    fast::rf::messages::VisionMsgs::Detection3DMsg TranslateUtility::convert(vision_msgs::msg::Detection3D msg) {
        fast::rf::messages::VisionMsgs::Detection3DMsg data;
        data.time_stamp = rclcpp::Time(msg.header.stamp).seconds();
        data.bbox = convert(msg.bbox);
        data.results.reserve(msg.results.size());
        for (const auto& result : msg.results) {
            data.results.emplace_back(convert(result));
        }
        return data;
    }

    vision_msgs::msg::Detection3D TranslateUtility::convert(fast::rf::messages::VisionMsgs::Detection3DMsg msg) {
        vision_msgs::msg::Detection3D data;
        data.header.stamp = rclcpp::Time(static_cast<int64_t>(msg.time_stamp * 1e9));
        data.bbox = convert(msg.bbox);
        data.results.reserve(msg.results.size());
        for (const auto& result : msg.results) {
            data.results.emplace_back(convert(result));
        }
        return data;
    }
}  // namespace fast::rf_ros2::utils