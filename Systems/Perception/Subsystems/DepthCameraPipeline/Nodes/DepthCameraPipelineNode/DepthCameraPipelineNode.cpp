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
    void DepthCameraPipelineNode::pointCloudCallback([[maybe_unused]] const std::string& topicName,
                                                     const sensor_msgs::msg::PointCloud2::SharedPtr msg) const {
        auto localMsg = *msg;
        m_sensorInputHandlerProcess->newPointCloud(fast::rf_ros2::utils::TranslateUtility::convert(localMsg));
        // ToDo: Feed rest of Pipeline as needed
    }
    bool DepthCameraPipelineNode::loadConfig() { return true; }
    bool DepthCameraPipelineNode::initPubSubs() {
        const auto sensor1InputTopic = this->declare_parameter<std::string>("sensor1_depthcamera_topic");
        m_sensorPointCloubSub = this->create_subscription<sensor_msgs::msg::PointCloud2>(
            getNamespacedTopic(sensor1InputTopic), 10,
            [this, sensor1InputTopic](const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
                this->pointCloudCallback(sensor1InputTopic, msg);
            });
        fast::rf::Logger::logWarn("Sub: " + sensor1InputTopic);
        return true;
    }
    bool DepthCameraPipelineNode::initServices() { return true; }
    bool DepthCameraPipelineNode::initDiagnostics() { return true; }
    bool DepthCameraPipelineNode::initData() {
        bool readyToArmFlag = true;
        fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg readyToArm;
        for (auto process : m_pipeline) {
            process.second->update(this->get_clock()->now().seconds());
            readyToArm = process.second->get_ready_to_arm();
            if (readyToArm.ready_to_arm == false) {
                readyToArmFlag = false;
            }
        }
        readyToArm.processID = 0;  // Entire Subsystem
        readyToArm.ready_to_arm = readyToArmFlag;
        setReadyToArm(readyToArm);
        return true;
    }
    void DepthCameraPipelineNode::run100Hz() {}
    void DepthCameraPipelineNode::run10Hz() {
        bool readyToArmFlag = true;
        fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg readyToArm;
        for (auto process : m_pipeline) {
            readyToArm = process.second->get_ready_to_arm();
            if (readyToArm.ready_to_arm == false) {
                readyToArmFlag = false;
            }
        }
        readyToArm.ready_to_arm = readyToArmFlag;
        setReadyToArm(readyToArm);
    }
    void DepthCameraPipelineNode::run1Hz() {
        //  auto diagnostics = m_process.getDiagnostics();
        // setDiagnostics(diagnostics);
    }
    void DepthCameraPipelineNode::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void DepthCameraPipelineNode::run001Hz() {}
    void DepthCameraPipelineNode::runLoop1() {
        //  m_process.update(this->get_clock()->now().seconds());
    }
    void DepthCameraPipelineNode::runLoop2() {}
    void DepthCameraPipelineNode::runLoop3() {}
    std::string DepthCameraPipelineNode::pretty() {
        std::string str = "\n--- DepthCameraPipelineNode ---\n";
        str += BaseNode::pretty() + "\n";
        for (auto process : m_pipeline) {
            str += process.second->pretty();
        }
        return str;
    }
}  // namespace fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem
namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<
            fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem::DepthCameraPipelineNode>();
    }
}  // namespace fast::rf_ros2
