#include <rclcpp/rclcpp.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

class MarkerWriter : public rclcpp::Node {
public:
    MarkerWriter() : Node("marker_writer") {
        // Read JSON from file and print its contents
        readJsonFromFile("marker_positions.json");
    }

private:
    void readJsonFromFile(const std::string &filename) {
        std::ifstream file(filename);
        if (file.is_open()) {
            json j;
            file >> j;

            RCLCPP_INFO(this->get_logger(), "Reading marker positions from %s", filename.c_str());

            for (const auto &marker : j) {
                int id = marker["id"];
                double x = marker["position"]["x"];
                double y = marker["position"]["y"];
                double z = marker["position"]["z"];

                RCLCPP_INFO(this->get_logger(), "Marker ID: %d, Position: (%f, %f, %f)", id, x, y, z);
            }
        } else {
            RCLCPP_ERROR(this->get_logger(), "Failed to open file %s for reading", filename.c_str());
        }
    }
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<MarkerWriter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
