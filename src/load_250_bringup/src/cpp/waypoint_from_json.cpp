#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <ctime>
#include <memory>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <nav2_msgs/action/follow_waypoints.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @brief A class to handle waypoint following using ROS 2 actions.
 */
class WaypointClient : public rclcpp::Node {
public:
    /**
     * @brief Construct a new Waypoint Client object.
     */
    WaypointClient() : Node("waypoint_follower") {
        client_ = rclcpp_action::create_client<nav2_msgs::action::FollowWaypoints>(this, "follow_waypoints");
    }

    /**
     * @brief Send waypoints to the action server.
     */
    void sendWaypoints() {
        // Wait for the action server to become available
        while (!client_->wait_for_action_server(std::chrono::seconds(2))) {
            RCLCPP_INFO(get_logger(), "Action server not available, waiting...");
            if (!rclcpp::ok()) {
                return;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Read waypoints from the JSON file
        std::vector<geometry_msgs::msg::PoseStamped> waypoints = readWaypointsFromJson("marker_positions.json");
        if (waypoints.empty()) {
            RCLCPP_ERROR(get_logger(), "No waypoints loaded from JSON. Exiting.");
            return;
        }

        // Create the goal message
        auto goal_msg = nav2_msgs::action::FollowWaypoints::Goal();
        goal_msg.poses = waypoints;

        RCLCPP_INFO(get_logger(), "Sending goal with %lu waypoints", waypoints.size());

        // Send the goal to the action server
        auto send_goal_options = rclcpp_action::Client<nav2_msgs::action::FollowWaypoints>::SendGoalOptions();
        send_goal_options.feedback_callback =
            std::bind(&WaypointClient::feedback_callback, this, std::placeholders::_1, std::placeholders::_2);
        send_goal_options.result_callback =
            std::bind(&WaypointClient::result_callback, this, std::placeholders::_1);

        auto goal_handle_future = client_->async_send_goal(goal_msg, send_goal_options);
    }

private:
    /**
     * @brief Read waypoints from a JSON file.
     * 
     * @param filename The name of the JSON file.
     * @return std::vector<geometry_msgs::msg::PoseStamped> A vector of waypoints.
     */
    /**
     * @brief Read waypoints from a JSON file.
     * 
     * This function reads waypoints from a JSON file and converts them into a vector of PoseStamped messages.
     * The JSON file is expected to have a "markers" array, where each marker contains position and orientation data.
     * 
     * @param filename The name of the JSON file.
     * @return std::vector<geometry_msgs::msg::PoseStamped> A vector of waypoints.
     */
    std::vector<geometry_msgs::msg::PoseStamped> readWaypointsFromJson(const std::string& filename) {
        std::vector<geometry_msgs::msg::PoseStamped> waypoints;
        std::ifstream file(filename);

        // Check if the file is open
        if (file.is_open()) {
            try {
                json j;
                file >> j;  // Read the JSON file into the json object

                RCLCPP_INFO(this->get_logger(), "Reading waypoints from %s", filename.c_str());

                // Check if the JSON has a "markers" array
                if (j.contains("markers") && j["markers"].is_array()) {
                    for (const auto& marker : j["markers"]) {
                        geometry_msgs::msg::PoseStamped pose;
                        pose.header.stamp = this->get_clock()->now();  // Set the current time
                        pose.header.frame_id = "map";  // Set the frame ID

                        // Explicitly convert JSON strings to doubles using std::stod
                        try {
                            pose.pose.position.x = std::stod(marker["position"]["x"].get<std::string>());
                            pose.pose.position.y = std::stod(marker["position"]["y"].get<std::string>());
                            pose.pose.position.z = std::stod(marker["position"]["z"].get<std::string>());
                            pose.pose.orientation.x = std::stod(marker["orientation"]["x"].get<std::string>());
                            pose.pose.orientation.y = std::stod(marker["orientation"]["y"].get<std::string>());
                            pose.pose.orientation.z = std::stod(marker["orientation"]["z"].get<std::string>());
                            pose.pose.orientation.w = std::stod(marker["orientation"]["w"].get<std::string>());

                            waypoints.push_back(pose);  // Add the pose to the waypoints vector
                        } catch (const std::exception& e) {
                            RCLCPP_ERROR(this->get_logger(), "Error converting JSON value to double: %s", e.what());
                            continue;  // Skip this marker and move to the next one
                        }
                    }
                } else {
                    RCLCPP_ERROR(this->get_logger(), "JSON file does not contain a 'markers' array.");
                }
            } catch (const json::parse_error& e) {
                RCLCPP_ERROR(this->get_logger(), "JSON parse error: %s", e.what());
            }
            file.close();  // Close the file
        } else {
            RCLCPP_ERROR(this->get_logger(), "Failed to open file %s for reading", filename.c_str());
        }

        return waypoints;  // Return the vector of waypoints
    }

    /**
     * @brief Callback function for feedback from the action server.
     * 
     * @param goal_handle The goal handle.
     * @param feedback The feedback from the action server.
     */
    void feedback_callback(
        rclcpp_action::ClientGoalHandle<nav2_msgs::action::FollowWaypoints>::SharedPtr,
        const std::shared_ptr<const nav2_msgs::action::FollowWaypoints::Feedback> feedback) {
        RCLCPP_INFO(
            get_logger(), "Received feedback: current waypoint %d", (int)feedback->current_waypoint);
    }

    /**
     * @brief Callback function for the result from the action server.
     * 
     * @param result The result from the action server.
     */
    void result_callback(const rclcpp_action::ClientGoalHandle<nav2_msgs::action::FollowWaypoints>::WrappedResult& result) {
        switch (result.code) {
        case rclcpp_action::ResultCode::SUCCEEDED:
            RCLCPP_INFO(get_logger(), "Goal succeeded");
            break;
        case rclcpp_action::ResultCode::ABORTED:
            RCLCPP_ERROR(get_logger(), "Goal was aborted");
            break;
        case rclcpp_action::ResultCode::CANCELED:
            RCLCPP_ERROR(get_logger(), "Goal was canceled");
            break;
        default:
            RCLCPP_ERROR(get_logger(), "Unknown result code");
            break;
        }

        RCLCPP_INFO(get_logger(), "Missed Waypoints: %lu", result.result->missed_waypoints.size());
        rclcpp::shutdown();
    }

    rclcpp_action::Client<nav2_msgs::action::FollowWaypoints>::SharedPtr client_;
};

/**
 * @brief Main function to initialize the ROS 2 node and send waypoints.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Exit status.
 */
int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<WaypointClient>();
    node->sendWaypoints();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
