
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf2/LinearMath/Quaternion.h>

#include <rclcpp/rclcpp.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include "robot_framework_ros2/utils/TranslateUtility.hpp"
namespace fast::rf_ros2::utils {
    fast::rf::messages::GeometryMsgs::AccelMsg TranslateUtility::convert(const geometry_msgs::msg::Accel& msg) {
        fast::rf::messages::GeometryMsgs::AccelMsg data;
        data.linear = convert(msg.linear);
        data.angular = convert(msg.angular);
        return data;
    }
    geometry_msgs::msg::Accel TranslateUtility::convert(fast::rf::messages::GeometryMsgs::AccelMsg msg) {
        geometry_msgs::msg::Accel data;
        data.linear = convert(msg.linear);
        data.angular = convert(msg.angular);
        return data;
    }
    fast::rf::messages::GeometryMsgs::AccelWithCovarianceMsg TranslateUtility::convert(
        geometry_msgs::msg::AccelWithCovarianceStamped msg) {
        fast::rf::messages::GeometryMsgs::AccelWithCovarianceMsg data;
        data.time_stamp = rclcpp::Time(msg.header.stamp).seconds();
        data.accel = convert(msg.accel.accel);
        data.covariance = convert_covariance6D(msg.accel.covariance);
        return data;
    }
    geometry_msgs::msg::AccelWithCovarianceStamped TranslateUtility::convert(
        fast::rf::messages::GeometryMsgs::AccelWithCovarianceMsg msg) {
        geometry_msgs::msg::AccelWithCovarianceStamped data;
        data.header.stamp = rclcpp::Time(static_cast<int64_t>(msg.time_stamp * 1e9));
        data.accel.accel = convert(msg.accel);
        data.accel.covariance = convert_covariance6D(msg.covariance);
        return data;
    }
    fast::rf::messages::GeometryMsgs::TwistMsg TranslateUtility::convert(geometry_msgs::msg::Twist msg) {
        fast::rf::messages::GeometryMsgs::TwistMsg data;
        data.linear = convert(msg.linear);
        data.angular = convert(msg.angular);
        return data;
    }
    fast::rf::messages::GeometryMsgs::OrientationMsg TranslateUtility::convert(
        const geometry_msgs::msg::Quaternion& msg) {
        fast::rf::messages::GeometryMsgs::OrientationMsg orientation;
        tf2::Quaternion tf_quat(msg.x, msg.y, msg.z, msg.w);
        tf2::Matrix3x3 matrix(tf_quat);
        double roll, pitch, yaw;
        matrix.getRPY(roll, pitch, yaw);
        orientation.roll = roll;
        orientation.pitch = pitch;
        orientation.yaw = yaw;
        return orientation;
    }
    geometry_msgs::msg::Quaternion TranslateUtility::convert(fast::rf::messages::GeometryMsgs::OrientationMsg msg) {
        tf2::Quaternion tf_quaternion;
        tf_quaternion.setRPY(msg.roll, msg.pitch, msg.yaw);
        geometry_msgs::msg::Quaternion data = tf2::toMsg(tf_quaternion);
        return data;
    }
    geometry_msgs::msg::Twist TranslateUtility::convert(fast::rf::messages::GeometryMsgs::TwistMsg msg) {
        geometry_msgs::msg::Twist data;
        data.linear = convert(msg.linear);
        data.angular = convert(msg.angular);
        return data;
    }

    fast::rf::messages::GeometryMsgs::OdomMsg TranslateUtility::convert(nav_msgs::msg::Odometry msg) {
        fast::rf::messages::GeometryMsgs::OdomMsg data;
        data.time_stamp = rclcpp::Time(msg.header.stamp).seconds();
        // data.pose = convert(msg.pose);  // Translate this and any other attribute during AB#1813
        data.twist.twist = convert(msg.twist.twist);
        return data;
    }
    nav_msgs::msg::Odometry TranslateUtility::convert(fast::rf::messages::GeometryMsgs::OdomMsg msg) {
        nav_msgs::msg::Odometry data;
        data.header.stamp = rclcpp::Time(static_cast<int64_t>(msg.time_stamp * 1e9));
        // data.pose = convert(msg.pose);  // Translate this and any other attribute during AB#1813
        data.twist.twist = convert(msg.twist.twist);
        return data;
    }
}  // namespace fast::rf_ros2::utils