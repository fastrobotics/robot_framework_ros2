
#include <gtest/gtest.h>

#include "robot_framework_ros2/utils/CoreUtility.hpp"

using namespace fast::rf_ros2::utils;

TEST(TestConversion, NodeState) {
    for (uint8_t i = robot_framework_ros2::msg::NodeState::STATE_UNKNOWN;
         i < robot_framework_ros2::msg::NodeState::STATE_END_OF_LIST; ++i) {
        robot_framework_ros2::msg::NodeState sut;
        sut.state = i;
        if ((i == (uint8_t)robot_framework_ros2::msg::NodeState::STATE_UNKNOWN) ||
            (i == (uint8_t)robot_framework_ros2::msg::NodeState::STATE_END_OF_LIST)) {
            ASSERT_EQ(fast::rf_ros2::utils::CoreUtility::pretty(sut), "UNKNOWN");
        } else {
            ASSERT_NE(fast::rf_ros2::utils::CoreUtility::pretty(sut), "UNKNOWN");
        }
    }
}
