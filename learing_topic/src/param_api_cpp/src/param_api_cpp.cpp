#include "rclcpp/rclcpp.hpp"

class ParamAPI : public rclcpp::Node
{
public:
  ParamAPI()
  : Node(
      "param_server_node_cpp",
      rclcpp::NodeOptions()
        .allow_undeclared_parameters(true)
        .automatically_declare_parameters_from_overrides(true))
  {
    RCLCPP_INFO(this->get_logger(), "参数服务端");
  }

  // 增
  void addParam()
  {
    RCLCPP_INFO(this->get_logger(), "-----增-----");
    this->declare_parameter("car_name", "xiaomi");
    this->declare_parameter("size", 5.11);
    this->declare_parameter("wheels", 4);

    // set_parameter() 也可以用来增加参数
    // 但是依赖 rclcpp::NodeOptions().allow_undeclared_parameters(true)
    // this->set_parameter(rclcpp::Parameter("width", 2));
  }

  // 查
  void selectParam()
  {
    RCLCPP_INFO(this->get_logger(), "-----查-----");
    // 获取指定参数
    RCLCPP_INFO(
      this->get_logger(), "car_name: %s",
      this->get_parameter("car_name").as_string().c_str());
    // 获取多个参数
    auto params = this->get_parameters({"car_name", "size", "wheels"});
    for (auto && param : params) {
      RCLCPP_INFO(
        this->get_logger(), "name: %s, value: %s",
        param.get_name().c_str(), param.value_to_string().c_str());
    }
    // 判断包含
    RCLCPP_INFO(this->get_logger(), "是否包含car_name: %d", this->has_parameter("car_name"));
    RCLCPP_INFO(this->get_logger(), "是否包含size: %d", this->has_parameter("size"));
    RCLCPP_INFO(this->get_logger(), "是否包含wheels: %d", this->has_parameter("wheels"));
    RCLCPP_INFO(this->get_logger(), "是否包含width: %d", this->has_parameter("width"));
  }

  // 改
  void updateParam()
  {
    // RCLCPP_INFO(this->get_logger(), "-----改-----");
    // RCLCPP_INFO(this->get_logger(), "更改之前size: %.2f", this->get_parameter("size").as_double());
    // this->set_parameter(rclcpp::Parameter("size", 6.22));
    // RCLCPP_INFO(this->get_logger(), "更改之后size: %.2f", this->get_parameter("size").as_double());
  }

  // 删
  void deleteParam()
  {
    // RCLCPP_INFO(this->get_logger(), "-----删-----");
    // this->undeclare_parameter("width");
    // RCLCPP_INFO(this->get_logger(), "是否包含width: %d", this->has_parameter("width"));
    // this->undeclare_parameter("wheels"); // 无法删除（静态类型参数）
    // RCLCPP_INFO(this->get_logger(), "是否包含wheels: %d", this->has_parameter("wheels"));
  }
};

int main(int argc, char const * argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ParamAPI>();
  node->addParam();
  node->selectParam();
  node->updateParam();
  node->deleteParam();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
