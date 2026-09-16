import os
import socket
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch_xml.launch_description_sources import XMLLaunchDescriptionSource
from launch_ros.actions import Node
import yaml

def generate_launch_description():
    # 1. Point to the parent package share directory
    bringup_dir = get_package_share_directory('robot_framework_ros2')
    
    # Expose the high-level scenario argument
    scenario_arg = DeclareLaunchArgument('scenario', default_value='normal')
    
    # Automatically read the local computer's network name
    current_host = socket.gethostname()
    print(f"\n[ORCHESTRATOR DIAGNOSTIC]: Host machine identified as: '{current_host}'")
    
    # Define paths to our separate repository configuration files
    registry_path = os.path.join(bringup_dir, 'config', 'device_registry.yaml')
    assignments_path = os.path.join(bringup_dir, 'config', 'host_assignments.yaml')
    
    # Initialize our launch queue with the scenario argument
    launch_actions = [scenario_arg]
    
    # Error checking to ensure both config files exist
    if not os.path.exists(registry_path) or not os.path.exists(assignments_path):
        print("[ORCHESTRATOR ERROR]: Missing 'device_registry.yaml' or 'host_assignments.yaml'!")
        return LaunchDescription(launch_actions)
        
    # Read and parse both files
    with open(registry_path, 'r') as f:
        registry_data = yaml.safe_load(f)
    with open(assignments_path, 'r') as f:
        assignments_data = yaml.safe_load(f)
        
    registry = registry_data.get('node_registry', {})
    all_host_assignments = assignments_data.get('host_assignments', {})
    
    # Extract deployment data specifically configured for THIS host computer
    this_host_config = all_host_assignments.get(current_host, {})
    active_nodes = this_host_config.get('nodes', [])
    
    print(f"[ORCHESTRATOR DIAGNOSTIC]: Executing computational nodes for '{current_host}': {[n.get('name') for n in active_nodes]}\n")
    
    # 2. Dynamic Execution Loop (100% Agnostic)
    for node_item in active_nodes:
        target_name = node_item.get('name')
        
        if target_name not in registry:
            print(f"[ORCHESTRATOR WARNING]: Node '{target_name}' is missing from the device registry!")
            continue
            
        node_def = registry[target_name]
        custom_node_params = node_item.get('parameters', {})
        
        # --- PATH A: THE REGISTRY DIRECTS THE ITEM TO AN XML LAUNCH BLUEPRINT ---
        if 'launch_file' in node_def:
            xml_absolute_path = os.path.join(bringup_dir, node_def['launch_file'])
            
            # Pass all dictionary parameters down directly as string launch arguments
            launch_args = {str(k): str(v) for k, v in custom_node_params.items()}
            
            included_xml_launch = IncludeLaunchDescription(
                XMLLaunchDescriptionSource(xml_absolute_path),
                launch_arguments=launch_args.items()
            )
            launch_actions.append(included_xml_launch)
            
        # --- PATH B: THE REGISTRY DIRECTS THE ITEM TO A STANDALONE BINARY ---
        elif 'executable' in node_def:
            node_parameters = [custom_node_params] if custom_node_params else []
            
            ros_node = Node(
                package=node_def['package'],
                executable=node_def['executable'],
                name=target_name,                         
                parameters=node_parameters, 
                output='screen',      # Stream stdout directly to the console window
                emulate_tty=True      # Prevent line buffering so logs show in real time
            )
            launch_actions.append(ros_node)
            
    return LaunchDescription(launch_actions)
