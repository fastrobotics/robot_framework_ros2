import sys
import os
import yaml
from datetime import datetime

def generate_metadata_summary(bag_path: str):
    # Ensure the path points directly to the metadata file
    bag_path = bag_path.rstrip("/")
    metadata_file = os.path.join(bag_path, "metadata.yaml")
    
    if not os.path.exists(metadata_file):
        print(f"Error: metadata.yaml not found in '{bag_path}'")
        return

    print(f"Reading metadata from: {metadata_file}")
    with open(metadata_file, 'r') as f:
        try:
            metadata_data = yaml.safe_load(f)
        except Exception as e:
            print(f"Error parsing YAML file: {e}")
            return

    # Drill down into the ROS2 standard metadata block
    bag_info = metadata_data.get('rosbag2_bagfile_information', {})
    
    storage_identifier = bag_info.get('storage_identifier', 'Unknown')
    duration_nanos = bag_info.get('duration', {}).get('nanoseconds', 0)
    duration_secs = duration_nanos / 1e9
    total_messages = bag_info.get('message_count', 0)
    
    # Format starting timestamp
    starting_time_nanos = bag_info.get('starting_time', {}).get('nanoseconds', 0)
    try:
        start_time_str = datetime.fromtimestamp(starting_time_nanos / 1e9).strftime('%Y-%m-%d %H:%M:%S')
    except Exception:
        start_time_str = "Unknown"

    topics_and_types = bag_info.get('topics_with_message_count', [])

    # Define the output report name
    output_md_path = "rosbag_metadata_summary.md"
    
    # Generate and write Markdown formatting
    systemMap = {
        "Infrastructure": {
            "Heartbeat": ["diagnostic_msgs/msg/DiagnosticStatus"], # Add custom heartbeat types here
            "Diagnostics": ["diagnostic_msgs/msg/DiagnosticArray"]
        },
        "Pose": {
            "IMU": ["sensor_msgs/msg/Imu"],
            "Pose": ["geometry_msgs/msg/PoseStamped", "nav_msgs/msg/Odometry", "geometry_msgs/msg/TransformStamped"]
        },
        "Perception": {
            "Cameras": ["sensor_msgs/msg/Image", "sensor_msgs/msg/CompressedImage", "sensor_msgs/msg/CameraInfo"],
            "LiDAR": ["sensor_msgs/msg/PointCloud2", "sensor_msgs/msg/LaserScan"]
        }
    }
    with open(output_md_path, 'w') as md:
        md.write(f"# ROS2 Bag Metadata Summary\n\n")
        md.write(f"📁 **Bag Path:** `{bag_path}`  \n")
        md.write(f"⚙️ **Storage Plugin:** `{storage_identifier}`  \n")
        md.write(f"📅 **Start Time:** {start_time_str}  \n")
        md.write(f"⏳ **Duration:** {duration_secs:.2f} seconds  \n")
        md.write(f"🔢 **Total Messages:** {total_messages:,}  \n\n")
        
        md.write(f"## 📑 Available Topics & Statistics\n\n")
        md.write(f"| Topic Name | Message Type | Message Count | Serialization Format |\n")
        md.write(f"| :--- | :--- | :---: | :---: |\n")
        
        for topic in topics_and_types:
            t_info = topic.get('topic_metadata', {})
            t_name = t_info.get('name', 'Unknown')
            t_type = t_info.get('type', 'Unknown')
            t_count = topic.get('message_count', 0)
            t_format = t_info.get('serialization_format', 'Unknown')
            
            md.write(f"| `{t_name}` | `{t_type}` | {t_count:,} | `{t_format}` |\n")
            
    print(f"🎉 Successfully exported Markdown summary to: {output_md_path}")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python log_analyzer.py /path/to/bag_directory")
        sys.exit(1)
        
    generate_metadata_summary(sys.argv[1])
