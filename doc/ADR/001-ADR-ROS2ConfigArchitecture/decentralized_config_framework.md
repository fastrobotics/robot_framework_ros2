# Decentralized Configuration & Deployment Framework Documentation

This document outlines the architecture designed to keep the core framework library (`robot_framework_ros2`) completely generic, while allowing outside application profiles to dictate deployment targets, hardware configurations, and custom package paths seamlessly.

---

## 📂 Complete Workspace File Layout

This structure isolates your reusable core software files from your specific configuration and hardware description profiles, allowing for an easy transplantation to an external repository (like `crawler_app`) later on.

```text
robot_framework_ros2/
├── CMakeLists.txt              # Build instructions handling recursive directory installations
├── Systems/                    # Universal core framework library
│   └── Pose/
│       └── Subsystems/
│           └── InertialSensor/
│               └── Nodes/
│                   └── IMUNode/
│                       └── launch/
│                           └── imu_node.launch.xml   # Explicit XML node blueprint
├── config/                     # Configuration Sandbox (Transplantable)
│   ├── device_registry.yaml    # Master software catalog
│   ├── host_assignments.yaml   # Machine-specific deployment map
│   ├── imu_node_config.yaml    # Baseline software update rates/loops
│   └── hardware/
│       ├── Fake_IMU_Device.yaml          # Universal default fallback calibration
│       └── IMU_RobotshopTM151_3435.yaml  # Specific hardware calibration profiles
└── launch/
    └── orchestrator.launch.py  # 100% node-agnostic foreground loop orchestrator
```

---

## 📄 Configuration Schema Layouts

### 1. The Master Catalog (`config/device_registry.yaml`)
Acts as a central software dictionary. It tracks only actual compiled binary files or core launch blueprints. It contains zero hardware asset definitions or machine strings.

```yaml
node_registry:
  example_node1:
    package: "robot_framework_ros2"
    executable: "example_node"

  example_node2:
    package: "robot_framework_ros2"
    executable: "example_node"

  imu_node:
    package: "robot_framework_ros2"
    launch_file: "Systems/Pose/Subsystems/InertialSensor/Nodes/IMUNode/launch/imu_node.launch.xml"
```

### 2. The Deployment Map (`config/host_assignments.yaml`)
Maps specific physical hardware computers (via their network hostname) to the target nodes they are responsible for executing. It handles cross-package path mappings using clean, explicit strings instead of brittle environment variables or absolute directory strings.

```yaml
host_assignments:
  DevComputer2:
    sensors:
      - name: "mock_imu"

    nodes:
      - name: "example_node1"
      - name: "example_node2"
      
      - name: "imu_node"
        parameters:
          sensor: "IMU_RobotshopTM151_3435"
          sensor_config_package: "robot_framework_ros2"
          sensor_config_relative_path: "config/hardware/IMU_RobotshopTM151_3435.yaml"
```

### 3. Structured Hardware Files (`config/hardware/IMU_RobotshopTM151_3435.yaml`)
Keeps your sensor properties entirely un-wrapped and portable. By structuring them under a global wildcard selector (`/**`), ROS 2 natively matches the inner fields into whatever nested namespace your node executes under.

```yaml
/**:
  ros__parameters:
    info:
      description: Robotshop TM151 9DoF IMU
      device_name: /dev/imu_STMicroelectronics_3435
      full_serial_number: 375D36663435
      manufacturer: STMicroelectronics
      name: IMU_RobotshopTM151_3435
      type: SYDTM151
    date_generated: '2026-08-05 17:35:33'
    orientation_covariance_matrix:
      - 2.0706504113366553e-07
      - 5.0208139028371385e-08
      - 8.92790293249248e-08
      - 5.0208139028371385e-08
      - 9.494179413759707e-08
```

---

## 🎻 Component Blueprint Implementations

