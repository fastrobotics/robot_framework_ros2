
#include <gtest/gtest.h>

#include <Infrastructure/Logger.hpp>
#include <Windows/DiagnosticWindow.hpp>

using namespace fast::rf_ros::Tools::Applications::SystemMonitor;
TEST(DiagnosticWindow, BasicTests) {
    DiagnosticWindow sut(-1, 100, 100);
    ASSERT_NE(sut.getName(), "");
    ASSERT_FALSE(sut.isInitialized());
    ASSERT_FALSE(sut.update(0.0));
    fast::rf::Logger::logDebug(sut.pretty());
}
TEST(DiagnosticWindow, NodeDiagnosticTests) {
    DiagnosticWindow sut(-1, 100, 100);
    ASSERT_NE(sut.getName(), "");
    ASSERT_FALSE(sut.isInitialized());
    ASSERT_FALSE(sut.update(0.0));
    fast::rf::Logger::logDebug(sut.pretty());
    {
        robot_framework_ros2::msg::Diagnostic nodeDiagnostic;
        nodeDiagnostic.NodeName = "node1";
        nodeDiagnostic.DiagnosticType = 1;
        nodeDiagnostic.Level = 2;
        sut.newDiagnosticMsg(nodeDiagnostic);
    }
    {
        robot_framework_ros2::msg::Diagnostic nodeDiagnostic;
        nodeDiagnostic.NodeName = "node1";
        nodeDiagnostic.DiagnosticType = 2;
        nodeDiagnostic.Level = 2;
        sut.newDiagnosticMsg(nodeDiagnostic);
    }
    {
        robot_framework_ros2::msg::Diagnostic nodeDiagnostic;
        nodeDiagnostic.NodeName = "node2";
        nodeDiagnostic.DiagnosticType = 2;
        nodeDiagnostic.Level = 2;
        sut.newDiagnosticMsg(nodeDiagnostic);
    }

    ASSERT_EQ(sut.getNodeDiagnosticMonitors().size(), 2);
    ASSERT_EQ(sut.getNodeDiagnosticMonitors()["node1"].size(), 2);
    ASSERT_EQ(sut.getNodeDiagnosticMonitors()["node2"].size(), 1);

    ASSERT_FALSE(sut.update(1.0));
}
int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
