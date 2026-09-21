/**
 * @file BaseWindow.hpp
 * @author David Gitz (davidgitz@gmail.com)
 * @brief
 * @version 0.1
 * @date 2026-08-13
 *
 * @copyright Copyright (c) 2026
 *
 */

#pragma once
#include <curses.h>

#include <Infrastructure/Logger.hpp>

#include "CommonWindowUtility.hpp"
#include "Definitions.hpp"
#include "IWindow.hpp"
#include "robot_framework_ros2/utils/CoreUtility.hpp"
#include "robot_framework_ros2/utils/TranslateUtility.hpp"

namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    /**
     * @brief Abstract Base Window
     *
     */
    class BaseWindow : public IWindow {
       public:
        /**
         * @brief Construct a new Base Window object
         *
         * @param name
         * @param tabOrder
         * @param startXPerc
         * @param startYPerc
         * @param widthPerc
         * @param heightPerc
         * @param mainWindowHeight
         * @param mainWindowWidth
         */
        BaseWindow(const std::string name, int16_t tabOrder, double startXPerc, double startYPerc, double widthPerc,
                   double heightPerc, uint16_t mainWindowHeight, uint16_t mainWindowWidth)
            : m_name(name),
              m_tabOrder(tabOrder),
              m_screenCoordPerc(startXPerc, startYPerc, widthPerc, heightPerc),
              m_screenCoordPixel(0, 0, 0, 0),
              m_mainWindowHeight(mainWindowHeight),
              m_mainWindowWidth(mainWindowWidth) {}
        virtual ~BaseWindow() {}
        /**
         * @brief Check if initialized
         *
         * @return true
         * @return false
         */
        bool isInitialized() override;
        /**
         * @brief Get the window name
         *
         * @return std::string
         */
        std::string getName() override { return m_name; }

        /**
         * @brief Check if the window has focus
         *
         * @return true
         * @return false
         */
        bool hasFocus() override { return m_focused; }

        /**
         * @brief Set the Window's focus
         *
         * @param cmdFocus
         * @return true
         * @return false
         */
        bool setFocus(bool cmdFocus) override {
            m_focused = cmdFocus;
            return true;
        }

       protected:
        /**
         * @brief Abstract Periodic Update
         *
         * @param currentTimeSec
         * @return true
         * @return false
         */
        bool update(double currentTimeSec);

        double getCurrentTimeSec() { return m_currentTimeSec; }
        /**
         * @brief Update the window details
         *
         * @return true
         * @return false
         */
        virtual bool updateWindow() = 0;
        /**
         * @brief Get base object pretty data
         *
         * @return std::string
         */
        std::string pretty() override = 0;
        /**
         * @brief Set the window
         *
         * @param win
         */
        void setWindow(WINDOW* win) { m_win = win; }
        /**
         * @brief Get the window object
         *
         * @return WINDOW*
         */
        WINDOW* getWindow() { return m_win; }
        /**
         * @brief Set the screen coordinates pix object
         *
         * @param coord
         */
        void setScreenCoordinatesPix(ScreenCoordinatePixel coord) { m_screenCoordPixel = coord; }
        /**
         * @brief Get the screen coordinates perc object
         *
         * @return ScreenCoordinatePerc
         */
        ScreenCoordinatePerc getScreenCoordinatesPerc() { return m_screenCoordPerc; }
        /**
         * @brief Get the screen coordinates pixel object
         *
         * @return ScreenCoordinatePixel
         */
        ScreenCoordinatePixel getScreenCoordinatesPixel() { return m_screenCoordPixel; }
        /**
         * @brief Get the tab order
         *
         * @return int16_t
         */
        int16_t getTabOrder() { return m_tabOrder; }

        int16_t getSelectedRecord() { return m_recordSelected; }

        void updateRecordCount(uint16_t count);
        uint16_t getMainWindowHeight() { return m_mainWindowHeight; }
        uint16_t getMainWindowWidth() { return m_mainWindowWidth; }
        void decrementSelectedRecord();
        void incrementSelectedRecord();
        bool getFocused() { return m_focused; }
        std::vector<int> supportedKeys;

       private:
        WINDOW* m_win{nullptr};
        std::string m_name{""};
        int16_t m_tabOrder;
        ScreenCoordinatePerc m_screenCoordPerc;
        ScreenCoordinatePixel m_screenCoordPixel;
        uint16_t m_mainWindowHeight;
        uint16_t m_mainWindowWidth;
        bool m_focused{false};

        double m_currentTimeSec;
        int16_t m_recordSelected{0};
        uint16_t m_recordCount{0};
    };

}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor