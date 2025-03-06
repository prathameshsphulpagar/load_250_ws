import os

from launch import LaunchDescription
from launch_ros.actions import Node

from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource

from ament_index_python.packages import get_package_share_directory
def generate_launch_description():

    load_250_bringup_file_dir = get_package_share_directory('load_250_bringup') 
    load_250_navigation_file_dir = get_package_share_directory('load_250_navigation') 
    load_250_slam_file_dir = get_package_share_directory('load_250_slam') 
    
    load_250_bringup_launch_file = os.path.join(load_250_bringup_file_dir, 'launch', 'robot_load_250.launch.py')
    include_load_250_bringup_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(load_250_bringup_launch_file)
    )
    load_250_slam_launch_file = os.path.join(load_250_slam_file_dir, 'launch', 'online_async_launch.py')
    include_load_250_slam_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(load_250_slam_launch_file)
    )
    
    load_250_navigation_launch_file = os.path.join(load_250_navigation_file_dir, 'launch', 'navigation_launch.py')
    include_load_250_navigation_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(load_250_navigation_launch_file)
    )
    

    return LaunchDescription([
        include_load_250_bringup_launch,
        include_load_250_navigation_launch,
        include_load_250_slam_launch

    ])
