#include "CommonWindowUtility.hpp"
namespace fast::rf_ros2::Tools::Applications::SystemMonitor {
    WINDOW* createNewWin(int height, int width, int startY, int startX) {
        WINDOW* localWin;
        localWin = newwin(height, width, startY, startX);
        box(localWin, 0, 0);
        wrefresh(localWin);
        return localWin;
    }
    ScreenCoordinatePixel convertCoordinate(ScreenCoordinatePerc coordPerc, uint16_t widthPix, uint16_t heightPix) {
        ScreenCoordinatePixel coord(0, 0, 0, 0);
        coord.startXPix = (uint16_t)((double)widthPix * (.01 * coordPerc.startXPerc));
        coord.startYPix = (uint16_t)((double)heightPix * (.01 * coordPerc.startYPerc));
        coord.widthPix = (uint16_t)((double)widthPix * (.01 * coordPerc.widthPerc));
        coord.heightPix = (uint16_t)((double)heightPix * (.01 * coordPerc.heightPerc));
        return coord;
    }
}  // namespace fast::rf_ros2::Tools::Applications::SystemMonitor