/**
 * @file IMUNode.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2026-09-13
 *
 * @copyright Copyright (c) 2026
 * @compare_tag Node-Source v0.1
 *
 */
#include "IMUNode.hpp"

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2 {
    bool IMUNode::loadConfig() {
        // Get Node Config
        m_imuConfig.highPacketDroppedRateThreshold = this->declare_parameter<double>(
            getNodeConfigNamespace() + "." + this->get_name() + ".high_packet_dropped_rate_threshold");
        m_imuConfig.lowPacketRxRateThreshold = this->declare_parameter<double>(
            getNodeConfigNamespace() + "." + this->get_name() + ".low_packet_rx_rate_threshold");

        // Get Sensor Config
        std::string imuName = this->declare_parameter<std::string>("sensor");
        m_imuFrame = this->declare_parameter<std::string>("target_frame");
        std::string imuType = this->declare_parameter<std::string>("info.type");
        m_imuConfig.imu_type = fast::rf::PoseSystem::InertialSensorSubsystem::IMU::IIMUDriver::convert_name(imuType);
        m_imuConfig.imu_device_name = this->declare_parameter<std::string>("info.device_name", "");
        m_imuConfig.linear_accelerometer_covariance.covariance =
            this->declare_parameter<std::vector<double>>("linear_accel_covariance_matrix");
        m_imuConfig.gyro_covariance.covariance = this->declare_parameter<std::vector<double>>("gyro_covariance_matrix");
        m_imuConfig.magnetometer_covariance.covariance =
            this->declare_parameter<std::vector<double>>("magnetic_covariance_matrix");
        m_imuConfig.orientation_covariance.covariance =
            this->declare_parameter<std::vector<double>>("orientation_covariance_matrix");
        return true;
    }
    bool IMUNode::initPubSubs() {
        const auto imuTopic = this->declare_parameter<std::string>("imu_topic");
        const auto accelTopic = this->declare_parameter<std::string>("accel_topic");
        const auto magneticTopic = this->declare_parameter<std::string>("magnetic_topic");
        m_imuAccelPub =
            this->create_publisher<geometry_msgs::msg::AccelStamped>(getRobotNamespace() + "/" + accelTopic, 10);
        m_imuPub = this->create_publisher<sensor_msgs::msg::Imu>(getRobotNamespace() + "/" + imuTopic, 10);
        m_imuMagneticPub =
            this->create_publisher<sensor_msgs::msg::MagneticField>(getRobotNamespace() + "/" + magneticTopic, 10);
        return true;
    }
    bool IMUNode::initServices() { return true; }
    bool IMUNode::initDiagnostics() { return true; }
    bool IMUNode::initData() {
        bool status = m_process.init(m_imuConfig);
        if (status == false) {
            fast::rf::Logger::logError("Unable to initialize Process with IMU: " +
                                       std::to_string((uint8_t)m_imuConfig.imu_type));
            return false;
        }
        m_process.update(this->get_clock()->now().seconds());  // Kick off the Process
        setReadyToArm(m_process.get_ready_to_arm());
        return true;
    }
    void IMUNode::run100Hz() { fast::rf::Logger::logDebug("100 Hz"); }
    void IMUNode::run10Hz() {
        setReadyToArm(m_process.get_ready_to_arm());
        fast::rf::Logger::logDebug("10 Hz");
    }
    void IMUNode::run1Hz() {
        auto diagnostics = m_process.getDiagnostics();
        setDiagnostics(diagnostics);
        fast::rf::Logger::logDebug("1 Hz");
    }
    void IMUNode::run01Hz() { fast::rf::Logger::logInfo(pretty()); }
    void IMUNode::run001Hz() { fast::rf::Logger::logDebug("0.01 Hz"); }
    void IMUNode::runLoop1() {
        m_process.update(this->get_clock()->now().seconds());
        fast::rf::Logger::logDebug("Loop1");
    }
    void IMUNode::runLoop2() {
        {
            fast::rf::messages::SensorMsgs::ImuMsg data;
            if (m_process.get_imu_data(data)) {
                auto imuData = fast::rf_ros2::utils::TranslateUtility::convert(data);
                imuData.header.frame_id = m_imuFrame;
                m_imuPub->publish(imuData);

                geometry_msgs::msg::AccelStamped accel;
                accel.header = imuData.header;
                accel.accel.linear = imuData.linear_acceleration;
                m_imuAccelPub->publish(accel);
            }
        }
        {
            fast::rf::messages::SensorMsgs::MagneticFieldMsg data;
            if (m_process.get_magnetic_data(data)) {
                auto magneticData = fast::rf_ros2::utils::TranslateUtility::convert(data);
                magneticData.header.frame_id = m_imuFrame;
                m_imuMagneticPub->publish(magneticData);
            }
        }
    }
    void IMUNode::runLoop3() { fast::rf::Logger::logDebug("Loop3"); }
    std::string IMUNode::pretty() {
        std::string str = "\n--- IMUNode ---\n";
        str += BaseNode::pretty() + "\n";
        str += m_process.pretty();
        return str;
    }

    std::shared_ptr<BaseNode> BaseNode::createNode() { return std::make_shared<IMUNode>(); }
}  // namespace fast::rf_ros2
