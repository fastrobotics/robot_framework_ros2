#include <rclcpp/rclcpp.hpp>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2::utils {
    fast::rf::messages::SensorMsgs::JoyMsg TranslateUtility::convert(sensor_msgs::msg::Joy msg) {
        fast::rf::messages::SensorMsgs::JoyMsg joy;
        joy.time_stamp = rclcpp::Time(msg.header.stamp).seconds();
        joy.axes.assign(msg.axes.begin(), msg.axes.end());
        joy.buttons.assign(msg.buttons.begin(), msg.buttons.end());
        return joy;
    }

    sensor_msgs::msg::Joy TranslateUtility::convert(fast::rf::messages::SensorMsgs::JoyMsg msg) {
        sensor_msgs::msg::Joy joy;
        joy.header.stamp = rclcpp::Time(static_cast<int64_t>(msg.time_stamp * 1e9));
        joy.axes.assign(msg.axes.begin(), msg.axes.end());
        joy.buttons.assign(msg.buttons.begin(), msg.buttons.end());
        return joy;
    }
    fast::rf::messages::SensorMsgs::ImuMsg TranslateUtility::convert(sensor_msgs::msg::Imu data) {
        fast::rf::messages::SensorMsgs::ImuMsg msg;
        msg.time_stamp = rclcpp::Time(data.header.stamp).seconds();
        msg.orientation = convert(data.orientation);
        msg.orientation = convert(data.orientation);
        msg.orientation_covariance = convert_covariance3D(data.orientation_covariance);
        msg.angular_velocity = convert(data.angular_velocity);
        msg.angular_velocity_covariance = convert_covariance3D(data.angular_velocity_covariance);
        msg.linear_acceleration = convert(data.linear_acceleration);
        msg.linear_acceleration_covariance = convert_covariance3D(data.linear_acceleration_covariance);
        return msg;
    }
    sensor_msgs::msg::Imu TranslateUtility::convert(fast::rf::messages::SensorMsgs::ImuMsg data) {
        sensor_msgs::msg::Imu msg;
        if (data.time_stamp < 0.0) {
            return msg;
        }
        msg.header.stamp = rclcpp::Time(static_cast<int64_t>(data.time_stamp * 1e9));
        msg.orientation = convert(data.orientation);
        msg.orientation_covariance = convert_covariance3D(data.orientation_covariance);
        msg.angular_velocity = convert(data.angular_velocity);
        msg.angular_velocity_covariance = convert_covariance3D(data.angular_velocity_covariance);
        msg.linear_acceleration = convert(data.linear_acceleration);
        msg.linear_acceleration_covariance = convert_covariance3D(data.linear_acceleration_covariance);
        return msg;
    }
    fast::rf::messages::SensorMsgs::MagneticFieldMsg TranslateUtility::convert(sensor_msgs::msg::MagneticField data) {
        fast::rf::messages::SensorMsgs::MagneticFieldMsg msg;
        msg.time_stamp = rclcpp::Time(data.header.stamp).seconds();
        msg.magnetic_field = convert(data.magnetic_field);
        msg.magnetic_field_covariance = convert_covariance3D(data.magnetic_field_covariance);
        return msg;
    }
    sensor_msgs::msg::MagneticField TranslateUtility::convert(fast::rf::messages::SensorMsgs::MagneticFieldMsg data) {
        sensor_msgs::msg::MagneticField msg;
        if (data.time_stamp < 0.0) {
            return msg;
        }
        msg.header.stamp = rclcpp::Time(static_cast<int64_t>(data.time_stamp * 1e9));
        msg.magnetic_field = convert(data.magnetic_field);
        msg.magnetic_field_covariance = convert_covariance3D(data.magnetic_field_covariance);
        return msg;
    }
    fast::rf::messages::SensorMsgs::PointFieldMsg TranslateUtility::convert(sensor_msgs::msg::PointField data) {
        fast::rf::messages::SensorMsgs::PointFieldMsg msg;
        msg.name = data.name;
        msg.offset = data.offset;
        msg.datatype = (fast::rf::messages::SensorMsgs::PointFieldMsg::PointFieldDataType)data.datatype;
        msg.count = data.count;
        return msg;
    }

    sensor_msgs::msg::PointField TranslateUtility::convert(fast::rf::messages::SensorMsgs::PointFieldMsg data) {
        sensor_msgs::msg::PointField msg;
        msg.name = data.name;
        msg.offset = data.offset;
        msg.datatype = (uint8_t)data.datatype;
        msg.count = data.count;
        return msg;
    }
    fast::rf::messages::SensorMsgs::PointCloudMsg TranslateUtility::convert(sensor_msgs::msg::PointCloud2 data) {
        fast::rf::messages::SensorMsgs::PointCloudMsg msg;
        msg.time_stamp = rclcpp::Time(data.header.stamp).seconds();
        msg.height = data.height;
        msg.width = data.width;
        msg.fields.reserve(data.fields.size());
        for (auto field : data.fields) {
            msg.fields.push_back(convert(field));
        }

        msg.is_bigendian = data.is_bigendian;
        msg.point_step = data.point_step;
        msg.row_step = data.row_step;
        msg.data.reserve(data.data.size());
        msg.data.assign(data.data.begin(), data.data.end());
        msg.is_dense = data.is_dense;
        return msg;
    }

    sensor_msgs::msg::PointCloud2 TranslateUtility::convert(fast::rf::messages::SensorMsgs::PointCloudMsg data) {
        sensor_msgs::msg::PointCloud2 msg;
        if (data.time_stamp < 0.0) {
            return msg;
        }
        msg.header.stamp = rclcpp::Time(static_cast<int64_t>(data.time_stamp * 1e9));
        msg.height = data.height;
        msg.width = data.width;
        msg.fields.reserve(data.fields.size());
        for (auto field : data.fields) {
            msg.fields.push_back(convert(field));
        }
        msg.is_bigendian = data.is_bigendian;
        msg.point_step = data.point_step;
        msg.row_step = data.row_step;
        msg.data.reserve(data.data.size());
        msg.data.assign(data.data.begin(), data.data.end());
        msg.is_dense = data.is_dense;
        return msg;
    }
}  // namespace fast::rf_ros2::utils