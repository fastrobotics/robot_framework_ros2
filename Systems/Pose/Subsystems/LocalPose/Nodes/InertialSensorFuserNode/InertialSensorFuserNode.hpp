/**
 * @file InertialSensorFuserNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-17
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once
#include <BasicInertialSensorFuserProcess.hpp>

#include "robot_framework_ros2/BaseNode.hpp"

// Messages
#include <sensor_msgs/msg/imu.hpp>
namespace fast::rf_ros2::PoseSystem::LocalPoseSubsystem::InertialSensorFuser {
    class InertialSensorFuserNode : public BaseNode {
       public:
        InertialSensorFuserNode()
            : BaseNode("inertial_sensor_fuser_node"),
              m_process(std::make_shared<fast::rf::PoseSystem::LocalPoseSubsystem::InertialSensorFuser::
                                             BasicInertialSensorFuserProcess>()) {}

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
        void imuCallback(const std::string& topic_name, const sensor_msgs::msg::Imu::SharedPtr msg) const;
        std::string pretty() override;
        std::shared_ptr<fast::rf::PoseSystem::LocalPoseSubsystem::InertialSensorFuser::IInertialSensorFuserProcess>
            m_process;
        // Pubs & Subs
        rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr m_fusedImuPub;
        std::vector<rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr> m_imuSubs;
    };
}  // namespace fast::rf_ros2::PoseSystem::LocalPoseSubsystem::InertialSensorFuser