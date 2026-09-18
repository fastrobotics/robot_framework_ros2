/**
 * @file IMUNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once
#include <IMUProcess.hpp>

#include "robot_framework_ros2/BaseNode.hpp"
// Messages
#include <geometry_msgs/msg/accel_stamped.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/magnetic_field.hpp>
namespace fast::rf_ros2::PoseSystem::InertialSensorSubsystem::IMU {
    class IMUNode : public BaseNode {
       public:
        IMUNode() : BaseNode("imu_node") {}

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
        std::string pretty() override;
        fast::rf::PoseSystem::InertialSensorSubsystem::IMU::IMUProcess m_process;
        std::string m_imuFrame{""};

        // Pubs & Subs
        rclcpp::Publisher<geometry_msgs::msg::AccelStamped>::SharedPtr m_imuAccelPub;
        rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr m_imuPub;
        rclcpp::Publisher<sensor_msgs::msg::MagneticField>::SharedPtr m_imuMagneticPub;

        fast::rf::PoseSystem::InertialSensorSubsystem::IMU::IIMUProcess::IMUConfig m_imuConfig;
    };
}  // namespace fast::rf_ros2::PoseSystem::InertialSensorSubsystem::IMU
