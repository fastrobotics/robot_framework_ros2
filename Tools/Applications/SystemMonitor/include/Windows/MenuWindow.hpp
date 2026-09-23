/**
 * @file MenuWindow.hpp
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
     * @brief Menu Window
     *
     */
    class MenuWindow : public BaseWindow {
        enum class InstructionMode {
            NODE = 0,
        };

       public:
        static constexpr double START_X_PERC =
            30.0; /*!< What percentage of the screen to put top left corner (X) of window. */
        static constexpr double START_Y_PERC =
            75.0; /*!< What percentage of the screen to put top left corner (Y) of window. */
        static constexpr double WIDTH_PERC = 25.0;  /*!< What percentage of the screen (Width) to draw the window. */
        static constexpr double HEIGHT_PERC = 25.0; /*!< What percentage of the screen (Height) to draw the window. */
        MenuWindow(std::shared_ptr<rclcpp::Node> node, int16_t tabOrder, int16_t mainWindowHeight,
                   uint16_t mainWindowWidth)
            : BaseWindow(node, "menu_window", tabOrder, START_X_PERC, START_Y_PERC, WIDTH_PERC, HEIGHT_PERC,
                         mainWindowHeight, mainWindowWidth) {
            ScreenCoordinatePixel coordPix =
                convertCoordinate(getScreenCoordinatesPerc(), mainWindowWidth, mainWindowHeight);
            WINDOW* win = createNewWin(coordPix.heightPix, coordPix.widthPix, coordPix.startYPix, coordPix.startXPix);
            setScreenCoordinatesPix(coordPix);
            setWindow(win);
            wrefresh(win);

            std::string str = "Instructions:";
            keypad(win, TRUE);
            mvwprintw(win, 1, 1, "%s", str.c_str());
            std::string dashed(coordPix.widthPix - 2, '-');
            mvwprintw(win, 2, 1, "%s", dashed.c_str());
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

       private:
        InstructionMode m_instructionMode{InstructionMode::NODE};
    };
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor