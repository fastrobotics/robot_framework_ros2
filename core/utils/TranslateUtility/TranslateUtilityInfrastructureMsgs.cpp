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
}  // namespace fast::rf_ros2::utils