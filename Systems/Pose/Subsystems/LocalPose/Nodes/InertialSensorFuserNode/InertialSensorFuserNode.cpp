/**
 * @file InertialSensorFuserNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.3
 *
 */
#include "InertialSensorFuserNode.hpp"

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2::PoseSystem::LocalPoseSubsystem::InertialSensorFuser {
    void InertialSensorFuserNode::imuCallback([[maybe_unused]] const std::string& topicName,
                                              const sensor_msgs::msg::Imu::SharedPtr msg) const {
        auto localMsg = *msg;
        m_process->new_imu_data(0, fast::rf_ros2::utils::TranslateUtility::convert(localMsg));
        fast::rf::messages::SensorMsgs::ImuMsg data;
        if (m_process->get_machine_inertial_data(data) == true) {
            auto fused_inertial_data = fast::rf_ros2::utils::TranslateUtility::convert(data);
            fused_inertial_data.header.frame_id = localMsg.header.frame_id;
            m_fusedImuPub->publish(fused_inertial_data);
        }
    }
    bool InertialSensorFuserNode::loadConfig() { return true; }
    bool InertialSensorFuserNode::initPubSubs() {
        {
            const auto imuInputTopic = this->declare_parameter<std::string>("imu1_input_topic");
            auto sub = this->create_subscription<sensor_msgs::msg::Imu>(
                getNamespacedTopic(imuInputTopic), 10,
                [this, imuInputTopic](const sensor_msgs::msg::Imu::SharedPtr msg) {
                    this->imuCallback(imuInputTopic, msg);
                });
            m_imuSubs.push_back(sub);
        }
        const auto fusedImuTopic = this->declare_parameter<std::string>("fused_imu_output_topic");
        m_fusedImuPub = this->create_publisher<sensor_msgs::msg::Imu>(getNamespacedTopic(fusedImuTopic), 10);
        return true;
    }
    bool InertialSensorFuserNode::initServices() { return true; }
    bool InertialSensorFuserNode::initDiagnostics() { return true; }
    bool InertialSensorFuserNode::initData() {
        bool status = m_process->init(1);  // Basic Inertial Sensor Fuser requires 1 and only 1 IMU
        if (status == false) {
            fast::rf::Logger::logError("Unable to initialize Process!");
        }
        m_process->update(this->get_clock()->now().seconds());  // Kick off the Process
        setReadyToArm(m_process->get_ready_to_arm());
        return true;
    }
    void InertialSensorFuserNode::run100Hz() {}
    void InertialSensorFuserNode::run10Hz() { setReadyToArm(m_process->get_ready_to_arm()); }
    void InertialSensorFuserNode::run1Hz() {
        auto diagnostics = m_process->getDiagnostics();
        setDiagnostics(diagnostics);
    }
    void InertialSensorFuserNode::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void InertialSensorFuserNode::run001Hz() {}
    void InertialSensorFuserNode::runLoop1() { m_process->update(this->get_clock()->now().seconds()); }
    void InertialSensorFuserNode::runLoop2() {}
    void InertialSensorFuserNode::runLoop3() {}
    std::string InertialSensorFuserNode::pretty() {
        std::string str = "\n--- InertialSensorFuserNode ---\n";
        str += BaseNode::pretty() + "\n";
        str += m_process->pretty();
        return str;
    }

}  // namespace fast::rf_ros2::PoseSystem::LocalPoseSubsystem::InertialSensorFuser

namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<
            fast::rf_ros2::PoseSystem::LocalPoseSubsystem::InertialSensorFuser::InertialSensorFuserNode>();
    }
}  // namespace fast::rf_ros2
