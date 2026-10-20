/**
 * @file TranslateUtility.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-09
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
// Robot Framework Dependencies
#include <AccelMsg.hpp>
#include <AccelWithCovarianceMsg.hpp>
#include <ArmCommandMsg.hpp>
#include <ArmStateChangeSrv.hpp>
#include <BoundingBox3DMsg.hpp>
#include <Covariance3DMsg.hpp>
#include <Covariance6DMsg.hpp>
#include <DiagnosticMsg.hpp>
#include <ImuMsg.hpp>
#include <JoyMsg.hpp>
#include <MagneticFieldMsg.hpp>
#include <ObjectHypothesisWithPoseMsg.hpp>
#include <OdomMsg.hpp>
#include <OrientationMsg.hpp>
#include <PointCloudMsg.hpp>
#include <PoseWithCovarianceMsg.hpp>
#include <ReadyToArmStatusMsg.hpp>
#include <TwistMsg.hpp>
#include <Vector3DMsg.hpp>

// Robot Framework ROS2 Interface Dependencies
#include "robot_framework_ros2/msg/arm_command.hpp"
#include "robot_framework_ros2/msg/diagnostic.hpp"
#include "robot_framework_ros2/msg/ready_to_arm.hpp"
#include "robot_framework_ros2/srv/arm_state_change.hpp"

// ROS2 Interface Dependencies
#include <geometry_msgs/msg/accel.hpp>
#include <geometry_msgs/msg/accel_with_covariance_stamped.hpp>
#include <geometry_msgs/msg/pose_with_covariance.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/vector3.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <sensor_msgs/msg/magnetic_field.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <vision_msgs/msg/bounding_box3_d.hpp>
#include <vision_msgs/msg/object_hypothesis_with_pose.hpp>
// General Dependencies
#include <array>

namespace fast::rf_ros2::utils {
    /**
     * @brief Translation Utilities
     *
     */
    class TranslateUtility {
       public:
        // Standard Messages
        /**
         * @brief Convert from a ROS2 message to a Core message of type Vector3
         *
         * @param msg
         * @return fast::rf::messages::StandardMsgs::Vector3DMsg
         */
        static fast::rf::messages::StandardMsgs::Vector3DMsg convert(geometry_msgs::msg::Vector3 msg);

        /**
         * @brief Convert from a Core message to a ROS2 message of type Vector3
         *
         * @param msg
         * @return geometry_msgs::Vector3
         */
        static geometry_msgs::msg::Vector3 convert(fast::rf::messages::StandardMsgs::Vector3DMsg msg);
        /**
         * @brief Convert from a ROS2 Message to a Core Message of type Covariance3D
         *
         * @param msg
         * @return fast::rf::messages::StandardMsgs::Covariance3DMsg
         */
        static fast::rf::messages::StandardMsgs::Covariance3DMsg convert_covariance3D(
            const std::array<double, fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                                         fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION>& msg);

        /**
         * @brief Convert from a Core Message to a ROS2 Message of type Covariance3D
         *
         * @param msg
         * @return std::array<double, Covariance3DMsg::DIMENSION * Covariance3DMsg::DIMENSION>
         */

        static std::array<double, fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION *
                                      fast::rf::messages::StandardMsgs::Covariance3DMsg::DIMENSION>
        convert_covariance3D(const fast::rf::messages::StandardMsgs::Covariance3DMsg& msg);

        /**
         * @brief Convert from a ROS2 Message to a Core Message of type Covariance6D
         *
         * @param msg
         * @return fast::rf::messages::StandardMsgs::Covariance6DMsg
         */
        static fast::rf::messages::StandardMsgs::Covariance6DMsg convert_covariance6D(
            const std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                                         fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>& msg);

        /**
         * @brief Convert from a Core Message to a ROS2 Message of type Covariance6D
         *
         * @param msg
         * @return std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
         * fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>
         */
        static std::array<double, fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION *
                                      fast::rf::messages::StandardMsgs::Covariance6DMsg::DIMENSION>
        convert_covariance6D(const fast::rf::messages::StandardMsgs::Covariance6DMsg& msg);

        // Infrastructure Messages

        /**
         * @brief Translate from a ROS message to a Core message of type Diagnostic
         *
         * @param msg
         * @return fast::rf::messages::InfrastructureMsgs::DiagnosticMsg
         */
        static fast::rf::messages::InfrastructureMsgs::DiagnosticMsg convert(robot_framework_ros2::msg::Diagnostic msg);

        /**
         * @brief Translate from a Core Message to a ROS message of type Diagnostic
         *
         * @param msg
         * @return robot_framework_ros::diagnostic
         */
        static robot_framework_ros2::msg::Diagnostic convert(fast::rf::messages::InfrastructureMsgs::DiagnosticMsg msg);

        /**
         * @brief Translate to a Core Message to a ROS2 message of type ReadyToArm
         *
         * @param msg
         * @return fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg
         */
        static fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg convert(
            robot_framework_ros2::msg::ReadyToArm msg);

        /**
         * @brief Translate from a Core Message to a ROS2 message of type ReadyToArm
         *
         * @param msg
         * @return robot_framework_ros2::msg::ReadyToArm
         */
        static robot_framework_ros2::msg::ReadyToArm convert(
            fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg msg);

        /**
         * @brief Translate from a ROS2 message to a Core message of type ArmCommand
         *
         * @param msg
         * @return fast::rf::messages::InfrastructureMsgs::ArmCommandMsg
         */
        static fast::rf::messages::InfrastructureMsgs::ArmCommandMsg convert(robot_framework_ros2::msg::ArmCommand msg);

        /**
         * @brief Translate from a Core Message to a ROS2 message of type ArmCommand
         *
         * @param msg
         * @return robot_framework_ros2::msg::ArmCommand
         */
        static robot_framework_ros2::msg::ArmCommand convert(fast::rf::messages::InfrastructureMsgs::ArmCommandMsg msg);

        /**
         * @brief Translate from a ROS2 Service Request to a Core Service Request of type ArmStateChange
         *
         * @param req
         * @return fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvRequest
         */
        static fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvRequest convert(
            robot_framework_ros2::srv::ArmStateChange::Request req);

        /**
         * @brief Translate from a Core Service Request to a ROS2 Service Request of type ArmStateChange
         *
         * @param req
         * @return robot_framework_ros2::srv::ArmStateChange::Request
         */
        static robot_framework_ros2::srv::ArmStateChange::Request convert(
            fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvRequest req);

        /**
         * @brief Translate from a ROS2 Service Response to a Core Service Response of type ArmStateChange
         *
         * @param resp
         * @return fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvResponse
         */
        static fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvResponse convert(
            robot_framework_ros2::srv::ArmStateChange::Response resp);

        /**
         * @brief Translate from a Core Service Request to a ROS2 Service Request of type ArmStateChange
         *
         * @param resp
         * @return robot_framework_ros2::srv::ArmStateChange::Response
         */
        static robot_framework_ros2::srv::ArmStateChange::Response convert(
            fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvResponse resp);

        // Geometry Messages
        /**
         * @brief Translate from a ROS2 Message to a Core Message of type Accel
         *
         * @param msg
         * @return fast::rf::messages::GeometryMsgs::AccelMsg
         */
        static fast::rf::messages::GeometryMsgs::AccelMsg convert(const geometry_msgs::msg::Accel& msg);

        /**
         * @brief  Translate from a Core Message to a ROS2 Message of type Accel
         *
         * @param msg
         * @return geometry_msgs::msg::Accel
         */
        static geometry_msgs::msg::Accel convert(fast::rf::messages::GeometryMsgs::AccelMsg msg);

        /**
         * @brief Translate from a ROS2 Message to a Core Message of type AccelWithCovariance
         *
         * @param msg
         * @return fast::rf::messages::GeometryMsgs::AccelWithCovarianceMsg
         */
        static fast::rf::messages::GeometryMsgs::AccelWithCovarianceMsg convert(
            geometry_msgs::msg::AccelWithCovarianceStamped msg);

        /**
         * @brief Translate from a Core Message to a ROS2 Message of type AccelWithCovarianceStamped
         *
         * @param msg
         * @return geometry_msgs::msg::AccelWithCovarianceStamped
         */
        static geometry_msgs::msg::AccelWithCovarianceStamped convert(
            fast::rf::messages::GeometryMsgs::AccelWithCovarianceMsg msg);

        /**
         * @brief Convert from a ROS2 message to a Core message of type Quaternion
         *
         * @param msg
         * @return fast::rf::messages::GeometryMsgs::OrientationMsg
         */
        static fast::rf::messages::GeometryMsgs::OrientationMsg convert(const geometry_msgs::msg::Quaternion& msg);

        /**
         * @brief Convert from a Core Message to a ROS2 message of type Quaternion
         *
         * @param msg
         * @return geometry_msgs::msg::Quaternion
         */
        static geometry_msgs::msg::Quaternion convert(fast::rf::messages::GeometryMsgs::OrientationMsg msg);

        /**
         * @brief Translate from a ROS2 mesage to a Core message of type Twist
         *
         * @param msg
         * @return fast::rf::messages::GeometryMsgs::TwistMsg
         */
        static fast::rf::messages::GeometryMsgs::TwistMsg convert(geometry_msgs::msg::Twist msg);

        /**
         * @brief Convert from a Core message to a ROS2 message of type Twist
         *
         * @param msg
         * @return geometry_msgs::msg::Twist
         */
        static geometry_msgs::msg::Twist convert(fast::rf::messages::GeometryMsgs::TwistMsg msg);

        /**
         * @brief Convert from a ROS2 message to a Core message of type Point
         *
         * @param msg
         * @return fast::rf::messages::GeometryMsgs::PointMsg
         */
        static fast::rf::messages::GeometryMsgs::PointMsg convert(geometry_msgs::msg::Point msg);

        /**
         * @brief Convert from a Core message to a ROS2 message of type Point
         *
         * @param msg
         * @return geometry_msgs::msg::Point
         */
        static geometry_msgs::msg::Point convert(fast::rf::messages::GeometryMsgs::PointMsg msg);

        /**
         * @brief Convert from a ROS2 message to a Core message of type Pose
         *
         * @param msg
         * @return fast::rf::messages::GeometryMsgs::PoseMsg
         */
        static fast::rf::messages::GeometryMsgs::PoseMsg convert(geometry_msgs::msg::Pose msg);

        /**
         * @brief Convert from a Core message to a ROS2 message of type Pose
         *
         * @param msg
         * @return geometry_msgs::msg::Pose
         */
        static geometry_msgs::msg::Pose convert(fast::rf::messages::GeometryMsgs::PoseMsg msg);

        /**
         * @brief Convert from a ROS2 message to a ROS2 message of type PoseWithCovariance
         *
         * @param msg
         * @return fast::rf::messages::GeometryMsgs::PoseWithCovarianceMsg
         */
        static fast::rf::messages::GeometryMsgs::PoseWithCovarianceMsg convert(
            geometry_msgs::msg::PoseWithCovariance msg);

        /**
         * @brief Convert from a Core message to a ROS2 message of type PoseWithCovariance
         *
         * @param msg
         * @return geometry_msgs::msg::PoseWithCovariance
         */
        static geometry_msgs::msg::PoseWithCovariance convert(
            fast::rf::messages::GeometryMsgs::PoseWithCovarianceMsg msg);
        /**
         * @brief Translate from a ROS2 mesage to a Core message of type Odometry
         *
         * @param msg
         * @return fast::rf::messages::GeometryMsgs::OdomMsg
         */
        static fast::rf::messages::GeometryMsgs::OdomMsg convert(nav_msgs::msg::Odometry msg);

        /**
         * @brief Convert from a Core message to a ROS2 message of type Odometry
         *
         * @param msg
         * @return nav_msgs::msg::Odometry
         */
        static nav_msgs::msg::Odometry convert(fast::rf::messages::GeometryMsgs::OdomMsg msg);

        /**
         * @brief Convert from a ROS2 message to a Core message of type BoundingBox3D
         *
         * @param msg
         * @return fast::rf::messages::GeometryMsgs::BoundingBox3DMsg
         */
        static fast::rf::messages::GeometryMsgs::BoundingBox3DMsg convert(vision_msgs::msg::BoundingBox3D msg);

        /**
         * @brief Convert from a Core message to a ROS2 message of type BoundingBox3D
         *
         * @param msg
         * @return vision_msgs::msg::BoundingBox3D
         */
        static vision_msgs::msg::BoundingBox3D convert(fast::rf::messages::GeometryMsgs::BoundingBox3DMsg msg);

        // Sensor Messages

        /**
         * @brief Convert from a ROS2 message to a Core message of type Joy
         *
         * @param msg
         * @return fast::rf::messages::SensorMsgs::JoyMsg
         */
        static fast::rf::messages::SensorMsgs::JoyMsg convert(sensor_msgs::msg::Joy msg);

        /**
         * @brief Convert from a Core message to a ROS2 message of type Joy
         *
         * @param msg
         * @return sensor_msgs::msg::Joy
         */
        static sensor_msgs::msg::Joy convert(fast::rf::messages::SensorMsgs::JoyMsg msg);

        /**
         * @brief Convert from a ROS2 message to a Core message of type Imu
         *
         * @param data
         * @return fast::rf::messages::SensorMsgs::ImuMsg
         */
        static fast::rf::messages::SensorMsgs::ImuMsg convert(sensor_msgs::msg::Imu data);

        /**
         * @brief Convert from a Core message to a ROS2 message of type IMU
         *
         * @param data
         * @return sensor_msgs::msg::Imu
         */
        static sensor_msgs::msg::Imu convert(fast::rf::messages::SensorMsgs::ImuMsg data);

        /**
         * @brief Convert from a ROS2 message to a Core message of type MagneticField
         *
         * @param data
         * @return fast::rf::messages::SensorMsgs::MagneticFieldMsg
         */
        static fast::rf::messages::SensorMsgs::MagneticFieldMsg convert(sensor_msgs::msg::MagneticField data);

        /**
         * @brief Convert from a Core message to a ROS2 message of type MagneticField
         *
         * @param data
         * @return sensor_msgs::msg::MagneticField
         */
        static sensor_msgs::msg::MagneticField convert(fast::rf::messages::SensorMsgs::MagneticFieldMsg data);

        /**
         * @brief Convert from a ROS2 message to a Core message of type PointCloud
         *
         * @param data
         * @return fast::rf::messages::SensorMsgs::PointCloudMsg
         */
        static fast::rf::messages::SensorMsgs::PointCloudMsg convert(const sensor_msgs::msg::PointCloud2& data);

        /**
         * @brief Convert from a Core message to a ROS2 message of type PointCloud
         *
         * @param data
         * @return sensor_msgs::msg::PointCloud2
         */
        static sensor_msgs::msg::PointCloud2 convert(const fast::rf::messages::SensorMsgs::PointCloudMsg& data);

        // Vision Messages

        static fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg convert(
            vision_msgs::msg::ObjectHypothesisWithPose msg);

        static vision_msgs::msg::ObjectHypothesisWithPose convert(
            fast::rf::messages::VisionMsgs::ObjectHypothesisWithPoseMsg msg);
    };
}  // namespace fast::rf_ros2::utils