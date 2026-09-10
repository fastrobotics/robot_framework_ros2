#include <rclcpp/rclcpp.hpp>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2::utils {
    fast::rf::messages::InfrastructureMsgs::DiagnosticMsg TranslateUtility::convert(
        robot_framework_ros2::msg::Diagnostic msg) {
        fast::rf::messages::InfrastructureMsgs::DiagnosticMsg data;
        data.systemID = msg.system_id;
        data.subsystemID = msg.subsystem_id;
        data.processID = msg.process_id;
        data.diagnosticType = (fast::rf::DiagnosticDefinition::DiagnosticType)msg.diagnostic_type;
        data.level = (fast::rf::Level)msg.level;
        data.diagnosticMessage = (fast::rf::DiagnosticDefinition::DiagnosticMessage)msg.diagnostic_message;
        data.description = msg.description;
        return data;
    }

    robot_framework_ros2::msg::Diagnostic TranslateUtility::convert(
        fast::rf::messages::InfrastructureMsgs::DiagnosticMsg data) {
        robot_framework_ros2::msg::Diagnostic msg;
        msg.system_id = data.systemID;
        msg.subsystem_id = data.subsystemID;
        msg.process_id = data.processID;
        msg.diagnostic_type = (uint8_t)data.diagnosticType;
        msg.level = (uint8_t)data.level;
        msg.diagnostic_message = (uint8_t)data.diagnosticMessage;
        msg.description = data.description;
        return msg;
    }
    fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg TranslateUtility::convert(
        robot_framework_ros2::msg::ReadyToArm msg) {
        fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg data;
        data.systemID = msg.system_id;
        data.subsystemID = msg.subsystem_id;
        data.processID = msg.process_id;
        data.ready_to_arm = msg.ready_to_arm;
        return data;
    }
    robot_framework_ros2::msg::ReadyToArm TranslateUtility::convert(
        fast::rf::messages::InfrastructureMsgs::ReadyToArmStatusMsg msg) {
        robot_framework_ros2::msg::ReadyToArm data;
        data.system_id = msg.systemID;
        data.subsystem_id = msg.subsystemID;
        data.process_id = msg.processID;
        data.ready_to_arm = msg.ready_to_arm;
        return data;
    }
    fast::rf::messages::InfrastructureMsgs::ArmCommandMsg TranslateUtility::convert(
        robot_framework_ros2::msg::ArmCommand msg) {
        fast::rf::messages::InfrastructureMsgs::ArmCommandMsg data;
        data.armed_state = (fast::rf::ArmedState)msg.armed_state.state;
        return data;
    }
    robot_framework_ros2::msg::ArmCommand TranslateUtility::convert(
        fast::rf::messages::InfrastructureMsgs::ArmCommandMsg msg) {
        robot_framework_ros2::msg::ArmCommand data;
        data.armed_state.state = (uint8_t)msg.armed_state;
        return data;
    }
    fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvRequest TranslateUtility::convert(
        robot_framework_ros2::srv::ArmStateChange::Request req) {
        fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvRequest request;
        request.requested_armed_state = (fast::rf::ArmedState)req.requested_armed_state.state;

        return request;
    }
    robot_framework_ros2::srv::ArmStateChange::Request TranslateUtility::convert(
        fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvRequest req) {
        robot_framework_ros2::srv::ArmStateChange::Request request;
        request.requested_armed_state.state = (uint8_t)req.requested_armed_state;
        return request;
    }
    fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvResponse TranslateUtility::convert(
        robot_framework_ros2::srv::ArmStateChange::Response resp) {
        fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvResponse response;
        response.current_armed_state = (fast::rf::ArmedState)resp.current_armed_state.state;
        response.request_approved = resp.request_approved;
        return response;
    }
    robot_framework_ros2::srv::ArmStateChange::Response TranslateUtility::convert(
        fast::rf::messages::InfrastructureMsgs::ArmStateChangeSrv::ArmStateChangeSrvResponse resp) {
        robot_framework_ros2::srv::ArmStateChange::Response response;
        response.current_armed_state.state = (uint8_t)resp.current_armed_state;
        response.request_approved = resp.request_approved;
        return response;
    }

}  // namespace fast::rf_ros2::utils
