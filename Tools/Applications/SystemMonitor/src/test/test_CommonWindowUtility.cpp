
#include <gtest/gtest.h>

#include <CommonWindowUtility.hpp>
using namespace fast::rf_ros::Tools::Applications::SystemMonitor;
TEST(BasicTest, Test_Convert) {
    uint16_t widthPix = 100;
    uint16_t heightPix = 100;
    ScreenCoordinatePerc coordPerc(0.0, 0.0, 100.0, 100.0);
    auto coordPIxel = convertCoordinate(coordPerc, widthPix, heightPix);
    EXPECT_EQ(coordPIxel.startXPix, 0);
    EXPECT_EQ(coordPIxel.startYPix, 0);
    EXPECT_EQ(coordPIxel.widthPix, widthPix);
    EXPECT_EQ(coordPIxel.heightPix, heightPix);
}
TEST(BasicTest, Test_NewWindow) {
    auto window = createNewWin(100, 100, 0, 0);
    EXPECT_TRUE(window == nullptr);  // This won't work in unit tests.
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}