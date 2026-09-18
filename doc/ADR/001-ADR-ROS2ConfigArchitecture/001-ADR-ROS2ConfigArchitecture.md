[Architecture Decision Records](../ADR.md)

- [ADR: ROS2 Config Architecture](#adr-ros2-config-architecture)
- [Description](#description)
  - [Objective](#objective)
  - [Workspace File Layout](#workspace-file-layout)
    - [Application File Layout](#application-file-layout)
    - [Framework File Layout](#framework-file-layout)
  - [Schema Layouts](#schema-layouts)
    - [Node Registry: `node_registry.yaml`](#node-registry-node_registryyaml)
    - [Infrastructure Registries: `tf_frames.yaml`, `topics.yaml`](#infrastructure-registries-tf_framesyaml-topicsyaml)
    - [Deployment Map: `deployment_map.yaml`](#deployment-map-deployment_mapyaml)
    - [Hardware Config Files: `config/hardware/<Device>.yaml`](#hardware-config-files-confighardwaredeviceyaml)
    - [Node Launch File `<Node Name>.launch.xml`](#node-launch-file-node-namelaunchxml)
    - [Orchestrator](#orchestrator)
    - [Scenario Augmentation Proposal](#scenario-augmentation-proposal)
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
    |  ├── hardware/
    |  |   ├── Fake_IMU_Device.yaml          # Universal default fallback calibration
    |  |   └── IMU_RobotshopTM151_3435.yaml  # Specific hardware calibration profiles
    |  └── infrastructure/
    |      ├── tf_frames.yaml
    |      └── topics.yaml
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
- Defines all Nodes, what XML file to use to launch them, and node-owned launch parameters.
- Owns shared infrastructure registry locations such as frames and topics.

```yaml
node_registry:
    <node name instance 1>:
        package: "<provider package>"
        launch_file: "<path to node launch file>" # Path to launch file, with root of <package> above.
    <node name instance N>:
        package: "<provider package>"
        launch_file: "<path to node launch file>" # Path to launch file, with root of <package> above.
      parameters:
        <parameter>: "<node-specific default>"
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
    parameters:
      node_namespace: "pose/inertial_sensor/imu"
      target_frame: "body_frame"
      imu_topic: "imu"
      accel_topic: "accel"
      magnetic_topic: "magnetic"
```

### Infrastructure Registries: `tf_frames.yaml`, `topics.yaml`
**Purpose**
- Defines shared ROS 2 values using stable configuration keys.
- Allows deployment maps to refer to frames, topics, and future infrastructure values by key instead of repeating concrete names.
- Keeps the mapping between a registry name and its file in the node registry.

```yaml
frames:
  <frame key>: "<ros frame name>"

topics:
  <topic key>: "<ros topic name>"
```

Example:
```yaml
# config/infrastructure/tf_frames.yaml
frames:
  body_frame: "base_link"
  imu_frame: "imu_link"

# config/infrastructure/topics.yaml
topics:
  imu: "imu"
  accel: "accel"
  magnetic: "magnetic"
```

The node registry declares the available registries:
```yaml
infrastructure_configs:
  frames:
    package: "robot_framework_ros2"
    relative_path: "config/infrastructure/tf_frames.yaml"
  topics:
    package: "robot_framework_ros2"
    relative_path: "config/infrastructure/topics.yaml"
```

Before launching a node, the orchestrator derives the registry from the parameter suffix. For example, `target_frame: "body_frame"` uses `frames` and becomes `target_frame: "base_link"`; `imu_topic: "imu"` uses `topics` and resolves to the configured topic value. Values that do not match a key are passed through unchanged. Future registries can be added by supplying another map name and configuration file, without adding another resolver function.

### Deployment Map: `deployment_map.yaml`
**Purpose**
- Defines what nodes are run on what hosts.
- Defines what hardware is connected to each host and node.
- Does not own node-wide topics, frames, namespaces, or other shared node defaults.

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
            <param name="robot_namespace" value="$(var robot_namespace)"/>
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
ros2 launch <application> orchestrator.launch.py robot_namespace:=<robot ID>
```

Note the intent is to have the user application have a symbolic link in their repo to this framework orchestrator.

### Scenario Augmentation Proposal
**Status: Initial implementation complete; schema and validation may evolve.**

**Objective**
Scenarios provide an application-level way to modify configuration for a specific operating mode without copying the complete base configuration and without placing robot-specific values in framework-owned files. A scenario may change node parameters, launch arguments, infrastructure aliases, enabled nodes, or other application-owned configuration as required by the scenario.

**Configuration Ownership**
Scenarios are overlays, not replacements for the base configuration. The base files remain the source of defaults and structure. A scenario directory should mirror the baseline application configuration layout so that ownership and relative paths remain predictable:

- `node_registry.yaml` defines the available nodes and their node-owned defaults.
- `deployment_map.yaml` defines host assignments and device relationships.
- Infrastructure registries define shared frames, topics, and similar named values.
- Hardware configuration files define device-specific parameters.
- A scenario overlay defines only the values that differ for that operating mode.

The overlay should be stored under the application configuration directory, for example:

```text
config/
└── scenarios/
  ├── simulation/
    │   ├── node_registry.yaml
    │   ├── deployment_map.yaml
    │   ├── hardware/
    │   └── infrastructure/
    └── field_test/
        ├── node_registry.yaml
        ├── deployment_map.yaml
        ├── hardware/
        └── infrastructure/
```

The scenario tree is sparse: a scenario includes only the baseline-relative files it changes. For example, a scenario that changes only topic aliases contains `infrastructure/topics.yaml`; it does not copy `node_registry.yaml`, `deployment_map.yaml`, or hardware files. Scenario-specific launch files may be placed under a matching `launch/` directory when a launch blueprint must change, but changing a launch blueprint is a protected operation described below.

The initial orchestrator implementation loads an explicitly selected scenario at launch time using an `OpaqueFunction`, then merges the sparse overlays before constructing node actions. When `scenario` is omitted, the baseline configuration is used directly; there is no required or implicit default scenario directory.

**Scenario Selection**
The orchestrator should accept a scenario argument:

```bash
ros2 launch <application> orchestrator.launch.py \
  robot_namespace:=robot1 scenario:=field_test
```

The selected scenario must be resolved before nodes are constructed. An omitted scenario uses the baseline files. A named scenario that is missing, malformed, or contains an invalid reference should stop launch with an actionable error rather than silently falling back to the baseline.

**Overlay Shape**
Each scenario file should use the same schema as its baseline counterpart. The orchestrator resolves a baseline file and then applies the matching scenario file by relative path. For example:

```yaml
# scenarios/field_test/node_registry.yaml
node_registry:
  imu_node:
    parameters:
      loop1_rate: 100.0
```

A matching deployment overlay uses the deployment schema:
```yaml
# scenarios/field_test/deployment_map.yaml
host_assignments:
  DevComputer2:
    nodes:
      - name: "imu_node"
        parameters:
          sensor_config_relative_path: "config/hardware/IMU_RobotshopTM151_3435.yaml"
```

An infrastructure overlay uses the infrastructure registry schema:
```yaml
# scenarios/field_test/infrastructure/topics.yaml
topics:
  imu: "imu/field"
```

The exact merge implementation remains subject to implementation, but the overlay should support these operations:

- Modify an existing node's launch parameters or ROS parameters.
- Modify an existing host/node hardware relationship.
- Add or remove a node from a host for the selected scenario.
- Select alternate hardware profiles and infrastructure registry values.
- Supply scenario-specific launch arguments while preserving the node launch blueprint.

Scenario files should not use a second scenario-specific schema such as a generic `scenario.yaml` with unrelated nested sections. Mirroring the baseline files makes each override local to the configuration it owns and allows the same validation rules to be applied to baseline and scenario files.

By default, a scenario should not replace the node registry's package, executable, or launch blueprint. Changing executable ownership or launch topology should require an explicit opt-in field so a scenario cannot accidentally replace the implementation it is meant to configure.

**Precedence**
Configuration should be resolved in a documented order, with later layers overriding earlier layers only where the schema permits it:

1. Framework defaults.
2. Application node registry and infrastructure registries.
3. Selected scenario overlay.
4. Host deployment map and hardware assignment.
5. Explicit command-line launch arguments.

The merge should be schema-aware. Mapping values may be merged recursively, node parameter maps may be overridden by key, and node lists should be merged by stable node name rather than by list position. An overlay must not silently delete an entire map because one nested value was changed.

The initial implementation merges `nodes` and `sensors` lists by their `name` field. An overlay item with `remove: true` removes the matching named item. Registry package, executable, and launch-file fields are protected for existing nodes unless an explicit implementation override is added in a future revision.

**Validation and Diagnostics**
Before launching nodes, the orchestrator should validate the fully resolved configuration. At minimum it should check that:

- The selected scenario exists and has a valid schema.
- Referenced hosts, nodes, hardware profiles, infrastructure keys, packages, and launch files exist.
- A node is not assigned to a host without a registry entry.
- Required parameters remain present after all overlays are applied.
- Protected fields have not changed unless explicitly allowed.

Diagnostics should identify the scenario file, logical configuration path, and final value that caused an error. The resolved configuration should also be printable in a non-launching validation mode for review and testing.

**Security and Reproducibility**
Scenario files are configuration inputs, not executable launch code. They may select existing launch files and values, but should not execute arbitrary Python or shell content. The selected scenario name and resolved configuration should be logged so a robot run can be reproduced from its scenario and deployment inputs.

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