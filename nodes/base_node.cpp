#include "base_node.hpp"

#include <unistd.h>

#include <chrono>
#include <functional>
using namespace std::chrono_literals;
namespace fast::rf_ros2 {
    BaseNode::BaseNode(const std::string& nodeName) : Node(nodeName) {
        // Parameters
        this->declare_parameter<std::string>("verbosity_level", "INFO");

        // Initializers
        std::string verbosityLevel = this->get_parameter("verbosity_level").as_string();
        fast::rf::Level level;
        if (verbosityLevel == "DEBUG") {
            level = fast::rf::Level::DEBUG;
        } else if (verbosityLevel == "INFO") {
            level = fast::rf::Level::INFO;
        } else if (verbosityLevel == "NOTICE") {
            level = fast::rf::Level::NOTICE;
        } else if (verbosityLevel == "WARN") {
            level = fast::rf::Level::WARN;
        } else if (verbosityLevel == "ERROR") {
            level = fast::rf::Level::ERROR;
        } else if (verbosityLevel == "FATAL") {
            level = fast::rf::Level::FATAL;
        } else {
            // TODO: Fail out if invalid
        }

        fast::rf::Logger::init(level, this->get_name());  // TODO: Fail out if invalid
        // Set Properties
        m_baseNodeName = nodeName;
        m_nodeState.state = robot_framework_ros2::msg::NodeState::STATE_UNKNOWN;
        m_heartbeat.hostname = getHostName();
        m_heartbeat.base_nodename = m_baseNodeName;
        m_heartbeat.nodename = this->get_name();
        m_readyToArm.nodename = this->get_name();

        // Timers
        m_timer10Hz = this->create_wall_timer(100ms, std::bind(&BaseNode::timer10HzCallback, this));
        m_timer1Hz = this->create_wall_timer(1s, std::bind(&BaseNode::timer1HzCallback, this));

        // Pubs and Subs
        m_heartbeatPub = this->create_publisher<robot_framework_ros2::msg::Heartbeat>("heartbeat", 10);
        m_readyToArmPub = this->create_publisher<robot_framework_ros2::msg::ReadyToArm>("ready_to_arm", 10);
    }

    void BaseNode::timer10HzCallback() {
        baseRun10Hz();

        this->run10Hz();
    }

    void BaseNode::timer1HzCallback() {
        baseRun1Hz();
        this->run1Hz();
    }
    bool BaseNode::baseRun10Hz() {
        m_heartbeat.timestamp = this->get_clock()->now();
        m_heartbeat.node_state = m_nodeState;
        m_heartbeatPub->publish(m_heartbeat);
        return true;
    }
    bool BaseNode::baseRun1Hz() {
        m_readyToArmPub->publish(m_readyToArm);
        return true;
    }
    // Util Functions
    std::string BaseNode::getHostName() {
        std::string hostname;
        char hostnameBuffer[256];
        if (gethostname(hostnameBuffer, sizeof(hostnameBuffer)) == 0) {
            // Assuming your Heartbeat.msg has a string field (e.g., string hostname)
            hostname = std::string(hostnameBuffer);
        } else {
            hostname = "";
        }
        return hostname;
    }
}  // namespace fast::rf_ros2
int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = fast::rf_ros2::BaseNode::createNode();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
