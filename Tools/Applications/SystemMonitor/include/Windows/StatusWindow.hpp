/**
 * @file StatusWindow.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-08-13
 *
 * @copyright Copyright (c) 2026
 *
 */
#pragma once
#include "BaseWindow.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    /**
     * @brief Status Window
     *
     */
    class StatusWindow : public BaseWindow {
       public:
        static constexpr double START_X_PERC =
            0.0; /*!< What percentage of the screen to put top left corner (X) of window. */
        static constexpr double START_Y_PERC =
            80.0; /*!< What percentage of the screen to put top left corner (Y) of window. */
        static constexpr double WIDTH_PERC = 30.0;  /*!< What percentage of the screen (Width) to draw the window. */
        static constexpr double HEIGHT_PERC = 20.0; /*!< What percentage of the screen (Height) to draw the window. */
        StatusWindow(int16_t tabOrder, int16_t mainWindowHeight, uint16_t mainWindowWidth)
            : BaseWindow("status_window", tabOrder, START_X_PERC, START_Y_PERC, WIDTH_PERC, HEIGHT_PERC,
                         mainWindowHeight, mainWindowWidth) {
            ScreenCoordinatePixel coord_pix =
                convertCoordinate(getScreenCoordinatesPerc(), mainWindowWidth, mainWindowHeight);
            WINDOW* win =
                createNewWin(coord_pix.heightPix, coord_pix.widthPix, coord_pix.startYPix, coord_pix.startXPix);
            setScreenCoordinatesPix(coord_pix);
            setWindow(win);
            wrefresh(win);
        }
        KeyEventContainer newKeyEvent([[maybe_unused]] int key) override {
            KeyEventContainer container;
            return container;
        }
        void newArmCommandMsg([[maybe_unused]] robot_framework_ros2::msg::ArmCommand msg) override {}

        void newHeartbeatMsg([[maybe_unused]] robot_framework_ros2::msg::Heartbeat msg) override {}

        void newReadyToArmMsg([[maybe_unused]] robot_framework_ros2::msg::ReadyToArm msg) override {}

        void newDiagnosticMsg([[maybe_unused]] robot_framework_ros2::msg::Diagnostic msg) override {}
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

       protected:
        bool updateWindow();
    };
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor