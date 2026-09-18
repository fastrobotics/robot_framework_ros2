/**
 * @file {{cookiecutter.Node}}.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once
#include <Basic{{cookiecutter.Process}}Process.hpp>

#include "robot_framework_ros2/BaseNode.hpp"
namespace fast::rf_ros2 {
    class {{cookiecutter.Node}} : public BaseNode {
       public:
        {{cookiecutter.Node}}() : BaseNode("{{cookiecutter.NodeBinary}}") {}

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
        fast::rf::{{cookiecutter.System}}System::{{cookiecutter.Subsystem}}Subsystem::{{cookiecutter.Process}}::Basic{{cookiecutter.Process}}Process m_process;
    };
}  // namespace fast::rf_ros2