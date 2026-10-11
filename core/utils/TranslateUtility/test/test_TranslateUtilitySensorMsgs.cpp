
#include <gtest/gtest.h>

#include <array>
#include <string>

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
TEST(TestConversion, SensorMsgsConvertPointCloud) {
    fast::rf::messages::SensorMsgs::PointCloudMsg data;
    data.time_stamp = 123.456789;
    data.point_cloud->header.frame_id = "camera_optical_frame";
    data.point_cloud->width = 2;
    data.point_cloud->height = 1;
    data.point_cloud->is_dense = false;
    data.point_cloud->points.resize(2);

    auto& first = data.point_cloud->points[0];
    first.x = 1.25f;
    first.y = -2.5f;
    first.z = 3.75f;
    first.r = 11;
    first.g = 22;
    first.b = 33;
    first.a = 44;

    auto& second = data.point_cloud->points[1];
    second.x = -4.5f;
    second.y = 5.25f;
    second.z = -6.125f;
    second.r = 101;
    second.g = 152;
    second.b = 203;
    second.a = 254;

    const auto rosMsg = TranslateUtility::convert(data);
    EXPECT_EQ(rosMsg.header.stamp.sec, 123);
    EXPECT_EQ(rosMsg.header.stamp.nanosec, 456789000);
    EXPECT_EQ(rosMsg.header.frame_id, "camera_optical_frame");
    EXPECT_EQ(rosMsg.height, 1);
    EXPECT_EQ(rosMsg.width, 2);
    EXPECT_FALSE(rosMsg.is_bigendian);
    EXPECT_EQ(rosMsg.row_step, rosMsg.point_step * rosMsg.width);
    EXPECT_EQ(rosMsg.data.size(), rosMsg.row_step * rosMsg.height);
    EXPECT_FALSE(rosMsg.is_dense);

    ASSERT_EQ(rosMsg.fields.size(), 4);
    const std::array<std::string, 4> expectedFieldNames{"x", "y", "z", "rgb"};
    const std::array<uint32_t, 4> expectedFieldOffsets{0, 4, 8, 16};
    for (std::size_t i = 0; i < rosMsg.fields.size(); ++i) {
        EXPECT_EQ(rosMsg.fields[i].name, expectedFieldNames[i]);
        EXPECT_EQ(rosMsg.fields[i].offset, expectedFieldOffsets[i]);
        EXPECT_EQ(rosMsg.fields[i].datatype, sensor_msgs::msg::PointField::FLOAT32);
        EXPECT_EQ(rosMsg.fields[i].count, 1);
    }

    const auto roundTripped = TranslateUtility::convert(rosMsg);
    ASSERT_NE(roundTripped.point_cloud, nullptr);
    EXPECT_DOUBLE_EQ(roundTripped.time_stamp, data.time_stamp);
    EXPECT_EQ(roundTripped.point_cloud->header.frame_id, "camera_optical_frame");
    EXPECT_EQ(roundTripped.point_cloud->header.stamp, 123456789);
    EXPECT_EQ(roundTripped.point_cloud->width, 2);
    EXPECT_EQ(roundTripped.point_cloud->height, 1);
    EXPECT_FALSE(roundTripped.point_cloud->is_dense);
    ASSERT_EQ(roundTripped.point_cloud->points.size(), 2);

    const auto& convertedFirst = roundTripped.point_cloud->points[0];
    EXPECT_FLOAT_EQ(convertedFirst.x, first.x);
    EXPECT_FLOAT_EQ(convertedFirst.y, first.y);
    EXPECT_FLOAT_EQ(convertedFirst.z, first.z);
    EXPECT_EQ(convertedFirst.r, first.r);
    EXPECT_EQ(convertedFirst.g, first.g);
    EXPECT_EQ(convertedFirst.b, first.b);
    EXPECT_EQ(convertedFirst.a, first.a);

    const auto& convertedSecond = roundTripped.point_cloud->points[1];
    EXPECT_FLOAT_EQ(convertedSecond.x, second.x);
    EXPECT_FLOAT_EQ(convertedSecond.y, second.y);
    EXPECT_FLOAT_EQ(convertedSecond.z, second.z);
    EXPECT_EQ(convertedSecond.r, second.r);
    EXPECT_EQ(convertedSecond.g, second.g);
    EXPECT_EQ(convertedSecond.b, second.b);
    EXPECT_EQ(convertedSecond.a, second.a);
}