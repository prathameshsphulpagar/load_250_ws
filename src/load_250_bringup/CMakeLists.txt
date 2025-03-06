cmake_minimum_required(VERSION 3.5)

project(load_250_bringup)

find_package(ament_cmake REQUIRED)
find_package(load_250_description REQUIRED)
find_package(load_250_gazebo REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclpy REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(action_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(tf2_sensor_msgs REQUIRED)
find_package(nav2_util REQUIRED)
find_package(nav2_common REQUIRED)
find_package(nav2_costmap_2d REQUIRED)
find_package(nlohmann_json REQUIRED)
install(
  DIRECTORY launch config src include
  DESTINATION share/${PROJECT_NAME}
)

add_executable(three_sample_waypoint_cpp src/cpp/three_sample_waypoint.cpp)
ament_target_dependencies(three_sample_waypoint_cpp rclcpp geometry_msgs std_msgs rclcpp_action nav2_msgs)

add_executable(marker_reader src/cpp/marker_read.cpp)
ament_target_dependencies(marker_reader rclcpp visualization_msgs nlohmann_json) 

add_executable(marker_writer src/cpp/marker_write.cpp)
ament_target_dependencies(marker_writer rclcpp visualization_msgs nlohmann_json) 

add_executable(waypoint_from_json src/cpp/waypoint_from_json.cpp)
ament_target_dependencies(waypoint_from_json rclcpp geometry_msgs std_msgs rclcpp_action nav2_msgs visualization_msgs nlohmann_json)

install(TARGETS
  three_sample_waypoint_cpp
  marker_reader
  marker_writer
  waypoint_from_json
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
