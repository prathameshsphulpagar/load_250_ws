#include "../../include/load_250_bringup/marker_read.h"     

/**
 * @brief Construct a new Marker Reader object.
 */
MarkerReader::MarkerReader() : Node("marker_reader") {
    subscription_ = this->create_subscription<visualization_msgs::msg::MarkerArray>(
        "/slam_toolbox/graph_visualization", 10,
        std::bind(&MarkerReader::topic_callback, this, std::placeholders::_1));

    // Get the start time when the node is constructed
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_c);
    std::stringstream datetime_stream;
    datetime_stream << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
    start_time_ = datetime_stream.str();
}

/**
 * @brief Callback function for processing received MarkerArray messages.
 * 
 * @param msg The received MarkerArray message.
 */
void MarkerReader::topic_callback(const visualization_msgs::msg::MarkerArray::SharedPtr msg) {
    RCLCPP_INFO(this->get_logger(), "Received MarkerArray with %lu markers", msg->markers.size());

    json marker_positions;
    marker_positions["start_time"] = start_time_;  // Use the pre-captured start time
    marker_positions["markers"] = json::array(); // Create a "markers" array inside the JSON

    for (const auto& marker_msg : msg->markers) {
        RCLCPP_INFO(this->get_logger(), "Marker ID: %d", marker_msg.id);

        // Store marker information in JSON
        json marker;
        marker["id"] = marker_msg.id;

        // Use std::fixed and std::setprecision to format the position values
        marker["position"] = {
            {"x", formatDecimal(marker_msg.pose.position.x)},
            {"y", formatDecimal(marker_msg.pose.position.y)},
            {"z", formatDecimal(marker_msg.pose.position.z)}
        };
        marker["orientation"] = {
            {"x", formatDecimal(marker_msg.pose.orientation.x)},
            {"y", formatDecimal(marker_msg.pose.orientation.y)},
            {"z", formatDecimal(marker_msg.pose.orientation.z)},
            {"w", formatDecimal(marker_msg.pose.orientation.w)}
        };
        marker_positions["markers"].push_back(marker);  // Append each marker to the "markers" array
    }

    // Save JSON to a file
    saveToJsonFile(marker_positions, "marker_positions.json");
}

/**
 * @brief Format a double value to a string with fixed decimal precision.
 * 
 * @param value The double value to format.
 * @return std::string The formatted string.
 */
std::string MarkerReader::formatDecimal(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;  // Set precision to 2
    return out.str();
}

/**
 * @brief Save JSON data to a file.
 * 
 * @param j The JSON data to save.
 * @param filename The name of the file to save the JSON data to.
 */
void MarkerReader::saveToJsonFile(const json& j, const std::string& filename) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);
        RCLCPP_INFO(this->get_logger(), "Marker positions saved to %s", filename.c_str());
    }
    else {
        RCLCPP_ERROR(this->get_logger(), "Failed to open file %s for writing", filename.c_str());
    }
}

/**
 * @brief Main function to initialize the ROS 2 node and process MarkerArray messages.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Exit status.
 */
int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MarkerReader>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
