#ifndef MARKER_READER_H
#define MARKER_READER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <chrono>
#include <ctime>

#include <rclcpp/rclcpp.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class MarkerReader : public rclcpp::Node {
public:
    MarkerReader(); // Constructor declaration

private:
    void topic_callback(const visualization_msgs::msg::MarkerArray::SharedPtr msg);
    std::string formatDecimal(double value);
    void saveToJsonFile(const json& j, const std::string& filename);

    rclcpp::Subscription<visualization_msgs::msg::MarkerArray>::SharedPtr subscription_;
    std::string start_time_;
};

#endif // MARKER_READER_H
