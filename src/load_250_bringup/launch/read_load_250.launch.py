import os

from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():

    read_waypoint_node = Node(
        package='load_250_bringup',  
        executable='marker_reader', 
        name='marker_reader', 
        output='screen'
    )
    return LaunchDescription([
        read_waypoint_node
    ])