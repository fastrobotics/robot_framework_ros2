
#include <gtest/gtest.h>

#include <BaseWindow.hpp>
#include <Infrastructure/Logger.hpp>

using namespace fast::rf_ros::Tools::Applications::SystemMonitor;
class TestBaseWindow : public BaseWindow {
   public:
    TestBaseWindow(int16_t tabOrder, int16_t mainWindowHeight, uint16_t mainWindowWidth)
        : BaseWindow("test_window", tabOrder, 0.0, 0.0, 100.0, 100.0, mainWindowHeight, mainWindowWidth) {
        ScreenCoordinatePixel coord_pix =
            convertCoordinate(getScreenCoordinatesPerc(), mainWindowWidth, mainWindowHeight);
        WINDOW* win = createNewWin(coord_pix.heightPix, coord_pix.widthPix, coord_pix.startYPix, coord_pix.startXPix);
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
    std::string pretty() {
        std::string str = "---Test Base Window---\n";
        str += BaseWindow::pretty();
        return str;
    }
    bool update(double currentTimeSec) {
        bool status = BaseWindow::update(currentTimeSec);
        if (status == false) {
            return false;
        }
        return updateWindow();
    }
    bool updateWindow() { return true; }
};
TEST(TestBaseWindow, BasicTests) {
    TestBaseWindow sut(-1, 0, 0);
    ASSERT_NE(sut.getName(), "");
    ASSERT_FALSE(sut.isInitialized());
    fast::rf::Logger::logDebug(sut.pretty());
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