### 1. Universal Node Wrapper XML (`imu_node.launch.xml`)
Exposes flexible parameter parameters and evaluates custom target paths at runtime using structural `<let>` bindings, bypassing ROS 2's command-line substitution limitations entirely.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<launch>
    <arg name="robot_namespace" default="/"/> 
    <arg name="node_name" default="imu_node"/>
    <arg name="sensor" default=""/>
    
    <arg name="sensor_config_package" default="robot_framework_ros2"/>
    <arg name="sensor_config_relative_path" default="config/hardware/Fake_IMU_Device.yaml"/>

    <!-- Native Resolution expansions happen locally within the XML framework -->
    <let name="hw_config" value="$(find-pkg-share $(var sensor_config_package))/$(var sensor_config_relative_path)"/>

    <group>
        <push-ros-namespace namespace="$(var robot_namespace)"/>
        
        <node pkg="robot_framework_ros2" exec="imu_node" name="$(var node_name)" namespace="pose/inertialsensor/imu" output="screen" emulate_tty="true">
            <!-- Dynamically load the targeted sensor profile directly into this namespace block -->
            <param from="$(var hw_config)"/>
            
            <param name="sensor" value="$(var sensor)"/>
            <param name="loop1_rate" value="200.0"/>
            <param name="loop2_rate" value="200.0"/>
        </node>
    </group>
</launch>
```

### 2. Agnostic Orchestrator (`orchestrator.launch.py`)
Remains 100% decoupled from node logic. It simply parses the target strings out of your assignment file configurations, determines whether to execute a naked binary or wrap an XML launcher description, and locks all processes in the foreground.

```python
import os
import socket
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch_xml.launch_description_sources import XMLLaunchDescriptionSource
from launch_ros.actions import Node
import yaml

def generate_launch_description():
    bringup_dir = get_package_share_directory('robot_framework_ros2')
    scenario_arg = DeclareLaunchArgument('scenario', default_value='normal')
    current_host = socket.gethostname()
    
    registry_path = os.path.join(bringup_dir, 'config', 'device_registry.yaml')
    assignments_path = os.path.join(bringup_dir, 'config', 'host_assignments.yaml')
    launch_actions = [scenario_arg]
    
    if not os.path.exists(registry_path) or not os.path.exists(assignments_path):
        return LaunchDescription(launch_actions)
        
    with open(registry_path, 'r') as f:
        registry = yaml.safe_load(f).get('node_registry', {})
    with open(assignments_path, 'r') as f:
        assignments = yaml.safe_load(f).get('host_assignments', {}).get(current_host, {})
        
    active_nodes = assignments.get('nodes', [])
    
    for node_item in active_nodes:
        target_name = node_item.get('name')
        if target_name not in registry:
            continue
            
        node_def = registry[target_name]
        custom_node_params = node_item.get('parameters', {})
        
        if 'launch_file' in node_def:
            xml_absolute_path = os.path.join(bringup_dir, node_def['launch_file'])
            launch_args = {str(k): str(v) for k, v in custom_node_params.items()}
            
            launch_actions.append(IncludeLaunchDescription(
                XMLLaunchDescriptionSource(xml_absolute_path),
                launch_arguments=launch_args.items()
            ))
        elif 'executable' in node_def:
            launch_actions.append(Node(
                package=node_def['package'],
                executable=node_def['executable'],
                name=target_name,                         
                parameters=[custom_node_params] if custom_node_params else [], 
                output='screen',      
                emulate_tty=True      
            ))
            
    return LaunchDescription(launch_actions)
```

---

## ⚙️ Compilation Integration (`CMakeLists.txt`)

To ensure that your nested core system modules are recursively copied over to the global installation sandbox space during a build step, your `CMakeLists.txt` file handles asset registration like this:

```cmake
# Recursively install your modular trees into the share directory
install(
  DIRECTORY Systems config launch
  DESTINATION share/${PROJECT_NAME}
)
```

---

## 📦 The Transplant Process Later
When you are ready to cut the cord and move this setup into your high-level application package (`crawler_app`):

1. **Move Files:** Cut and paste `host_assignments.yaml` and your `hardware/` subfolder straight into your new package repo directory.
2. **Text Swap:** In `host_assignments.yaml`, simply update your parameter text fields to point to your new package namespace:
   ```yaml
   sensor_config_package: "crawler_app"
   sensor_config_relative_path: "scenarios/dev/DeviceConfig/IMU_RobotshopTM151_3435.yaml"
   ```

Your entire underlying framework library, agnostic Python loops, and XML blueprints will continue working instantly with zero modifications!