/*
  需求：编写服务端，接收两个整数并返回它们的和
  流程：
      1.包含头文件
      2.初始化 ROS2 客户端
      3.定义节点类
          3-1.创建服务端
          3-2.回调函数处理请求并响应
      4.调用 spin 函数
      5.释放资源
*/
#include "rclcpp/rclcpp.hpp"
#include "base_interfaces/srv/addnum.hpp"

using base_interfaces::srv::Addnum;

class AddServer : public rclcpp::Node
{
public:
  AddServer() : Node("add_server_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "服务端创建！");
    // 3-1.创建服务端，服务名为 add_nums
    server_ = this->create_service<Addnum>(
      "add_nums",
      std::bind(&AddServer::add_callback, this, std::placeholders::_1, std::placeholders::_2));
  }

private:
  // 3-2.回调函数：处理请求，填写响应
  void add_callback(
    const Addnum::Request::SharedPtr request,
    const Addnum::Response::SharedPtr response)
  {
    response->sum = request->num1 + request->num2;
    RCLCPP_INFO(this->get_logger(), "收到请求: num1=%d, num2=%d", request->num1, request->num2);
    RCLCPP_INFO(this->get_logger(), "响应结果: sum=%d", response->sum);
  }

  rclcpp::Service<Addnum>::SharedPtr server_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<AddServer>());
  rclcpp::shutdown();
  return 0;
}
