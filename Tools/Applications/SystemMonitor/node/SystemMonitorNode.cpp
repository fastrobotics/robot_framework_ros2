/**
 * @file SystemMonitorNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.3
 *
 */
#include "SystemMonitorNode.hpp"

#include "Windows/HeaderWindow.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    bool SystemMonitorNode::loadConfig() { return true; }
    bool SystemMonitorNode::initPubSubs() { return true; }
    bool SystemMonitorNode::initServices() { return true; }
    bool SystemMonitorNode::initDiagnostics() { return true; }
    bool SystemMonitorNode::initData() {
        if (initScreen() == false) {
            fast::rf::Logger::logError("Unable to initialize Screen!");
            return false;
        }
        return true;
    }
    void SystemMonitorNode::run100Hz() {}
    void SystemMonitorNode::run10Hz() {
        int key_pressed = getch();
        if ((key_pressed == Key::KEY_q) || (key_pressed == Key::KEY_Q)) {
            rclcpp::shutdown();
        }
        // Update all Windows
        for (const auto& window : windows) {
            if (window.second->hasFocus()) {
                auto output = window.second->newKeyEvent(key_pressed);
                if (output.message.level > fast::rf::Level::NOTICE) {
                    fast::rf::Logger::logWarn(output.message.text);
                }
            }
            window.second->update(this->get_clock()->now().seconds());
            /*
            if (window.second->getName() == "node_info_window") {
                auto node_info_window = std::dynamic_pointer_cast<NodeInfoWindow>(window.second);
                m_selectedNode = node_info_window->get_m_selectedNode();
            }
            if (window.second->getName() == "diagnostic_window") {
                auto diagnostic_window = std::dynamic_pointer_cast<DiagnosticWindow>(window.second);
                diagnostic_window->setNodeToMonitor(m_selectedNode);
            }
                */
        }
        flushinp();
    }
    void SystemMonitorNode::run1Hz() {}
    void SystemMonitorNode::run01Hz() {}
    void SystemMonitorNode::run001Hz() {}
    void SystemMonitorNode::runLoop1() {}
    void SystemMonitorNode::runLoop2() {}
    void SystemMonitorNode::runLoop3() {}
    std::string SystemMonitorNode::pretty() {
        std::string str = "\n--- SystemMonitorNode ---\n";
        str += BaseNode::pretty() + "\n";
        return str;
    }
    bool SystemMonitorNode::initScreen() {
        setlocale(LC_ALL, "");
        mousemask(ALL_MOUSE_EVENTS, NULL);
        initscr();
        timeout(0);
        keypad(stdscr, TRUE);
        clear();
        if (has_colors() == FALSE) {
            endwin();
            // logger->enable_consoleprint();
            fast::rf::Logger::logError("Terminal does not support colors. Exiting.");
            return false;
        }
        curs_set(0);
        noecho();
        raw();

        start_color();
        init_color(COLOR_BLACK, 0, 0, 0);
        init_color(COLOR_GREEN, 0, 600, 0);
        init_color(10, 500, 0, 500);
        init_pair((uint8_t)Color::NO_COLOR, COLOR_WHITE, COLOR_BLACK);
        init_pair((uint8_t)Color::WHITE_COLOR, COLOR_BLACK, COLOR_WHITE);
        init_pair((uint8_t)Color::RED_COLOR, COLOR_WHITE, COLOR_RED);
        init_pair((uint8_t)Color::YELLOW_COLOR, COLOR_WHITE, COLOR_YELLOW);
        init_pair((uint8_t)Color::GREEN_COLOR, COLOR_WHITE, COLOR_GREEN);
        init_pair((uint8_t)Color::BLUE_COLOR, COLOR_WHITE, COLOR_BLUE);
        init_pair((uint8_t)Color::PURPLE_COLOR, COLOR_WHITE, 10);

        bool status = initWindows();
        if (status == false) {
            fast::rf::Logger::logError("Unable to initialize Windows!");
            return false;
        }
        return true;
    }
    bool SystemMonitorNode::initWindows() {
        uint16_t mainwindowWidth, mainwindowHeight;
        getmaxyx(stdscr, mainwindowHeight, mainwindowWidth);
        {
            auto window = std::make_shared<HeaderWindow>(-1, mainwindowHeight, mainwindowWidth);
            windows[window->getName()] = window;
            // highest_tab_index++;
        }
        /*
        {
            auto window = std::make_shared<NodeInfoWindow>(-1, mainwindowHeight, mainwindowWidth);
            window->setFocus(true);
            windows[window->getName()] = window;

            // highest_tab_index++;
        }
        {
            auto window = std::make_shared<StatusWindow>(-1, mainwindowHeight, mainwindowWidth);
            windows[window->getName()] = window;
            // highest_tab_index++;
        }
        {
            auto window = std::make_shared<DiagnosticWindow>(-1, mainwindowHeight, mainwindowWidth);
            windows[window->getName()] = window;
            // highest_tab_index++;
        }
        */
        return true;
    }
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor
namespace fast::rf_ros2 {
    std::shared_ptr<BaseNode> BaseNode::createNode() {
        return std::make_shared<fast::rf_ros2::Tools::Applications::SystemMonitor::SystemMonitorNode>();
    }
}  // namespace fast::rf_ros2
