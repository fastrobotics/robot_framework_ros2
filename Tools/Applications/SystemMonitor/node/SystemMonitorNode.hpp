/**
 * @file SystemMonitorNode.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-09-11
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Header v0.2
 */
#pragma once

#include <curses.h>

#include "IWindow.hpp"
#include "robot_framework_ros2/BaseNode.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    class SystemMonitorNode : public BaseNode {
       public:
        SystemMonitorNode() : BaseNode("system_monitor") {}
        ~SystemMonitorNode() {
            windows.clear();
            endwin();
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
        bool initScreen();
        bool initWindows();
        std::string pretty() override;
        std::map<std::string, std::shared_ptr<IWindow>> windows;
    };
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor