// 发布者框架
#include "rclcpp/rclcpp.hpp"
// #include "std_msgs/msg/string.hpp"
#include "base_interfaces/msg/student.hpp"

using namespace std::chrono_literals;

class Talker : public rclcpp::Node
{
public:
  Talker() : Node("talker_node_cpp"), count_(0)
  {
    RCLCPP_INFO(this->get_logger(), "发布：节点创建");
    // publisher_ = this->create_publisher<std_msgs::msg::String>("chatter", 10);
    publisher_ = this->create_publisher<base_interfaces::msg::Student>("chatter", 10);
    timer_ = this->create_wall_timer(1s, std::bind(&Talker::timer_callback, this));
  }

private:
  void timer_callback()
  {
    // auto message = std_msgs::msg::String();
    // message.data = "helloworld " + std::to_string(count_++);
    // RCLCPP_INFO(this->get_logger(), "发布: '%s'", message.data.c_str());
    // publisher_->publish(message);

    auto message = base_interfaces::msg::Student();
    message.name = "helloworld";
    message.age = static_cast<int32_t>(count_++);
    message.height = 1.75;
    RCLCPP_INFO(this->get_logger(), "发布: name=%s age=%d height=%.2f",
                message.name.c_str(), message.age, message.height);
    publisher_->publish(message);
  }

  // rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::Publisher<base_interfaces::msg::Student>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  size_t count_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Talker>());
  rclcpp::shutdown();
  return 0;
}
