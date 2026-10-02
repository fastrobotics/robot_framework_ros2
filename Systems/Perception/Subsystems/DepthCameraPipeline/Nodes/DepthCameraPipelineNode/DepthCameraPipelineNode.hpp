/**
 * @file DepthCameraPipelineNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once
#include <BasicSensorFuserProcess/BasicSensorFuserProcess.hpp>
#include <IProcess.hpp>
#include <SensorHealthMonitorProcess.hpp>
#include <SensorInputHandlerProcess.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>

#include "robot_framework_ros2/BaseNode.hpp"
namespace fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem {
    class DepthCameraPipelineNode : public BaseNode {
       public:
        DepthCameraPipelineNode()
            : BaseNode("depthcamera_pipeline_node"),
              m_sensorInputHandlerProcess(std::make_shared<fast::rf::PerceptionSystem::DepthCameraPipelineSubsystem::
                                                               SensorInputHandler::SensorInputHandlerProcess>()),
              m_sensorHealthMonitorProcess(std::make_shared<fast::rf::PerceptionSystem::DepthCameraPipelineSubsystem::
                                                                SensorHealthMonitor::SensorHealthMonitorProcess>()) {
            m_pipeline["input_handler"] = m_sensorInputHandlerProcess;
            m_pipeline["health_monitor"] = m_sensorHealthMonitorProcess;
        }

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
        void pointCloudCallback(const std::string& topicName, const sensor_msgs::msg::PointCloud2::SharedPtr msg) const;
        std::string pretty() override;

        // Pubs & Subs
        rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr m_sensorPointCloubSub;
        // Data
        std::shared_ptr<
            fast::rf::PerceptionSystem::DepthCameraPipelineSubsystem::SensorInputHandler::SensorInputHandlerProcess>
            m_sensorInputHandlerProcess;
        std::shared_ptr<
            fast::rf::PerceptionSystem::DepthCameraPipelineSubsystem::SensorHealthMonitor::SensorHealthMonitorProcess>
            m_sensorHealthMonitorProcess;

        std::unordered_map<std::string, std::shared_ptr<fast::rf::IProcess>> m_pipeline;
    };
}  // namespace fast::rf_ros2::PerceptionSystem::DepthCameraPipelineSubsystem