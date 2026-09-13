/**
 * @file ExampleNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
// You must type out the path starting from the root folder:
#include <IMUProcess.hpp>

#include "robot_framework_ros2/BaseNode.hpp"
namespace fast::rf_ros2 {
    class ExampleNode : public BaseNode {
       public:
        ExampleNode() : BaseNode("example_node") {}

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
        fast::rf::PoseSystem::InertialSensorSubsystem::IMU::IMUProcess process;
    };
}  // namespace fast::rf_ros2