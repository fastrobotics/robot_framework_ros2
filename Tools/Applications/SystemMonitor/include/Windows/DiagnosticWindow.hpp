/**
 * @file DiagnosticWindow.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-08-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include <map>

#include "BaseWindow.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    /**
     * @brief Diagnostic Window
     *
     */
    class DiagnosticWindow : public BaseWindow {
       public:
        class NodeDiagnosticMonitor {
           public:
            robot_framework_ros2::msg::Diagnostic nodeDiagnostic;
            double last_update_sec;
            double last_update_delta_sec;
        };
        enum class DiagnosticWindowMode { UNKNOWN = 0, NODE = 1, DIAGNOSTIC_TYPE = 2, SYSTEM = 3 };
        static constexpr double START_X_PERC =
            66.0; /*!< What percentage of the screen to put top left corner (X) of window. */
        static constexpr double START_Y_PERC =
            15.0; /*!< What percentage of the screen to put top left corner (Y) of window. */
        static constexpr double WIDTH_PERC = 34.5;  /*!< What percentage of the screen (Width) to draw the window. */
        static constexpr double HEIGHT_PERC = 60.0; /*!< What percentage of the screen (Height) to draw the window. */

        DiagnosticWindow(int16_t tabOrder, int16_t mainwindowHeight, uint16_t mainwindowWidth)
            : BaseWindow("diagnostic_window", tabOrder, START_X_PERC, START_Y_PERC, WIDTH_PERC, HEIGHT_PERC,
                         mainwindowHeight, mainwindowWidth) {
            ScreenCoordinatePixel coord_pix =
                convertCoordinate(getScreenCoordinatesPerc(), mainwindowWidth, mainwindowHeight);
            WINDOW* win =
                createNewWin(coord_pix.heightPix, coord_pix.widthPix, coord_pix.startYPix, coord_pix.startXPix);
            setScreenCoordinatesPix(coord_pix);
            setWindow(win);
            std::string header = getWindowHeader();
            mvwprintw(win, 1, 1, header.c_str());
            std::string dashed(getScreenCoordinatesPixel().widthPix - 2, '-');
            mvwprintw(win, 2, 1, dashed.c_str());
            wrefresh(win);
            wrefresh(win);
        }
        KeyEventContainer newKeyEvent([[maybe_unused]] int key) override {
            KeyEventContainer container;
            return container;
        }
        void newArmCommandMsg([[maybe_unused]] robot_framework_ros2::msg::ArmCommand msg) override {}

        void newHeartbeatMsg([[maybe_unused]] robot_framework_ros2::msg::Heartbeat msg) override {}

        void newReadyToArmMsg([[maybe_unused]] robot_framework_ros2::msg::ReadyToArm msg) override {}

        void newDiagnosticMsg(robot_framework_ros2::msg::Diagnostic msg) override;
        /**
         * @brief Human readable string
         *
         * @return std::string
         */
        std::string pretty() override;

        /**
         * @brief Update window at a regular rate
         *
         * @param currentTimeSec
         * @return true
         * @return false
         */
        bool update(double currentTimeSec) override;

        std::map<std::string, std::map<uint8_t, NodeDiagnosticMonitor>> getNodeDiagnosticMonitors() {
            return m_nodeDiagnosticMonitors;
        }
        void setNodeToMonitor(std::string nodeName) { m_nodeToMonitor = nodeName; }

       protected:
        bool updateWindow();

       private:
        DiagnosticWindowMode m_windowMode{DiagnosticWindowMode::NODE};
        std::string getWindowHeader();
        std::map<std::string, std::map<uint8_t, NodeDiagnosticMonitor>> m_nodeDiagnosticMonitors;
        std::string m_nodeToMonitor{""};
    };
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor