import os

from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    waypoint_node = Node(
        package='load_250_bringup',  
        executable='waypoint_from_json', 
        name='waypoint_from_json_node', 
        output='screen'
    )
    return LaunchDescription([
        waypoint_node
    ])