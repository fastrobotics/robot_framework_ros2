
#include <gtest/gtest.h>

#include <Infrastructure/Logger.hpp>
#include <Windows/HeaderWindow.hpp>

using namespace fast::rf_ros::Tools::Applications::SystemMonitor;
TEST(HeaderWindow, BasicTests) {
    HeaderWindow sut(-1, 100, 100);
    ASSERT_NE(sut.getName(), "");
    ASSERT_FALSE(sut.isInitialized());
    ASSERT_FALSE(sut.update(0.0));
    fast::rf::Logger::logDebug(sut.pretty());
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
