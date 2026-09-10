
#include <gtest/gtest.h>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"

using namespace fast::rf_ros2::utils;

TEST(TestConversion, InfrastructureMessageConvertDiagnostic) {
    {  // Convert to ROS Message
        fast::rf::messages::InfrastructureMsgs::DiagnosticMsg data;
        data.systemID = 1;
        data.subsystemID = 2;
        data.processID = 3;
        data.diagnosticType = fast::rf::DiagnosticDefinition::DiagnosticType::SOFTWARE;
        data.level = fast::rf::Level::INFO;
        data.diagnosticMessage = fast::rf::DiagnosticDefinition::DiagnosticMessage::INITIALIZING;
        data.description = "Testing...";
        robot_framework_ros2::msg::Diagnostic msg = TranslateUtility::convert(data);
        ASSERT_EQ(msg.system_id, data.systemID);
        ASSERT_EQ(msg.subsystem_id, data.subsystemID);
        ASSERT_EQ(msg.process_id, data.processID);
        ASSERT_EQ(msg.diagnostic_type, (uint8_t)data.diagnosticType);
        ASSERT_EQ(msg.level, (uint8_t)data.level);
        ASSERT_EQ(msg.diagnostic_message, (uint8_t)data.diagnosticMessage);
        ASSERT_EQ(msg.description, data.description);
    }

    {  // Convert from ROS Message
        robot_framework_ros2::msg::Diagnostic msg;
        msg.system_id = 1;
        msg.subsystem_id = 2;
        msg.process_id = 3;
        msg.diagnostic_type = (uint8_t)fast::rf::DiagnosticDefinition::DiagnosticType::SOFTWARE;
        msg.level = (uint8_t)fast::rf::Level::INFO;
        msg.diagnostic_message = (uint8_t)fast::rf::DiagnosticDefinition::DiagnosticMessage::INITIALIZING;
        msg.description = "Test diagnostic_messaging...";
        fast::rf::messages::InfrastructureMsgs::DiagnosticMsg data = TranslateUtility::convert(msg);
        ASSERT_EQ(data.systemID, 0);  // msg.system_id);
        ASSERT_EQ(data.subsystemID, msg.subsystem_id);
        ASSERT_EQ(data.processID, msg.process_id);
        ASSERT_EQ((uint8_t)data.diagnosticType, msg.diagnostic_type);
        ASSERT_EQ((uint8_t)data.level, msg.level);
        ASSERT_EQ((uint8_t)data.diagnosticMessage, msg.diagnostic_message);
        ASSERT_EQ(data.description, msg.description);
    }
}