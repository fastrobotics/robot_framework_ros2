`@compare_tag Node-Document v0.2`

- [{{cookiecutter.Node}} Node](#cookiecutternode-node)
- [Architecture](#architecture)
  - [Class Diagram](#class-diagram)
- [Integration Guide](#integration-guide)
  - [Configuration](#configuration)
    - [Node Registry](#node-registry)

# {{cookiecutter.Node}} Node

# Architecture
![](../../../Legend.png)

## Class Diagram
![](puml/{{cookiecutter.Node}}ClassDiagram.png)

# Integration Guide
## Configuration
### Node Registry
In your `node_registry.yaml` file, add the following:
```yaml
{{cookiecutter.NodeBinary}}: #Instance name of {{cookiecutter.NodeBinary}}, like {{cookiecutter.NodeBinary}}1, etc
    package: "robot_framework_ros2" 
    launch_file: "Systems/{{cookiecutter.System}}/Subsystems/{{cookiecutter.Subsystem}}/Nodes/{{cookiecutter.Node}}/launch/{{cookiecutter.NodeBinary}}.launch.xml" #Path to Launch File
    parameters: # Any parameter in xml launch file
      verbosity_level: "DEBUG"
```