#include <gtest/gtest.h>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"

using namespace fast::rf_ros2::utils;

TEST(TestConversion, StandardMessageConvertVector) {
    {  // Convert to ROS Message
        fast::rf::messages::StandardMsgs::Vector3DMsg data;
        data.x = 1.0;
        data.y = 2.0;
        data.z = 3.0;
        geometry_msgs::msg::Vector3 msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(msg.x, data.x);
        ASSERT_FLOAT_EQ(msg.y, data.y);
        ASSERT_FLOAT_EQ(msg.z, data.z);
    }

    {  // Convert from ROS Message
        geometry_msgs::msg::Vector3 data;
        data.x = 1.0;
        data.y = 2.0;
        data.z = 3.0;
        fast::rf::messages::StandardMsgs::Vector3DMsg msg = TranslateUtility::convert(data);
        ASSERT_FLOAT_EQ(data.x, msg.x);
        ASSERT_FLOAT_EQ(data.y, msg.y);
        ASSERT_FLOAT_EQ(data.z, msg.z);
    }
}

TEST(TestConversion, StandardMessageConvertCovariance3D) {
    {  // Convert to ROS Message
        fast::rf::messages::StandardMsgs::Covariance3DMsg data;
        ASSERT_EQ(data.covariance.size(), fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                                              fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION);
        for (std::size_t i = 0; i < data.covariance.size(); ++i) {
            data.covariance[i] = (double)i + 1;
        }
        auto msg = TranslateUtility::convert_covariance3D(data);
        ASSERT_EQ(msg.size(), fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                                  fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION);
        for (std::size_t i = 0; i < msg.size(); ++i) {
            ASSERT_EQ(data.covariance[i], msg[i]);
        }
    }
    {  // Convert from ROS Message
        std::array<double, fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                               fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION>
            data;
        ASSERT_EQ(data.size(), fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                                   fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION);
        for (std::size_t i = 0; i < data.size(); ++i) {
            data[i] = (double)i + 1;
        }
        auto msg = TranslateUtility::convert_covariance3D(data);
        ASSERT_EQ(msg.covariance.size(), fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                                             fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION);
        for (std::size_t i = 0; i < msg.covariance.size(); ++i) {
            ASSERT_EQ(data[i], msg.covariance[i]);
        }
    }
}
TEST(TestConversion, StandardMessageConvertCovariance6D) {
    {  // Convert to ROS Message
        fast::rf::messages::StandardMsgs::Covariance6DMsg data;
        ASSERT_EQ(data.covariance.size(), fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                                              fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION);
        for (std::size_t i = 0; i < data.covariance.size(); ++i) {
            data.covariance[i] = (double)i + 1;
        }
        auto msg = TranslateUtility::convert_covariance6D(data);
        ASSERT_EQ(msg.size(), fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                                  fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION);
        for (std::size_t i = 0; i < msg.size(); ++i) {
            ASSERT_EQ(data.covariance[i], msg[i]);
        }
    }
    {  // Convert from ROS Message
        std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                               fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>
            data;
        ASSERT_EQ(data.size(), fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                                   fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION);
        for (std::size_t i = 0; i < data.size(); ++i) {
            data[i] = (double)i + 1;
        }
        auto msg = TranslateUtility::convert_covariance6D(data);
        ASSERT_EQ(msg.covariance.size(), fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                                             fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION);
        for (std::size_t i = 0; i < msg.covariance.size(); ++i) {
            ASSERT_EQ(data[i], msg.covariance[i]);
        }
    }
}