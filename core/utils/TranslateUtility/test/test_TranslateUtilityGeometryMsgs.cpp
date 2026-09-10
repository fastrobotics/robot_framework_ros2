#include <gtest/gtest.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
using namespace fast::rf_ros2::utils;
TEST(TestConversion, GeometryMessageConvertAccel) {
    {  // Convert from ROS Message
        geometry_msgs::msg::Accel data;
        data.linear.x = 1.0;
        data.linear.y = 2.0;
        data.linear.z = 3.0;
        data.angular.x = 4.0;
        data.angular.y = 5.0;
        data.angular.z = 6.0;
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.linear.x, 1.0);
        ASSERT_FLOAT_EQ(msg.linear.y, 2.0);
        ASSERT_FLOAT_EQ(msg.linear.z, 3.0);
        ASSERT_FLOAT_EQ(msg.angular.x, 4.0);
        ASSERT_FLOAT_EQ(msg.angular.y, 5.0);
        ASSERT_FLOAT_EQ(msg.angular.z, 6.0);
    }
    {  // Convert to ROS Message
        fast::rf::messages::GeometryMsgs::AccelMsg data;
        data.linear.x = 1.0;
        data.linear.y = 2.0;
        data.linear.z = 3.0;
        data.angular.x = 4.0;
        data.angular.y = 5.0;
        data.angular.z = 6.0;
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.linear.x, 1.0);
        ASSERT_FLOAT_EQ(msg.linear.y, 2.0);
        ASSERT_FLOAT_EQ(msg.linear.z, 3.0);
        ASSERT_FLOAT_EQ(msg.angular.x, 4.0);
        ASSERT_FLOAT_EQ(msg.angular.y, 5.0);
        ASSERT_FLOAT_EQ(msg.angular.z, 6.0);
    }
}
TEST(TestConversion, GeometryMessageConvertAccelWithCovariance) {
    {  // Convert from ROS Message
        geometry_msgs::msg::AccelWithCovarianceStamped data;
        data.accel.accel.linear.x = 1.0;
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.accel.linear.x, data.accel.accel.linear.x);
    }
    {  // Convert to ROS Message
        fast::rf::messages::GeometryMsgs::AccelWithCovarianceMsg data;
        data.time_stamp = 1.0;
        data.accel.linear.x = 2.0;
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.accel.accel.linear.x, data.accel.linear.x);
    }
}
TEST(TestConversion, GeometryMessageConvertQuaternion) {
    {  // Convert from ROS Message
        {
            geometry_msgs::msg::Quaternion data;
            data.w = 1.0;
            data.x = 0.0;
            data.y = 0.0;
            data.z = 0.0;
            auto msg = TranslateUtility::convert(data);
            ASSERT_FLOAT_EQ(msg.roll, 0.0);
            ASSERT_FLOAT_EQ(msg.pitch, 0.0);
            ASSERT_FLOAT_EQ(msg.yaw, 0.0);
        }
        {
            geometry_msgs::msg::Quaternion data;
            data.w = 0.844623198620733;
            data.x = 0.191341716182545;
            data.y = 0.461939766255643;
            data.z = 0.191341716182545;
            auto msg = TranslateUtility::convert(data);
            ASSERT_FLOAT_EQ(msg.roll, 45.0 * M_PI / 180.0);
            ASSERT_FLOAT_EQ(msg.pitch, 45.0 * M_PI / 180.0);
            ASSERT_FLOAT_EQ(msg.yaw, 45.0 * M_PI / 180.0);
        }
    }
    {  // Convert to ROS Message
        {
            fast::rf::messages::GeometryMsgs::OrientationMsg data;
            data.roll = 0.0;
            data.pitch = 0.0;
            data.yaw = 0.0;
            auto msg = TranslateUtility::convert(data);
            ASSERT_FLOAT_EQ(msg.w, 1.0);
            ASSERT_FLOAT_EQ(msg.x, 0.0);
            ASSERT_FLOAT_EQ(msg.y, 0.0);
            ASSERT_FLOAT_EQ(msg.z, 0.0);
        }
        {
            fast::rf::messages::GeometryMsgs::OrientationMsg data;
            data.roll = 45.0 * M_PI / 180.0;
            data.pitch = 45.0 * M_PI / 180.0;
            data.yaw = 45.0 * M_PI / 180.0;
            auto msg = TranslateUtility::convert(data);
            ASSERT_FLOAT_EQ(msg.w, 0.844623198620733);
            ASSERT_FLOAT_EQ(msg.x, 0.191341716182545);
            ASSERT_FLOAT_EQ(msg.y, 0.461939766255643);
            ASSERT_FLOAT_EQ(msg.z, 0.191341716182545);
        }
    }
}
TEST(TestConversion, GeometryMessageConvertTwist) {
    {  // Convert to ROS Message
        fast::rf::messages::GeometryMsgs::TwistMsg twist;
        twist.linear.x = 1.0;
        twist.linear.y = 2.0;
        twist.linear.z = 3.0;
        twist.angular.x = 4.0;
        twist.angular.y = 5.0;
        twist.angular.z = 6.0;
        geometry_msgs::msg::Twist msg = TranslateUtility::convert(twist);
        ASSERT_FLOAT_EQ(msg.linear.x, twist.linear.x);
        ASSERT_FLOAT_EQ(msg.linear.y, twist.linear.y);
        ASSERT_FLOAT_EQ(msg.linear.z, twist.linear.z);
        ASSERT_FLOAT_EQ(msg.angular.x, twist.angular.x);
        ASSERT_FLOAT_EQ(msg.angular.y, twist.angular.y);
        ASSERT_FLOAT_EQ(msg.angular.z, twist.angular.z);
    }

    {  // Convert from ROS Message
        geometry_msgs::msg::Twist twist;
        twist.linear.x = 1.0;
        twist.linear.y = 2.0;
        twist.linear.z = 3.0;
        twist.angular.x = 4.0;
        twist.angular.y = 5.0;
        twist.angular.z = 6.0;
        fast::rf::messages::GeometryMsgs::TwistMsg msg = TranslateUtility::convert(twist);
        ASSERT_FLOAT_EQ(msg.linear.x, twist.linear.x);
        ASSERT_FLOAT_EQ(msg.linear.y, twist.linear.y);
        ASSERT_FLOAT_EQ(msg.linear.z, twist.linear.z);
        ASSERT_FLOAT_EQ(msg.angular.x, twist.angular.x);
        ASSERT_FLOAT_EQ(msg.angular.y, twist.angular.y);
        ASSERT_FLOAT_EQ(msg.angular.z, twist.angular.z);
    }
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}