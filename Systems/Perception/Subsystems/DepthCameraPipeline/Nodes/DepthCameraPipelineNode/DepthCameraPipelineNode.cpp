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
namespace fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem {
    bool DepthCameraPipelineNode::loadConfig() { return true; }
    bool DepthCameraPipelineNode::initPubSubs() { return true; }
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
        // str += m_process.pretty();
        return str;
    }
}  // namespace fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem
namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<
            fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem::DepthCameraPipelineNode>();
    }
}  // namespace fast::rf_ros2
