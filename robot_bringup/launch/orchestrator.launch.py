import os
import socket
import xml.etree.ElementTree as ElementTree
from copy import deepcopy
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch_xml.launch_description_sources import XMLLaunchDescriptionSource
from launch_ros.actions import Node
import yaml

def load_yaml_file(config_path, description):
    if not os.path.exists(config_path):
        raise RuntimeError(f"{description} does not exist: {config_path}")

    with open(config_path, 'r') as config_file:
        return yaml.safe_load(config_file) or {}


def merge_named_lists(base_values, overlay_values):
    merged_values = deepcopy(base_values)
    indexes = {item.get('name'): index for index, item in enumerate(merged_values) if isinstance(item, dict)}

    for overlay_value in overlay_values:
        if not isinstance(overlay_value, dict) or 'name' not in overlay_value:
            merged_values.append(deepcopy(overlay_value))
            continue

        item_name = overlay_value['name']
        if overlay_value.get('remove') is True:
            if item_name in indexes:
                merged_values.pop(indexes[item_name])
                indexes = {item.get('name'): index for index, item in enumerate(merged_values) if isinstance(item, dict)}
            continue

        if item_name in indexes:
            merged_values[indexes[item_name]] = merge_config(
                merged_values[indexes[item_name]], overlay_value)
        else:
            indexes[item_name] = len(merged_values)
            merged_values.append(deepcopy(overlay_value))

    return merged_values


def merge_config(base_config, overlay_config, key=None):
    if isinstance(base_config, dict) and isinstance(overlay_config, dict):
        merged_config = deepcopy(base_config)
        for config_key, overlay_value in overlay_config.items():
            merged_config[config_key] = merge_config(
                merged_config.get(config_key), overlay_value, config_key)
        return merged_config

    if isinstance(base_config, list) and isinstance(overlay_config, list) and key in {'nodes', 'sensors'}:
        return merge_named_lists(base_config, overlay_config)

    return deepcopy(overlay_config)


def validate_registry_overlay(base_registry, overlay_registry):
    protected_fields = {'package', 'executable', 'launch_file'}
    base_nodes = base_registry.get('node_registry', {})
    overlay_nodes = overlay_registry.get('node_registry', {})

    for node_name, overlay_node in overlay_nodes.items():
        if node_name not in base_nodes or not isinstance(overlay_node, dict):
            continue
        for field in protected_fields:
            if field in overlay_node and overlay_node[field] != base_nodes[node_name].get(field):
                raise RuntimeError(
                    f"Scenario cannot change protected node_registry field "
                    f"'{node_name}.{field}' without explicit implementation override")


def scenario_relative_path(scenario_directory, config_relative_path):
    relative_path = config_relative_path
    if relative_path.startswith('config/'):
        relative_path = relative_path[len('config/'):]
    return os.path.join(scenario_directory, relative_path)


def load_named_map(config_name, config_definition, scenario_directory=None):
    config_package = config_definition.get('package', 'robot_framework_ros2')
    config_relative_path = config_definition.get('relative_path')
    if not config_relative_path:
        raise RuntimeError(f"Configuration '{config_name}' has no relative_path")

    config_share = get_package_share_directory(config_package)
    config_path = os.path.join(config_share, config_relative_path)

    config_data = load_yaml_file(config_path, f"Configuration '{config_name}'")

    if scenario_directory:
        overlay_path = scenario_relative_path(scenario_directory, config_relative_path)
        if os.path.exists(overlay_path):
            overlay_data = load_yaml_file(overlay_path, f"Scenario configuration '{config_name}'")
            config_data = merge_config(config_data, overlay_data)

    values = config_data.get(config_name, {})
    if not isinstance(values, dict):
        raise RuntimeError(f"Configuration must define a '{config_name}' mapping")
    return values


def resolve_named_parameters(parameters, named_maps):
    resolved_parameters = dict(parameters)

    for parameter_name, parameter_value in parameters.items():
        if '_' not in parameter_name or not isinstance(parameter_value, str):
            continue

        map_name = f"{parameter_name.rsplit('_', 1)[-1]}s"
        values = named_maps.get(map_name)
        if values is not None and parameter_value in values:
            resolved_parameters[parameter_name] = values[parameter_value]

    return resolved_parameters


def launch_argument_defaults(xml_absolute_path):
    launch_root = ElementTree.parse(xml_absolute_path).getroot()
    return {
        launch_argument.get('name'): launch_argument.get('default')
        for launch_argument in launch_root.findall('arg')
        if launch_argument.get('name') and launch_argument.get('default') is not None
    }


