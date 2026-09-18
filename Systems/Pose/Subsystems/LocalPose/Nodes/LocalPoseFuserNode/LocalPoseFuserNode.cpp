/**
 * @file LocalPoseFuserNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.3
 *
 */
#include "LocalPoseFuserNode.hpp"

#include <robot_framework_ros2/utils/TranslateUtility.hpp>
namespace fast::rf_ros2::PoseSystem::LocalPoseSubsystem::LocalPoseFuser {
    void LocalPoseFuserNode::machineInertialCallback(const sensor_msgs::msg::Imu::SharedPtr msg) const {
        auto localMsg = *msg;
        m_process->new_machine_inertial_data(fast::rf_ros2::utils::TranslateUtility::convert(localMsg));
        fast::rf::messages::GeometryMsgs::OdomMsg local_pose_data;
        fast::rf::messages::GeometryMsgs::AccelWithCovarianceMsg local_pose_angular_accel_data;
        if (m_process->get_local_pose(local_pose_data, local_pose_angular_accel_data) == true) {
            auto local_pose = fast::rf_ros2::utils::TranslateUtility::convert(local_pose_data);
            local_pose.header.frame_id = localMsg.header.frame_id;
            m_localPosePub->publish(local_pose);
            auto local_pose_angular_accel =
                fast::rf_ros2::utils::TranslateUtility::convert(local_pose_angular_accel_data);
            local_pose_angular_accel.header.frame_id = localMsg.header.frame_id;
            m_localPoseAngularAccelPub->publish(local_pose_angular_accel);
        }
    }
    bool LocalPoseFuserNode::loadConfig() { return true; }
    bool LocalPoseFuserNode::initPubSubs() {
        const auto machineInertialTopic = this->declare_parameter<std::string>("machine_inertial_input_topic");
        m_machineInertialSub = this->create_subscription<sensor_msgs::msg::Imu>(
            getNamespacedTopic(machineInertialTopic), 10,
            [this](const sensor_msgs::msg::Imu::SharedPtr msg) { this->machineInertialCallback(msg); });

        const auto localPoseTopic = this->declare_parameter<std::string>("local_pose_output_topic");
        m_localPosePub = this->create_publisher<nav_msgs::msg::Odometry>(getNamespacedTopic(localPoseTopic), 10);

        const auto localPoseAngularAccelTopic =
            this->declare_parameter<std::string>("local_pose_angular_accel_output_topic");
        m_localPoseAngularAccelPub = this->create_publisher<geometry_msgs::msg::AccelWithCovarianceStamped>(
            getNamespacedTopic(localPoseAngularAccelTopic), 10);
        return true;
    }
    bool LocalPoseFuserNode::initServices() { return true; }
    bool LocalPoseFuserNode::initDiagnostics() { return true; }
    bool LocalPoseFuserNode::initData() {
        bool status = m_process->init();
        if (status == false) {
            fast::rf::Logger::logError("Unable to initialize Process!");
            return false;
        }
        m_process->update(this->get_clock()->now().seconds());  // Kick off the Process
        setReadyToArm(m_process->get_ready_to_arm());
        return true;
    }
    void LocalPoseFuserNode::run100Hz() {}
    void LocalPoseFuserNode::run10Hz() { setReadyToArm(m_process->get_ready_to_arm()); }
    void LocalPoseFuserNode::run1Hz() {
        auto diagnostics = m_process->getDiagnostics();
        setDiagnostics(diagnostics);
    }
    void LocalPoseFuserNode::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void LocalPoseFuserNode::run001Hz() {}
    void LocalPoseFuserNode::runLoop1() { m_process->update(this->get_clock()->now().seconds()); }
    void LocalPoseFuserNode::runLoop2() {}
    void LocalPoseFuserNode::runLoop3() {}
    std::string LocalPoseFuserNode::pretty() {
        std::string str = "\n--- LocalPoseFuserNode ---\n";
        str += BaseNode::pretty() + "\n";
        str += m_process->pretty();
        return str;
    }
}  // namespace fast::rf_ros2::PoseSystem::LocalPoseSubsystem::LocalPoseFuser
namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<fast::rf_ros2::PoseSystem::LocalPoseSubsystem::LocalPoseFuser::LocalPoseFuserNode>();
    }
}  // namespace fast::rf_ros2
