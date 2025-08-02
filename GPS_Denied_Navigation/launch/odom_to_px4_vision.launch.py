from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='files_rtabmap',
            executable='odom_to_px4_vision', 
            name='odom_to_px4_vision_node',
            output='screen',
        )
    ])
