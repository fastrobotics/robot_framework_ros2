/**
 * @file LocalPoseFuserNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once
#include <BasicLocalPoseFuserProcess.hpp>
#include <ILocalPoseFuserProcess.hpp>
#include <geometry_msgs/msg/accel_with_covariance_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/imu.hpp>

#include "robot_framework_ros2/BaseNode.hpp"

namespace fast::rf_ros2::PoseSystem::LocalPoseSubsystem::LocalPoseFuser {
    class LocalPoseFuserNode : public BaseNode {
       public:
        LocalPoseFuserNode()
            : BaseNode("local_pose_fuser_node"),
              m_process(std::make_shared<
                        fast::rf::PoseSystem::LocalPoseSubsystem::LocalPoseFuser::BasicLocalPoseFuserProcess>()) {}

       protected:
        void run100Hz() override;
        void run10Hz() override;
        void run1Hz() override;
        void run01Hz() override;
        void run001Hz() override;
        void runLoop1() override;
        void runLoop2() override;
        void runLoop3() override;

        bool loadConfig() override;
        bool initPubSubs() override;
        bool initServices() override;
        bool initDiagnostics() override;
        bool initData() override;

       private:
        void machineInertialCallback(const sensor_msgs::msg::Imu::SharedPtr msg) const;
        std::string pretty() override;
        std::shared_ptr<fast::rf::PoseSystem::LocalPoseSubsystem::LocalPoseFuser::ILocalPoseFuserProcess> m_process;

        // Pubs & Subs
        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr m_machineInertialSub;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr m_localPosePub;
        rclcpp::Publisher<geometry_msgs::msg::AccelWithCovarianceStamped>::SharedPtr m_localPoseAngularAccelPub;
    };
}  // namespace fast::rf_ros2::PoseSystem::LocalPoseSubsystem::LocalPoseFuser