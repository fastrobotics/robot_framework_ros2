[Architecture Decision Records](../ADR.md)

- [ADR: ROS2 Config Architecture](#adr-ros2-config-architecture)
- [ToDo List](#todo-list)
- [Description](#description)
  - [Objective](#objective)
  - [Workspace File Layout](#workspace-file-layout)
    - [Application File Layout](#application-file-layout)
    - [Framework File Layout](#framework-file-layout)
  - [Schema Layouts](#schema-layouts)
    - [Node Registry: `node_registry.yaml`](#node-registry-node_registryyaml)
    - [Deployment Map: `deployment_map.yaml`](#deployment-map-deployment_mapyaml)
    - [Hardware Config Files: `config/hardware/<Device>.yaml`](#hardware-config-files-confighardwaredeviceyaml)
    - [Node Launch File `<Node Name>.launch.xml`](#node-launch-file-node-namelaunchxml)
    - [Orchestrator](#orchestrator)
- [Critical Analysis](#critical-analysis)
  - [1. Configuration Explosion (Maintainability \& Scale Debt)](#1-configuration-explosion-maintainability--scale-debt)
  - [2. Loss of Native ROS 2 Launch Tooling (Orchestration Limitations)](#2-loss-of-native-ros-2-launch-tooling-orchestration-limitations)
  - [3. Typeless Fragility \& Silent Failures (Data Integrity Risks)](#3-typeless-fragility--silent-failures-data-integrity-risks)
  - [4. Graph Topography Hiding (Hidden Network Dependencies)](#4-graph-topography-hiding-hidden-network-dependencies)
- [Alternatives Investigated](#alternatives-investigated)
- [Implications](#implications)
- [Follow-up](#follow-up)
- [Deviations](#deviations)

# ADR: ROS2 Config Architecture
# ToDo List
- Robot namespace
- Launch on crawler
- Linkage to orchestrator
- Add topic map config
- Update Node Templates
- 
# Description
## Objective
ROS2 provides a much larger breadth of how config works.  This ADR aims at codifying how this architecture will work in this robot framework.  Worth noting is that this is being created with the following concerns:
- ROS2 Compatability
- Best Practices in the ROS2 Communicty
- Sensical separation of concerns
- Maintenance

## Workspace File Layout
The following lists a typical file layout.  This is broken into 2 sections: Application and Framework.  Note that this Framework itself also provides an "Application" section that is used for testing.

### Application File Layout
```text
<application>/
├── CMakeLists.txt
├── robot_bringup
    ├── config/                     # Configuration Sandbox (Transplantable)
    |  ├── node_registry.yaml      # Master software catalog
    |  ├── deployment_map.yaml   # Machine-specific deployment map
    |  ├── imu_node_config.yaml    # Baseline software update rates/loops
    |  └── hardware/
    |      ├── Fake_IMU_Device.yaml          # Universal default fallback calibration
    |      └── IMU_RobotshopTM151_3435.yaml  # Specific hardware calibration profiles
    └── launch/
        └── orchestrator.launch.py  # Sym-Link to Framework Orchestrator
```

### Framework File Layout
```text
robot_framework_ros2/
├── CMakeLists.txt
├── Systems/
│   └── <System>/
│       └── <Subsystem>/
│           └── <Subsystem 1-N>/
│               └── Nodes/
│                   └── <Node>/
│                       └── launch/
│                           └── <Node>.launch.xml   # Explicit XML node blueprint
└── launch/
    └── orchestrator.launch.py  # Re-usable Orchestrator Launch File.  This is what the application launches.
```

## Schema Layouts
### Node Registry: `node_registry.yaml`
**Purpose**
- Defines all Nodes and what XML file to use to launch them.

```yaml
node_registry:
    <node name instance 1>:
        package: "<provider package>"
        launch_file: "<path to node launch file>" # Path to launch file, with root of <package> above.
    <node name instance N>:
        package: "<provider package>"
        launch_file: "<path to node launch file>" # Path to launch file, with root of <package> above.
```

Example:
```yaml
node_registry:
  example_node1:
    package: "robot_framework_ros2"
    launch_file: "examples/ExampleNode/launch/example_node.launch.xml"

  example_node2:
    package: "robot_framework_ros2"
    launch_file: "examples/ExampleNode/launch/example_node.launch.xml"

  imu_node:
    package: "robot_framework_ros2"
    launch_file: "Systems/Pose/Subsystems/InertialSensor/Nodes/IMUNode/launch/imu_node.launch.xml"
```

### Deployment Map: `deployment_map.yaml`
**Purpose**
- Defines what nodes are run on what hosts
- Defines what hardware is connected to what node

```yaml
host_assignments:
    <device hostname 1>:
        sensors:
        - name: "<sensor 1>"
        - name: "<sensor N>"
    nodes:
      - name: "node name instance 1"
      - name: "node name instance 2"
        parameters:
          sensor: "<sensor 1>"
          sensor_config_package: "<package>"
          sensor_config_relative_path: "config/hardware/<sensor config>.yaml"
    <device hostname N>:
        ...
```

Example:
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

### Hardware Config Files: `config/hardware/<Device>.yaml`
**Purpose**
Hardware Config files by their nature will be specific to that piece of hardware.  In general effort should be made to commonize these as much as 
possible.  For example IMU's should typically have the same layout.  However there is some common elements that should generally be applied to all hardware config files:

```yaml
/**:
  ros__parameters:
    info:
      description: <Describe the Hardware>
      manufacturer: <Manufacturer>
      name: <Unique Name>
      type: <Hardware Type>
    date_generated: <When this config was created/generated>
    <parameter 1>: ...
    <parameter N>: ...
```

Example:
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

### Node Launch File `<Node Name>.launch.xml`
**Purpose**
The Node Launch file is typically provided by the framework and isn't intended to be modified by the application.  The purpose is to provide a wrapper that allows the node to have a common config structure.

```xml
<?xml version="1.0" encoding="UTF-8"?>
<launch>
    <arg name="robot_namespace" default="/"/> 
    <arg name="node_name" default="<Default Node Name>"/>
    <!-- Parameters that can be modified by Deployment Map above -->
    <arg name="sensor" default=""/>    
    <arg name="sensor_config_package" default="robot_framework_ros2"/>
    <arg name="sensor_config_relative_path" default="config/hardware/Fake_IMU_Device.yaml"/>

    <!-- Native Resolution expansions happen locally within the XML framework -->
    <let name="hw_config" value="$(find-pkg-share $(var sensor_config_package))/$(var sensor_config_relative_path)"/>

    <group>
        <push-ros-namespace namespace="$(var robot_namespace)"/>
        
        <node pkg="<Node Package>" exec="<Node Binary>" name="$(var node_name)" namespace="<Namespace to launch under>" output="screen" emulate_tty="true">
            <!-- Dynamically load the targeted sensor profile directly into this namespace block -->
            <param from="$(var hw_config)"/>
            
            <param name="sensor" value="$(var sensor)"/>
            <param name="loop1_rate" value="200.0"/>
            <param name="loop2_rate" value="200.0"/>
        </node>
    </group>
</launch>
```

Example:
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

### Orchestrator
**Purpose**
An orchestrator is provided that is intended to be the main application file that spawns off all nodes on the robot hosts.  Note that the intent here is that no matter what host is executed, it's the same orchestrator with the same user space configuration, and the orchestrator's job is to select which nodes and how they run.  For example to run this on any host on your robot:
```bash
ros2 launch <application> orchestrator.launch.py
```

Note the intent is to have the user application have a symbolic link in their repo to this framework orchestrator.

# Critical Analysis
The following list critial concerns with this ADR:
## 1. Configuration Explosion (Maintainability & Scale Debt)
To manage, declare, and execute even a single modular node package within the current design paradigm, developers must interact with and synchronize data values across up to four distinct directory files.

* **The Negative:** Adding or renaming software tasks requires manual modification of `node_registry.yaml`, `deployment_map.yaml`, a node launch file (`*.launch.xml`), and, where applicable, hardware configuration files. This separation is intentional: general node behavior, node instances, host placement, and hardware-specific data are different concerns. However, the separation still creates coordination and naming overhead.
* **The Mitigation:** Hardware auto-identification can reduce deployment-specific configuration when a node can reliably discover the attached device. Reusable, stable lookup keys can also reduce duplication, but they introduce another indirection layer and do not remove the need to define node instances, host assignments, or hardware profiles in every case.
* **The Residual Risk:** As the robot infrastructure scales, references can still drift between files. The orchestrator must therefore report missing registry entries, invalid assignments, and unresolved hardware profiles clearly; otherwise a configuration mistake may only become visible during launch.
---

## 2. Loss of Native ROS 2 Launch Tooling (Orchestration Limitations)
Because the orchestrator loop enforces a strict node-agnostic design, it treats launch instructions entirely as flat, un-nested text parameters rather than building out a rich, reactive node execution dependency tree.

* **The Negative:** The ADR does not remove native ROS 2 launch capabilities if the referenced XML launch files remain free to use them. However, it does not define how the orchestrator exposes, composes, or validates event handlers, lifecycle transitions, conditional actions, or process dependencies across those files. The risk is therefore an integration and observability gap, rather than proof that the capabilities are impossible.
* **The Mitigation:** The orchestrator can be extended to support richer launch descriptions, and node-specific launch files can retain native ROS 2 launch behavior. That extension should be treated as an explicit contract, with documented behavior for startup ordering, readiness, shutdown, and process failure.
* **The Residual Risk:** Until that contract exists, deployments requiring deterministic sequencing or recovery may need custom launch wrappers. The ADR should not claim that the current orchestrator provides those guarantees merely because it could be augmented in the future.
---

## 3. Typeless Fragility & Silent Failures (Data Integrity Risks)
YAML and standard XML argument schemas are inherently string-literal and lack compiled constraint checks. 

* **The Negative:** The current ADR defines YAML and XML layouts, but it does not define a schema, type-checking step, or pre-launch validation command. A typo in an asset key or an invalid value can therefore remain undetected until package resolution or node startup.
* **The Mitigation:** The orchestrator can scan YAML and XML before launching, validate required fields and cross-file references, and reject values that do not match the expected type or range. This is a valid mitigation, but it is not provided by the file formats alone and should be specified as part of the architecture.
* **The Residual Risk:** Until validation is implemented and made mandatory, configuration errors remain a runtime risk. Validation should distinguish warnings from launch-blocking errors and report the source file and field so failures are actionable.

---

## 4. Graph Topography Hiding (Hidden Network Dependencies)
While the current configuration layout effectively decouples *physical system directory file paths* on the hard drive, it introduces tight coupling across the *ROS network topic graph namespaces*.

* **The Negative:** The feedback is valid that nodes defined by this framework are intended to use a specified namespace. That makes the namespace predictable for consumers, but the current deployment map does not declare or validate the node's topic, service, action, or parameter interfaces. Those interfaces remain hidden in the node launch file and implementation.
* **The Mitigation:** Keeping framework nodes in stable, documented namespaces reduces accidental movement. A topic/interface map, explicit remapping contract, or validation test can make those public interfaces discoverable and detect breaking changes. The existing ToDo item to add a topic map is relevant here.
* **The Residual Risk:** The namespace convention does not eliminate interface coupling, and it does not apply automatically to arbitrary ROS 2 nodes outside this framework. A namespace or topic change can still break consumers unless the public interface is documented and checked independently of the internal directory layout.

# Alternatives Investigated

# Implications

# Follow-up

This ADR should be revisited in the future based on the following:

# Deviations

Not following this practice may be unavoidable in some exceptions. These are detailed below: