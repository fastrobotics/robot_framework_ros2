
#include <gtest/gtest.h>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"

using namespace fast::rf_ros2::utils;

TEST(TestConversion, SensorMsgsConvertJoy) {
    {  // Convert to ROS Message
        fast::rf::messages::SensorMsgs::JoyMsg joy;
        joy.axes.push_back(1.0);
        joy.buttons.push_back(2);
        auto msg = TranslateUtility::convert(joy);
        ASSERT_EQ(msg.axes.size(), joy.axes.size());
        ASSERT_EQ(msg.buttons.size(), joy.buttons.size());
    }
    {  // Convert from ROS Message
        sensor_msgs::msg::Joy joy;
        joy.axes.push_back(1.0);
        joy.buttons.push_back(2);
        auto msg = TranslateUtility::convert(joy);
        ASSERT_EQ(msg.axes.size(), joy.axes.size());
        ASSERT_EQ(msg.buttons.size(), joy.buttons.size());
    }
}
TEST(TestConversion, SensorMsgsConvertImu) {
    {  // Convert to ROS Message
        fast::rf::messages::SensorMsgs::ImuMsg data;
        data.time_stamp = 1.0;
        data.seq = 2;
        data.orientation.roll = 0.0;
        data.orientation.pitch = 0.0;
        data.orientation.yaw = 0.0;
        auto msg = TranslateUtility::convert(data);
        auto msg_orientation_euler = TranslateUtility::convert(msg.orientation);
        ASSERT_FLOAT_EQ(msg_orientation_euler.roll, data.orientation.roll);
        ASSERT_FLOAT_EQ(msg_orientation_euler.pitch, data.orientation.pitch);
        ASSERT_FLOAT_EQ(msg_orientation_euler.yaw, data.orientation.yaw);
    }
    {  // Convert from ROS Message
        sensor_msgs::msg::Imu data;
        data.orientation.w = 1.0;
        data.orientation.x = 0.0;
        data.orientation.y = 0.0;
        data.orientation.z = 0.0;
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.orientation.roll, 0.0);
        ASSERT_FLOAT_EQ(msg.orientation.pitch, 0.0);
        ASSERT_FLOAT_EQ(msg.orientation.yaw, 0.0);
    }
}
TEST(TestConversion, SensorMsgsConvertMagneticField) {
    {  // Convert to ROS Message
        fast::rf::messages::SensorMsgs::MagneticFieldMsg data;
        data.time_stamp = 1.0;
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.magnetic_field.x, data.magnetic_field.x);
        ASSERT_FLOAT_EQ(msg.magnetic_field.y, data.magnetic_field.y);
        ASSERT_FLOAT_EQ(msg.magnetic_field.z, data.magnetic_field.z);
    }
    {  // Convert from ROS Message
        sensor_msgs::msg::MagneticField data;
        auto msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.magnetic_field.x, data.magnetic_field.x);
        ASSERT_FLOAT_EQ(msg.magnetic_field.y, data.magnetic_field.y);
        ASSERT_FLOAT_EQ(msg.magnetic_field.z, data.magnetic_field.z);
    }
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}