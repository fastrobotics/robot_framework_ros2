#include <gtest/gtest.h>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"

using namespace fast::rf_ros2::utils;

TEST(TestConversion, VisionMessageConvertObjectHypothesisWithPose) {
    {  // Convert to ROS Message
        fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg data;
        data.score = 42.0;
        data.classification_id = "abc";
        data.pose.pose.position.x = 1.0;
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.hypothesis.score, data.score);
        ASSERT_EQ(msg.hypothesis.class_id, data.classification_id);
        ASSERT_FLOAT_EQ(msg.pose.pose.position.x, data.pose.pose.position.x);
    }

    {  // Convert from ROS Message
        vision_msgs::msg::ObjectHypothesisWithPose data;
        data.hypothesis.score = 37.0;
        data.pose.pose.position.x = 1.0;
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.score, data.hypothesis.score);
        ASSERT_EQ(msg.classification_id, data.hypothesis.class_id);
        ASSERT_FLOAT_EQ(msg.pose.pose.position.x, data.pose.pose.position.x);
    }
}
TEST(TestConversion, VisionMessageConvertDetection3D) {
    {  // Convert to ROS Message
        fast::rf::messages::VisionMsgs::Detection3DMsg data;
        data.time_stamp = 123.456789;
        data.bbox.center.position.x = 1.0;
        {
            fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg result;
            result.score = 0.85;
            result.classification_id = "core-class";
            result.pose.pose.position.x = 2.0;
            data.results.push_back(result);
        }
        auto msg = TranslateUtility::convert(data);
        ASSERT_EQ(msg.header.stamp.sec, 123);
        EXPECT_EQ(msg.header.stamp.nanosec, 456789000);
        ASSERT_FLOAT_EQ(msg.bbox.center.position.x, data.bbox.center.position.x);
        ASSERT_GT(msg.results.size(), 0);
        ASSERT_EQ(msg.results.size(), data.results.size());
        ASSERT_FLOAT_EQ(msg.results[0].hypothesis.score, data.results[0].score);
        ASSERT_EQ(msg.results[0].hypothesis.class_id, data.results[0].classification_id);
        ASSERT_FLOAT_EQ(msg.results[0].pose.pose.position.x, data.results[0].pose.pose.position.x);
    }

    {  // Convert from ROS Message
        vision_msgs::msg::Detection3D data;
        data.header.stamp.sec = 123;
        data.header.stamp.nanosec = 456789000;
        data.bbox.center.position.x = 1.0;
        {
            vision_msgs::msg::ObjectHypothesisWithPose result;
            result.hypothesis.score = 0.75;
            result.hypothesis.class_id = "ros-class";
            result.pose.pose.position.x = 3.0;
            data.results.push_back(result);
        }
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.time_stamp, 123.456789);
        ASSERT_FLOAT_EQ(msg.bbox.center.position.x, data.bbox.center.position.x);
        ASSERT_GT(msg.results.size(), 0);
        ASSERT_EQ(msg.results.size(), data.results.size());
        ASSERT_FLOAT_EQ(msg.results[0].score, data.results[0].hypothesis.score);
        ASSERT_EQ(msg.results[0].classification_id, data.results[0].hypothesis.class_id);
        ASSERT_FLOAT_EQ(msg.results[0].pose.pose.position.x, data.results[0].pose.pose.position.x);
    }
}