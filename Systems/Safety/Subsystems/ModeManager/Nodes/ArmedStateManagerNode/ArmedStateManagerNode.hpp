/**
 * @file ArmedStateManagerNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once
#include <BasicArmedStateManagerProcess.hpp>

#include "robot_framework_ros2/BaseNode.hpp"
namespace fast::rf_ros2::SafetySystem::ModeManagerSubsystem::ArmedStateManager {
    class ArmedStateManagerNode : public BaseNode {
       public:
        ArmedStateManagerNode() : BaseNode("armed_state_manager_node") {}

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
        fast::rf::SafetySystem::ModeManagerSubsystem::ArmedStateManager::BasicArmedStateManagerProcess m_process;
    };
}  // namespace fast::rf_ros2::SafetySystem::ModeManagerSubsystem::ArmedStateManager