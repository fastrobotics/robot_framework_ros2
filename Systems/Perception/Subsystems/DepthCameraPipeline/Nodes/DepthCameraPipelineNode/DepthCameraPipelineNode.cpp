/**
 * @file DepthCameraPipelineNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.3
 *
 */
#include "DepthCameraPipelineNode.hpp"

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem {
    void DepthCameraPipelineNode::pointCloudCallback(const std::string& topicName,
                                                     const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
        auto localMsg = *msg;
        auto convertedMsg = fast::rf_ros2::utils::TranslateUtility::convert(localMsg);
        m_subsystem.newPointCloud(convertedMsg, topicName);
    }
    bool DepthCameraPipelineNode::loadConfig() { return true; }
    bool DepthCameraPipelineNode::initPubSubs() {
        const auto sensor1InputTopic = this->declare_parameter<std::string>("sensor1_depthcamera_topic");
        m_subsystem.addSignalToMonitor(sensor1InputTopic, "sensor_msgs/msg/PointCloud2", 20.0,
                                       50.0);  // TODO: Make these config
        m_sensorPointCloubSub = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            getNamespacedTopic(sensor1InputTopic), 10,
            [this, sensor1InputTopic](const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
                this->pointCloudCallback(sensor1InputTopic, msg);
            });
        return true;
    }
    bool DepthCameraPipelineNode::initServices() { return true; }
    bool DepthCameraPipelineNode::initDiagnostics() { return true; }
    bool DepthCameraPipelineNode::initData() {
        setReadyToArm(m_subsystem.get_ready_to_arm());
        return true;
    }
    void DepthCameraPipelineNode::run100Hz() {}
    void DepthCameraPipelineNode::run10Hz() { setReadyToArm(m_subsystem.get_ready_to_arm()); }
    void DepthCameraPipelineNode::run1Hz() { setDiagnostics(m_subsystem.getDiagnostics()); }
    void DepthCameraPipelineNode::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void DepthCameraPipelineNode::run001Hz() {}
    void DepthCameraPipelineNode::runLoop1() { m_subsystem.update(this->get_clock()->now().seconds()); }
    void DepthCameraPipelineNode::runLoop2() {}
    void DepthCameraPipelineNode::runLoop3() {}
    std::string DepthCameraPipelineNode::pretty() {
        std::string str = "\n--- DepthCameraPipelineNode ---\n";
        str += BaseNode::pretty() + "\n";
        str += m_subsystem.pretty();
        return str;
    }
}  // namespace fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem
namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<
            fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem::DepthCameraPipelineNode>();
    }
}  // namespace fast::rf_ros2
