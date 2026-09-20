/**
 * @file CommonWindowUtility.hpp
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

#include "Definitions.hpp"

namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    /**
     * @brief Create a newwin object
     *
     * @param height
     * @param width
     * @param startY
     * @param startX
     * @return WINDOW*
     */
    WINDOW* createNewWin(int height, int width, int startY, int startX);
    /**
     * @brief Convert Coordinates
     *
     * @param coordPerc
     * @param widthPix
     * @param heightPix
     * @return ScreenCoordinatePixel
     */
    ScreenCoordinatePixel convertCoordinate(ScreenCoordinatePerc coordPerc, uint16_t widthPix, uint16_t heightPix);
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor
