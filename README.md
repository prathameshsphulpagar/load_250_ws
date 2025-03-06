# **1. Software Information**
1)  Ros version = Ros2 Humble
2)  Gazebo = Ignition Fortress
3)  Simulations = Slam_toolbox
4)  Navigation = Nav2

# **2. How to launch the files**
1) In first terminal launch = For launching the robot simulation
```
ros2 launch load_250_bringup load_250.launch.py
```
  this will launch the following things
  1. Robot URDF and Rviz.
  2. Slam_toolbox
  3. navigation

2) In second terminal launch = For recording the marker array / robot movement points,
   and storing this files in jeson file name = "marker_positions.json"
   ```
   ros2 launch load_250_bringup read_load_250.launch.py
   ```
   This will launch the following things
   1. marker_read.cpp code present in /src/load_250_bringup/src/cpp
  
3) In third terminal launch = For launching the recoreded jeson points and move robot accordingly.
   ```
   ros2 launch load_250_bringup write_load_250.launch.py
   ```
   This will launch the following things
   1. waypoint_from_json.cpp code present in /src/load_250_bringup/src/cpp

# **3. Media Links**
1) Screen recording =
2) Screenshots =
3) mobail recording =
   
# **4. Discriptions**
1) **marker_read.cpp =**
   1. This node subscribe to "/slam_toolbox/graph_visualization".
   2. extract the position and orientation value out of it.
   3. store it to jeson file alonge with the starting node date and time.
      
2) **waypoint_from_json.cpp =**
   1. This node opens the Jeson files.
   2. Read the jeson co-ordinates (position ,orientation).
   3. Craete a service client and pass co-oridate to it.
      
# **3. Refrances Taken**
1. ros_gz_project_template = https://github.com/gazebosim/ros_gz_project_template.
2. For SDF creation = Fusion 360 software and "FusionSDF" for export sdf.
