
#include <gtest/gtest.h>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"

using namespace fast::rf_ros2::utils;

TEST(TestConversion, InfrastructureMessageConvertDiagnostic) {
    {  // Convert to ROS2 Message
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

    {  // Convert from ROS2 Message
        robot_framework_ros2::msg::Diagnostic msg;
        msg.system_id = 1;
        msg.subsystem_id = 2;
        msg.process_id = 3;
        msg.diagnostic_type = (uint8_t)fast::rf::DiagnosticDefinition::DiagnosticType::SOFTWARE;
        msg.level = (uint8_t)fast::rf::Level::INFO;
        msg.diagnostic_message = (uint8_t)fast::rf::DiagnosticDefinition::DiagnosticMessage::INITIALIZING;
        msg.description = "Test diagnostic_messaging...";
        fast::rf::messages::InfrastructureMsgs::DiagnosticMsg data = TranslateUtility::convert(msg);
        ASSERT_EQ(data.systemID, msg.system_id);
        ASSERT_EQ(data.subsystemID, msg.subsystem_id);
        ASSERT_EQ(data.processID, msg.process_id);
        ASSERT_EQ((uint8_t)data.diagnosticType, msg.diagnostic_type);
        ASSERT_EQ((uint8_t)data.level, msg.level);
        ASSERT_EQ((uint8_t)data.diagnosticMessage, msg.diagnostic_message);
        ASSERT_EQ(data.description, msg.description);
    }
}
TEST(TestConversion, InfrastructureMessageConvertReadyToArm) {
    {  // Convert to ROS2 Message
        fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg data;
        data.systemID = 1;
        data.subsystemID = 2;
        data.processID = 3;
        data.ready_to_arm = true;
        robot_framework_ros2::msg::ReadyToArm msg = TranslateUtility::convert(data);
        ASSERT_EQ(data.systemID, msg.system_id);
        ASSERT_EQ(data.subsystemID, msg.subsystem_id);
        ASSERT_EQ(data.processID, msg.process_id);
        ASSERT_EQ(data.ready_to_arm, msg.ready_to_arm);
    }
    {  // Convert from ROS2 Message
        robot_framework_ros2::msg::ReadyToArm msg;
        msg.system_id = 1;
        msg.subsystem_id = 2;
        msg.process_id = 3;
        msg.ready_to_arm = true;
        fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg data = TranslateUtility::convert(msg);
        ASSERT_EQ(data.systemID, msg.system_id);
        ASSERT_EQ(data.subsystemID, msg.subsystem_id);
        ASSERT_EQ(data.processID, msg.process_id);
        ASSERT_EQ(data.ready_to_arm, msg.ready_to_arm);
    }
}
TEST(TestConversion, InfrastructureMessageConvertArmCommand) {
    {  // Convert to ROS2 Message
        fast::rf::messages::InfrastructureMsgs::ArmCommandMsg data;
        robot_framework_ros2::msg::ArmCommand msg = TranslateUtility::convert(data);
        ASSERT_EQ(msg.armed_state.state, (uint8_t)data.armed_state);
    }
    {  // Convert from ROS2 Message
        robot_framework_ros2::msg::ArmCommand msg;
        fast::rf::messages::InfrastructureMsgs::ArmCommandMsg data = TranslateUtility::convert(msg);
        ASSERT_EQ(msg.armed_state.state, (uint8_t)data.armed_state);
    }
}
TEST(TestConversion, InfrastructureMessageConvertRequestArmStateChange) {
    {  // Convert to ROS2 Service Request
        fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvRequest req;
        req.requested_armed_state = fast::rf::ArmedState::DISARMED;
        robot_framework_ros2::srv::ArmStateChange::Request request = TranslateUtility::convert(req);
        ASSERT_EQ(request.requested_armed_state.state, (uint8_t)req.requested_armed_state);
    }
    {  // Convert from ROS2 Service Request
        robot_framework_ros2::srv::ArmStateChange::Request req;
        req.requested_armed_state.state = robot_framework_ros2::msg::ArmedState::DISARMED;
        fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvRequest request =
            TranslateUtility::convert(req);
        ASSERT_EQ(req.requested_armed_state.state, (uint8_t)request.requested_armed_state);
    }
    {  // Convert to ROS2 Service Response
        fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvResponse resp;
        resp.current_armed_state = fast::rf::ArmedState::DISARMED;
        resp.request_approved = true;
        robot_framework_ros2::srv::ArmStateChange::Response response = TranslateUtility::convert(resp);
        ASSERT_EQ(response.current_armed_state.state, (uint8_t)resp.current_armed_state);
        ASSERT_EQ(response.request_approved, resp.request_approved);
    }
    {  // Convert from ROS2 Service Response
        robot_framework_ros2::srv::ArmStateChange::Response resp;
        resp.current_armed_state.state = robot_framework_ros2::msg::ArmedState::DISARMED;
        resp.request_approved = true;
        fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvResponse response =
            TranslateUtility::convert(resp);
        ASSERT_EQ(resp.current_armed_state.state, (uint8_t)response.current_armed_state);
        ASSERT_EQ(resp.request_approved, response.request_approved);
    }
}