def build_launch_actions(context):
    bringup_dir = get_package_share_directory('robot_framework_ros2')
    scenario_name = context.perform_substitution(LaunchConfiguration('scenario'))
    scenario_directory = None

    if scenario_name and (os.path.sep in scenario_name or scenario_name in {'.', '..'}):
        print(f"[ORCHESTRATOR ERROR]: Invalid scenario name '{scenario_name}'")
        return []

    if scenario_name:
        scenario_directory = os.path.join(bringup_dir, 'config', 'scenarios', scenario_name)
        if not os.path.isdir(scenario_directory):
            print(f"[ORCHESTRATOR ERROR]: Scenario '{scenario_name}' does not exist: {scenario_directory}")
            return []

    # Automatically read the local computer's network name
    current_host = socket.gethostname()
    print(f"\n[ORCHESTRATOR DIAGNOSTIC]: Host machine identified as: '{current_host}'")
    
    # Define paths to our separate repository configuration files
    node_registry_path = os.path.join(bringup_dir, 'config', 'node_registry.yaml')
    deployment_map_path = os.path.join(bringup_dir, 'config', 'deployment_map.yaml')
    
    # Error checking to ensure both config files exist
    if not os.path.exists(node_registry_path) or not os.path.exists(deployment_map_path):
        print("[ORCHESTRATOR ERROR]: Missing 'node_registry.yaml' or 'deployment_map.yaml'!")
        return []
        
    # Read and parse both files
    node_registry_data = load_yaml_file(node_registry_path, 'Node registry')
    deployed_data = load_yaml_file(deployment_map_path, 'Deployment map')

    if scenario_directory:
        scenario_registry_path = os.path.join(scenario_directory, 'node_registry.yaml')
        scenario_deployment_path = os.path.join(scenario_directory, 'deployment_map.yaml')
        if os.path.exists(scenario_registry_path):
            scenario_registry_data = load_yaml_file(scenario_registry_path, 'Scenario node registry')
            validate_registry_overlay(node_registry_data, scenario_registry_data)
            node_registry_data = merge_config(node_registry_data, scenario_registry_data)
        if os.path.exists(scenario_deployment_path):
            scenario_deployment_data = load_yaml_file(scenario_deployment_path, 'Scenario deployment map')
            deployed_data = merge_config(deployed_data, scenario_deployment_data)
        
    node_registry = node_registry_data.get('node_registry', {})
    all_host_assignments = deployed_data.get('host_assignments', {})
    
    # Extract deployment data specifically configured for THIS host computer
    this_host_config = all_host_assignments.get(current_host, {})
    active_nodes = this_host_config.get('nodes', [])

    infrastructure_configs = node_registry_data.get('infrastructure_configs', {})
    named_maps = {}
    try:
        for config_name, config_definition in infrastructure_configs.items():
            named_maps[config_name] = load_named_map(config_name, config_definition, scenario_directory)
    except (KeyError, RuntimeError, yaml.YAMLError) as error:
        print(f"[ORCHESTRATOR ERROR]: Unable to load infrastructure configuration: {error}")
        return []

    launch_actions = []
    
    print(f"[ORCHESTRATOR DIAGNOSTIC]: Executing computational nodes for '{current_host}': {[n.get('name') for n in active_nodes]}\n")
    
    # 2. Dynamic Execution Loop (100% Agnostic)
    for node_item in active_nodes:
        target_name = node_item.get('name')
        
        if target_name not in node_registry:
            print(f"[ORCHESTRATOR WARNING]: Node '{target_name}' is missing from the node registry!")
            continue
            
        node_def = node_registry[target_name]
        registry_node_params = node_def.get('parameters', {})
        deployment_node_params = node_item.get('parameters', {})
        node_params = {**registry_node_params, **deployment_node_params}
        resolved_node_params = resolve_named_parameters(node_params, named_maps)
        
        # --- PATH A: THE REGISTRY DIRECTS THE ITEM TO AN XML LAUNCH BLUEPRINT ---
        if 'launch_file' in node_def:
            launch_package = node_def.get('package', 'robot_framework_ros2')
            launch_package_share = get_package_share_directory(launch_package)
            xml_absolute_path = os.path.join(launch_package_share, node_def['launch_file'])
            
            # Pass all dictionary parameters down directly as string launch arguments.
            # Do not override an XML default node_namespace with an empty value; that would
            # collapse the config namespace to "" and make parameters resolve as ".imu_node.*".
            launch_args = launch_argument_defaults(xml_absolute_path)
            launch_args.update({str(k): str(v) for k, v in resolved_node_params.items()})
            launch_args['node_name'] = target_name
            launch_args['robot_namespace'] = LaunchConfiguration('robot_namespace')

            included_xml_launch = IncludeLaunchDescription(
                XMLLaunchDescriptionSource(xml_absolute_path),
                launch_arguments=launch_args.items()
            )
            launch_actions.append(included_xml_launch)
            
        # --- PATH B: THE REGISTRY DIRECTS THE ITEM TO A STANDALONE BINARY ---
        elif 'executable' in node_def:
            node_parameters = [resolved_node_params] if resolved_node_params else []
            
            ros_node = Node(
                package=node_def['package'],
                executable=node_def['executable'],
                name=target_name,                         
                namespace=LaunchConfiguration('robot_namespace'),
                parameters=node_parameters, 
                output='screen',      # Stream stdout directly to the console window
                emulate_tty=True      # Prevent line buffering so logs show in real time
            )
            launch_actions.append(ros_node)
            
    return launch_actions


def generate_launch_description():
    scenario_arg = DeclareLaunchArgument(
        'scenario',
        default_value='',
        description='Optional scenario overlay; baseline configuration is used when omitted')
    robot_namespace_arg = DeclareLaunchArgument(
        'robot_namespace',
        default_value='',
        description='Unique root namespace for this robot')

    return LaunchDescription([
        scenario_arg,
        robot_namespace_arg,
        OpaqueFunction(function=build_launch_actions),
    ])
