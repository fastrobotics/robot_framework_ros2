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
        std::string paramExampleParameter = "example_parameter";
        this->declare_parameter<double>(paramExampleParameter, -1.0);
        double exampleParameter = this->get_parameter(paramExampleParameter).as_double();
        fast::rf::Logger::logNotice("Example Parameter: " + std::to_string(exampleParameter));
        return true;
    }
    bool IMUNode::initPubSubs() {
        m_imuAccelPub = this->create_publisher<geometry_msgs::msg::AccelStamped>(
            "~/accel", 10);                                                     // fix, should be under a pose namespace
        m_imuPub = this->create_publisher<sensor_msgs::msg::Imu>("~/imu", 10);  // fix, should be under a pose namespace
        m_imuMagneticPub = this->create_publisher<sensor_msgs::msg::MagneticField>(
            "~/magnetic", 10);  // fix, should be under a pose namespace
        return true;
    }
    bool IMUNode::initServices() { return true; }
    bool IMUNode::initDiagnostics() { return true; }
    bool IMUNode::initData() {
        fast::rf::PoseSystem::InertialSensorSubsystem::IMU::IIMUProcess::IMUConfig imu_config;
        // imu_config.highPacketDroppedRateThreshold = highPacketDroppedRateThreshold;
        // imu_config.lowPacketRxRateThreshold = lowPacketRxRateThreshold;
        std::string imuType = "SYDTM151";
        std::string imuDeviceName = "/dev/imu_STMicroelectronics_3435";
        imu_config.imu_type = fast::rf::PoseSystem::InertialSensorSubsystem::IMU::IIMUDriver::convert_name(imuType);
        imu_config.imu_device_name = imuDeviceName;
        // imu_config.linear_accelerometer_covariance = linear_acc_covariance_matrix;
        // imu_config.gyro_covariance = gyro_covariance_matrix;
        // imu_config.magnetometer_covariance = magnetometer_covariance_matrix;
        // imu_config.orientation_covariance = orientation_covariance_matrix;
        bool status = m_process.init(imu_config);
        if (status == false) {
            fast::rf::Logger::logError("Unable to initialize Process with IMU: " + imuType);
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
                imuData.header.frame_id = "todo";
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
                magneticData.header.frame_id = "todo";
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
