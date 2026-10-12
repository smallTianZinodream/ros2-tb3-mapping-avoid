#include <cmath>
#include <limits>
#include <string>

#include "geometry_msgs/msg/twist_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

class AvoidNode : public rclcpp::Node
{
public:
  AvoidNode() : Node("avoid_node")
  {
    stop_distance_ = this->declare_parameter("stop_distance", 0.5);
    linear_speed_ = this->declare_parameter("linear_speed", 0.15);
    angular_speed_ = this->declare_parameter("angular_speed", 0.8);

    cmd_pub_ = this->create_publisher<geometry_msgs::msg::TwistStamped>("cmd_vel", 10);
    scan_sub_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
      "scan", rclcpp::SensorDataQoS(),
      std::bind(&AvoidNode::on_scan, this, std::placeholders::_1));

    RCLCPP_INFO(this->get_logger(), "避障节点已启动，前方小于 %.2f 米就转弯", stop_distance_);
  }

private:
  void on_scan(const sensor_msgs::msg::LaserScan::SharedPtr msg)
  {
    double front = range_in_sector(*msg, -30.0, 30.0);
    double left = range_in_sector(*msg, 30.0, 100.0);
    double right = range_in_sector(*msg, -100.0, -30.0);

    double linear = linear_speed_;
    double angular = 0.0;
    std::string action = "直行";
    if (front < stop_distance_) {
      linear = 0.0;
      angular = left >= right ? angular_speed_ : -angular_speed_;
      action = angular > 0.0 ? "左转" : "右转";
    }
    publish_speed(linear, angular);
    RCLCPP_INFO_THROTTLE(
      this->get_logger(), *this->get_clock(), 1000,
      "前方 %.2f 米，%s", front, action.c_str());
  }

  double range_in_sector(
    const sensor_msgs::msg::LaserScan & scan, double start_deg, double end_deg) const
  {
    const double start = start_deg * M_PI / 180.0;
    const double end = end_deg * M_PI / 180.0;
    double nearest = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < scan.ranges.size(); ++i) {
      const double angle = scan.angle_min + static_cast<double>(i) * scan.angle_increment;
      if (angle < start || angle > end) {
        continue;
      }
      const float distance = scan.ranges[i];
      if (!std::isfinite(distance)) {
        continue;
      }
      nearest = std::min(nearest, static_cast<double>(distance));
    }
    return nearest;
  }

  void publish_speed(double linear, double angular)
  {
    geometry_msgs::msg::TwistStamped message;
    message.header.stamp = this->now();
    message.header.frame_id = "base_link";
    message.twist.linear.x = linear;
    message.twist.angular.z = angular;
    cmd_pub_->publish(message);
  }

  double stop_distance_;
  double linear_speed_;
  double angular_speed_;
  rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr cmd_pub_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AvoidNode>());
  rclcpp::shutdown();
  return 0;
}
