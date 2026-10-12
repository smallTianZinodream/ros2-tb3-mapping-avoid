/*
  需求：编写客户端，提交两个整数，接收并输出求和结果
  流程：
      1.包含头文件
      2.初始化 ROS2 客户端
      3.定义节点类
          3-1.创建客户端
          3-2.连接服务端
          3-3.组织请求数据并发送
      4.调用 spin_until_future_complete 等待响应
      5.释放资源
*/
#include "rclcpp/rclcpp.hpp"
#include "base_interfaces/srv/addnum.hpp"

using base_interfaces::srv::Addnum;
using namespace std::chrono_literals;

class AddClient : public rclcpp::Node
{
public:
  AddClient() : Node("add_client_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "客户端创建！");
    // 3-1.创建客户端
    client_ = this->create_client<Addnum>("add_nums");
  }

  // 3-2.连接服务端
  bool connect_server()
  {
    while (!client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "等待服务过程中客户端被打断...");
        return false;
      }
      RCLCPP_INFO(this->get_logger(), "服务连接中，请稍候...");
    }
    return true;
  }

  // 3-3.组织请求数据并发送
  rclcpp::Client<Addnum>::FutureAndRequestId send_request(int32_t num1, int32_t num2)
  {
    auto request = std::make_shared<Addnum::Request>();
    request->num1 = num1;
    request->num2 = num2;
    return client_->async_send_request(request);
  }

private:
  rclcpp::Client<Addnum>::SharedPtr client_;
};

int main(int argc, char * argv[])
{
  if (argc != 3) {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "请提交两个整型数字");
    return 1;
  }

  rclcpp::init(argc, argv);
  auto client = std::make_shared<AddClient>();

  // 连接服务端
  bool flag = client->connect_server();
  if (!flag) {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "服务连接失败，程序退出！");
    return 1;
  }

  // 发送请求
  auto response = client->send_request(atoi(argv[1]), atoi(argv[2]));

  // 处理响应
  if (rclcpp::spin_until_future_complete(client, response) == rclcpp::FutureReturnCode::SUCCESS) {
    RCLCPP_INFO(client->get_logger(), "响应成功！sum = %d", response.get()->sum);
  } else {
    RCLCPP_ERROR(client->get_logger(), "响应失败！");
  }

  rclcpp::shutdown();
  return 0;
}
