#include <gtest/gtest.h>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"

using namespace fast::rf_ros2::utils;

TEST(TestConversion, VisionMessageConvertObjectHypothesisWithPose) {
    {  // Convert to ROS Message
        fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg data;
        auto msg = TranslateUtility::convert(data);
    }

    {  // Convert from ROS Message
        vision_msgs::msg::ObjectHypothesisWithPose data;
        auto msg = TranslateUtility::convert(data);
    }
}