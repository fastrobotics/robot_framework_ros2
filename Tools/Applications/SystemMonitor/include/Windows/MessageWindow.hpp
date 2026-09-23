/**
 * @file MessageWindow.hpp
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
     * @brief Message Window
     *
     */
    class MessageWindow : public BaseWindow {
       public:
        static constexpr double TIME_TO_SHOW_MESSAGES = 10.0f;  // Seconds
        static constexpr double START_X_PERC =
            0.0; /*!< What percentage of the screen to put top left corner (X) of window. */
        static constexpr double START_Y_PERC =
            68.0; /*!< What percentage of the screen to put top left corner (Y) of window. */
        static constexpr double WIDTH_PERC = 100.0; /*!< What percentage of the screen (Width) to draw the window. */
        static constexpr double HEIGHT_PERC = 7.0;  /*!< What percentage of the screen (Height) to draw the window. */
        MessageWindow(std::shared_ptr<rclcpp::Node> node, int16_t tabOrder, int16_t mainWindowHeight,
                      uint16_t mainWindowWidth)
            : BaseWindow(node, "message_window", tabOrder, START_X_PERC, START_Y_PERC, WIDTH_PERC, HEIGHT_PERC,
                         mainWindowHeight, mainWindowWidth) {
            ScreenCoordinatePixel coordPix =
                convertCoordinate(getScreenCoordinatesPerc(), mainWindowWidth, mainWindowHeight);
            WINDOW* win = createNewWin(coordPix.heightPix, coordPix.widthPix, coordPix.startYPix, coordPix.startXPix);
            setScreenCoordinatesPix(coordPix);
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

        bool new_MessageTextList(std::vector<MessageText> messages);
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
        void setMessageText(std::string text, fast::rf::Level level);
        void setMessageText(std::string text, Color color);
        double m_timerShowingMessageInWindowSec{0.0};
        std::string m_messageText;
        Color m_messageTextColor{Color::NO_COLOR};
    };
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor