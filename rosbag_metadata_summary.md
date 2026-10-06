# ROS2 Bag Metadata Summary

📅 **Analysis Generated:** 2026-10-06 16:53:32  
📁 **Bag Path:** `/mnt/usb_storage/datalogs/rosbag2_2026_10_05-23_27_04`  
⚙️ **Storage Plugin:** `mcap`  
📅 **Log Start Time:** 1969-12-31 18:00:00  
⏳ **Duration:** 23301.48 seconds  
🔢 **Total Messages:** 1,253,894  

---

## 🏢 System: Infrastructure

### 🔹 Datatype: `robot_framework_ros2/msg/Heartbeat`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/basemachine/basemachine/hatdriver/servo_hat_node/heartbeat` | 24,475 | `cdr` |
| `/robot/perception/depthcamerapipeline/depthcamera_pipeline_node/heartbeat` | 24,560 | `cdr` |
| `/robot/pose/inertial_sensor/imu/imu_node1/heartbeat` | 24,539 | `cdr` |
| `/robot/pose/localpose/inertialsensorfuser/inertial_sensor_fuser_node/heartbeat` | 24,616 | `cdr` |
| `/robot/pose/localpose/localposefuser/local_pose_fuser_node/heartbeat` | 24,546 | `cdr` |
| `/test/basemachine/basemachine/hatdriver/servo_hat_node/heartbeat` | 10 | `cdr` |
| `/test/example/example/example/example_node/heartbeat` | 13 | `cdr` |
| `/test/perception/depthcamerapipeline/depthcamera_pipeline_node/heartbeat` | 18 | `cdr` |
| `/test/pose/localpose/inertialsensorfuser/inertial_sensor_fuser_node/heartbeat` | 24 | `cdr` |
| `/test/pose/localpose/localposefuser/local_pose_fuser_node/heartbeat` | 10 | `cdr` |
| `/test/safety/modemanager/armedstatemanager/armed_state_manager_node/heartbeat` | 32 | `cdr` |

### 🔹 Datatype: `robot_framework_ros2/msg/ReadyToArm`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/basemachine/basemachine/hatdriver/servo_hat_node/ready_to_arm` | 2,950 | `cdr` |
| `/robot/perception/depthcamerapipeline/depthcamera_pipeline_node/ready_to_arm` | 2,923 | `cdr` |
| `/robot/pose/inertial_sensor/imu/imu_node1/ready_to_arm` | 2,957 | `cdr` |
| `/robot/pose/localpose/inertialsensorfuser/inertial_sensor_fuser_node/ready_to_arm` | 2,962 | `cdr` |
| `/robot/pose/localpose/localposefuser/local_pose_fuser_node/ready_to_arm` | 2,948 | `cdr` |
| `/test/basemachine/basemachine/hatdriver/servo_hat_node/ready_to_arm` | 2 | `cdr` |
| `/test/example/example/example/example_node/ready_to_arm` | 1 | `cdr` |
| `/test/perception/depthcamerapipeline/depthcamera_pipeline_node/ready_to_arm` | 2 | `cdr` |
| `/test/pose/localpose/inertialsensorfuser/inertial_sensor_fuser_node/ready_to_arm` | 5 | `cdr` |
| `/test/pose/localpose/localposefuser/local_pose_fuser_node/ready_to_arm` | 1 | `cdr` |
| `/test/safety/modemanager/armedstatemanager/armed_state_manager_node/ready_to_arm` | 9 | `cdr` |

## 🏢 System: Diagnostics

### 🔹 Datatype: `robot_framework_ros2/msg/Diagnostic`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/basemachine/basemachine/hatdriver/servo_hat_node/diagnostic` | 7,990 | `cdr` |
| `/robot/perception/depthcamerapipeline/depthcamera_pipeline_node/diagnostic` | 7,892 | `cdr` |
| `/robot/pose/inertial_sensor/imu/imu_node1/diagnostic` | 7,999 | `cdr` |
| `/robot/pose/localpose/inertialsensorfuser/inertial_sensor_fuser_node/diagnostic` | 2,962 | `cdr` |
| `/robot/pose/localpose/localposefuser/local_pose_fuser_node/diagnostic` | 5,502 | `cdr` |
| `/test/basemachine/basemachine/hatdriver/servo_hat_node/diagnostic` | 3 | `cdr` |
| `/test/example/example/example/example_node/diagnostic` | 1 | `cdr` |
| `/test/perception/depthcamerapipeline/depthcamera_pipeline_node/diagnostic` | 3 | `cdr` |
| `/test/pose/localpose/inertialsensorfuser/inertial_sensor_fuser_node/diagnostic` | 4 | `cdr` |
| `/test/pose/localpose/localposefuser/local_pose_fuser_node/diagnostic` | 2 | `cdr` |

## 🏢 System: Pose

### 🔹 Datatype: `sensor_msgs/msg/Imu`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/fused_imu/imu` | 0 | `cdr` |
| `/robot/imu1/imu` | 205,166 | `cdr` |
| `/test/fused_imu` | 2 | `cdr` |
| `/test/fused_imu/imu` | 10 | `cdr` |

### 🔹 Datatype: `geometry_msgs/msg/AccelStamped`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/imu1/accel` | 205,142 | `cdr` |

### 🔹 Datatype: `sensor_msgs/msg/MagneticField`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/imu1/magnetic` | 205,149 | `cdr` |

### 🔹 Datatype: `geometry_msgs/msg/AccelWithCovarianceStamped`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/local_pose_angular_accel` | 0 | `cdr` |
| `/test/local_pose_angular_accel` | 10 | `cdr` |

### 🔹 Datatype: `nav_msgs/msg/Odometry`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/local_pose` | 0 | `cdr` |
| `/test/local_pose` | 10 | `cdr` |

### 🔹 Datatype: `tf2_msgs/msg/TFMessage`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/tf_static` | 4 | `cdr` |

## 🏢 System: Perception

### 🔹 Datatype: `sensor_msgs/msg/CameraInfo`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/front_cam/depth/camera_info` | 69,941 | `cdr` |
| `/robot/front_cam/depth_raw/camera_info` | 70,774 | `cdr` |
| `/robot/front_cam/ir/camera_info` | 0 | `cdr` |
| `/robot/front_cam/projector/camera_info` | 69,717 | `cdr` |
| `/robot/front_cam/rgb/camera_info` | 69,527 | `cdr` |

### 🔹 Datatype: `sensor_msgs/msg/Image`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/front_cam/depth_raw/image` | 67,029 | `cdr` |
| `/robot/front_cam/depth_registered/image_raw` | 58,855 | `cdr` |
| `/robot/front_cam/ir/image_raw` | 0 | `cdr` |
| `/robot/front_cam/rgb/image_raw` | 57,716 | `cdr` |

### 🔹 Datatype: `sensor_msgs/msg/PointCloud2`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/robot/front_cam/depth_registered/points` | 4,610 | `cdr` |

## 🏢 System: ROS2

### 🔹 Datatype: `rosbag2_interfaces/msg/WriteSplitEvent`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/events/write_split` | 38 | `cdr` |

### 🔹 Datatype: `rcl_interfaces/msg/Log`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/rosout` | 98 | `cdr` |

### 🔹 Datatype: `rcl_interfaces/msg/ParameterEvent`

| Topic Name | Message Count | Serialization Format |
| :--- | :---: | :---: |
| `/parameter_events` | 135 | `cdr` |

