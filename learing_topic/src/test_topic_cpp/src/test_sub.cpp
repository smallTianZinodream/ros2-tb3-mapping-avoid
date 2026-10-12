#include "rclcpp/rclcpp.hpp"
// #include "std_msgs/msg/string.hpp"
#include "base_interfaces/msg/student.hpp"

class Listener : public rclcpp::Node
{
public:
  Listener() : Node("listener")
  {
    // 创建订阅者，话题名为 "chatter"，收到消息后调用回调函数
    // subscription_ = this->create_subscription<std_msgs::msg::String>(
    //   "chatter", 10, std::bind(&Listener::topic_callback, this, std::placeholders::_1));
    subscription_ = this->create_subscription<base_interfaces::msg::Student>(
      "chatter", 10, std::bind(&Listener::topic_callback, this, std::placeholders::_1));
  }

private:
  // void topic_callback(const std_msgs::msg::String::SharedPtr msg) const
  // {
  //   RCLCPP_INFO(this->get_logger(), "I heard: '%s'", msg->data.c_str());
  // }
  void topic_callback(const base_interfaces::msg::Student::SharedPtr msg) const
  {
    RCLCPP_INFO(this->get_logger(), "I heard: name=%s age=%d height=%.2f",
                msg->name.c_str(), msg->age, msg->height);
  }

  // rclcpp::Subscription<std_msgs::msg::String>::SharedPtr subscription_;
  rclcpp::Subscription<base_interfaces::msg::Student>::SharedPtr subscription_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<Listener>());
  rclcpp::shutdown();
  return 0;
}
