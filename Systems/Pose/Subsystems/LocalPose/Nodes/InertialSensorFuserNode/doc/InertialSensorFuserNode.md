`@compare_tag Node-Document v0.2`

- [InertialSensorFuserNode](#inertialsensorfusernode)
  - [Purpose](#purpose)
- [Architecture](#architecture)
  - [Class Diagram](#class-diagram)
- [Integration Guide](#integration-guide)
  - [Configuration](#configuration)
    - [Node Registry](#node-registry)

# InertialSensorFuserNode
## Purpose
The Inertial Sensor Fuser Node's objective is to read multiple IMU data and aggreate all this into one wholistic representation of an IMU.  NOTE: Multiple instances of this node may need to be executed if IMU's are not rigididly mounted to one another.

# Architecture
![](../../../../../../../Legend.png)

## Class Diagram
![](puml/InertialSensorFuserNodeClassDiagram.png)


# Integration Guide
## Configuration
### Node Registry
In your `node_registry.yaml` file, add the following:
```yaml
inertial_sensor_fuser_node: #Instance name of inertial_sensor_fuser_node, like inertial_sensor_fuser_node1, etc
    package: "robot_framework_ros2" 
    launch_file: "Systems/Pose/Subsystems/LocalPose/Nodes/InertialSensorFuserNode/launch/inertial_sensor_fuser_node.launch.xml" #Path to Launch File
    parameters: # Any parameter in xml launch file
      verbosity_level: "DEBUG"
      imu1_input_topic: "imu1" # Multiple IMU's will be supported in AB#1814
      fused_imu_output_topic: "fused_imu"
```