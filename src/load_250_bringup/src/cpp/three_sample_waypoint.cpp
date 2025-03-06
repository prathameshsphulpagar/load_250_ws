#include <memory>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "nav2_msgs/action/follow_waypoints.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"

using namespace std::chrono_literals;  // Import literals like 2s

/**
 * @brief WaypointClient class to send waypoints to the action server
 */
class WaypointClient : public rclcpp::Node {
 public:
  /**
   * @brief Construct a new WaypointClient object
   */
  WaypointClient() : Node("waypoint_follower") {
    // Create an action client
    client_ = rclcpp_action::create_client<nav2_msgs::action::FollowWaypoints>(
        this, "follow_waypoints");
  }

  /**
   * @brief Send waypoints to the action server
   */
  void sendWaypoints() {
    // Wait for the action server to be available
    while (!client_->wait_for_action_server(2s)) {
      RCLCPP_INFO(get_logger(), "Action server not available, waiting...");
    }

    // Define waypoints
    std::vector<geometry_msgs::msg::PoseStamped> waypoints;
    geometry_msgs::msg::PoseStamped pose;

    pose.header.stamp = this->get_clock()->now();
    pose.header.frame_id = "map";
    pose.pose.position.x = 3.0;
    pose.pose.position.y = -3.0;
    pose.pose.position.z = 0.0;
    pose.pose.orientation.x = 0.0;
    pose.pose.orientation.y = 0.0;
    pose.pose.orientation.z = 0.0;
    pose.pose.orientation.w = 0.0;
    waypoints.push_back(pose);

    // Send the waypoints
    auto goal_msg = nav2_msgs::action::FollowWaypoints::Goal();
    goal_msg.poses = waypoints;

    RCLCPP_INFO(get_logger(), "Sending goal");

    auto send_goal_options =
        rclcpp_action::Client<nav2_msgs::action::FollowWaypoints>::SendGoalOptions();
    send_goal_options.feedback_callback =
        std::bind(&WaypointClient::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
    send_goal_options.result_callback =
        std::bind(&WaypointClient::result_callback, this, std::placeholders::_1);

    auto goal_handle_future = client_->async_send_goal(goal_msg, send_goal_options);
  }

 private:
  /**
   * @brief Feedback callback function
   * 
   * @param goal_handle Shared pointer to the goal handle
   * @param feedback Shared pointer to the feedback message
   */
  void feedback_callback(
      rclcpp_action::ClientGoalHandle<nav2_msgs::action::FollowWaypoints>::SharedPtr,
      const std::shared_ptr<const nav2_msgs::action::FollowWaypoints::Feedback> feedback)
  {
    RCLCPP_INFO(
        get_logger(), "Received feedback: current waypoint %d", feedback->current_waypoint);
  }

  /**
   * @brief Result callback function
   * 
   * @param result Wrapped result of the action
   */
  void result_callback(const rclcpp_action::ClientGoalHandle<nav2_msgs::action::FollowWaypoints>::WrappedResult & result)
  {
      switch (result.code) {
        case rclcpp_action::ResultCode::SUCCEEDED:
          break;
        case rclcpp_action::ResultCode::ABORTED:
          RCLCPP_ERROR(get_logger(), "Goal was aborted");
          return;
        case rclcpp_action::ResultCode::CANCELED:
          RCLCPP_ERROR(get_logger(), "Goal was canceled");
          return;
        default:
          RCLCPP_ERROR(get_logger(), "Unknown result code");
          return;
      }
      RCLCPP_INFO(get_logger(), "Goal succeeded");
      RCLCPP_INFO(get_logger(), "Missed Waypoints: %d", (int)result.result->missed_waypoints.size());
      rclcpp::shutdown();
  }

  std::shared_ptr<rclcpp_action::Client<nav2_msgs::action::FollowWaypoints>> client_;
};

/**
 * @brief Main function
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return int 
 */
int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<WaypointClient>();
  node->sendWaypoints();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

