# ROS 2 Distributed Configuration System Architecture Blueprint

This document details the production-ready architecture for a modular, host-agnostic, and scenario-layered ROS 2 configuration framework.

---

## 🏛️ Directory Architecture
Keep your layout structured cleanly to segregate base hardware capabilities, environmental parameters, network topologies, and dynamic mission scenarios.

```text
my_robot_bringup/
├── config/
│   ├── infrastructure/
│   │   └── tf_frames.yaml       # Shared transformations and frames
│   ├── systems/
│   │   └── safety.yaml          # Base safety constraints using wildcards
│   └── scenarios/
│       ├── normal.yaml          # Empty or baseline baseline scenario configurations
│       └── aggressive.yaml      # High-performance parameter layer modifications
├── launch/
│   ├── orchestrator.launch.py   # Environment-agnostic programmatic node deployer
│   └── topology/
│       └── hardware_layout.yaml # Simplified single-point node assignment registry
├── package.xml
└── CMakeLists.txt
```

---

## 🗺️ Configuration Layering (YAML Layouts)

### 1. The Global Parameter Layer (`config/systems/safety.yaml`)
Using the global wildcard pattern (`/**`), variables are exposed universally to any execution unit mapping this config file, removing namespace restrictions.

```yaml
/**:
  ros__parameters:
    max_linear_acceleration: 2.5   # Base fallback profile value (m/s^2)
    diagnostic_rate: 10.0          # Shared across safety subsystems

/safety/acceleration_monitor:      # Example of localized node overrides within the same file
  ros__parameters:
    filter_gain: 0.85
```

### 2. The Dynamic Scenario Augmentation Layer (`config/scenarios/aggressive.yaml`)
Scenarios act as delta modifiers. When stacked downstream by the launcher, the parameters defined here seamlessly override matching upstream baseline values.

```yaml
/**:
  ros__parameters:
    max_linear_acceleration: 5.0   # Seamlessly elevates the base limit from 2.5 to 5.0
```

### 3. Lean Network Topology Mapping (`launch/topology/hardware_layout.yaml`)
Decouple package dependencies from your physical target machines. The `node_registry` serves as a master dictionary, allowing hosts to claim compute blocks dynamically.

```yaml
# Master node configurations decoupled from execution nodes
node_registry:
  imu_node:
    package: "imu_driver_pkg"
    executable: "imu_node"
    default_parameters: ["infrastructure/tf_frames.yaml"]
  accel_monitor:
    package: "safety_pkg"
    executable: "monitor_node"
    default_parameters: ["systems/safety.yaml"]

# Host-specific mapping matrices (Compute load balance mapping)
host_assignments:
  alpha_comp:
    - imu_node
    - accel_monitor
  beta_comp:
    - some_heavy_vision_node
```

---

## 🎻 The Python Orchestrator (`launch/orchestrator.launch.py`)

This host-agnostic orchestration script resolves environmental constraints dynamically. It automatically evaluates the running machine's `hostname`, clusters relative tracking schemas under a flexible root parameter namespace, and injects runtime scenario overlays.

```python
import os
import socket
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
import yaml

def generate_launch_description():
    bringup_dir = get_package_share_directory('my_robot_bringup')
    
    # 1. Initialize High-Level Global Launch Modifiers
    robot_name_arg = DeclareLaunchArgument('robot_name', default_value='robot1')
    scenario_arg = DeclareLaunchArgument('scenario', default_value='normal')
    
    # Resolve physical environmental runtime configuration
    current_host = socket.gethostname()
    
    # 2. Parse and Ingest Central Topology Strategy
    topology_path = os.path.join(bringup_dir, 'launch', 'topology', 'hardware_layout.yaml')
    with open(topology_path, 'r') as f:
        topology = yaml.safe_load(f)
        
    registry = topology.get('node_registry', {})
    assignments = topology.get('host_assignments', {})
    
    # Identify localized tracking targets intended for this compute node
    nodes_for_this_host = assignments.get(current_host, [])
    
    launch_nodes = []
    
    # 3. Iteratively Assemble Node Lifecycle Contexts
    for node_id in nodes_for_this_host:
        if node_id not in registry:
            continue
            
        node_def = registry[node_id]
        
        # Build Parameter Layering Pipeline Stack
        parameter_stack = []
        
        # Layer I: Pull structural defaults mapped in registry
        for param_file in node_def.get('default_parameters', []):
            parameter_stack.append(os.path.join(bringup_dir, 'config', param_file))
            
        # Layer II: Contextually append live runtime scenario adjustments
        scenario_file_path = [
            os.path.join(bringup_dir, 'config', 'scenarios/'), 
            LaunchConfiguration('scenario'), 
            '.yaml'
        ]
        parameter_stack.append(scenario_file_path)
        
        # 4. Generate Node Executable Target Matrix
        ros_node = Node(
            package=node_def['package'],
            executable=node_def['executable'],
            name=node_id,
            namespace=LaunchConfiguration('robot_name'), # Enforces system neutrality
            parameters=parameter_stack,
            output='screen'
        )
        launch_nodes.append(ros_node)
        
    return LaunchDescription([
        robot_name_arg,
        scenario_arg,
        *launch_nodes
    ])
```

---

## 🚀 Deployment & Operations Guide

### Step 1: Distribution
Deploy the **exact same code package workspace** across all machine endpoints sharing the physical robot network.

### Step 2: Runtime Instructions
By leveraging dynamic local lookups, execution syntax scales naturally without unique localized scripts.

**Standard Profile Run:**
```bash
ros2 launch my_robot_bringup orchestrator.launch.py robot_name:=robot_alpha
```
* Nodes are pushed under the root namespace sequence `/robot_alpha/...`
* System pulls parameters from `safety.yaml` resolving to default `max_linear_acceleration: 2.5`.

**Dynamic High-Performance (Augmented) Override Run:**
```bash
ros2 launch my_robot_bringup orchestrator.launch.py robot_name:=robot_alpha scenario:=aggressive
```
* Infrastructure mapping maintains root uniformity under `/robot_alpha/...`
* ROS 2 dynamically feeds `safety.yaml` and overlays `aggressive.yaml` consecutively.
* Internal values flatten cleanly, initializing runtime thresholds directly to `max_linear_acceleration: 5.0`.
