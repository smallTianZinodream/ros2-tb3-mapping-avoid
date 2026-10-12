#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class ParamClient : public rclcpp::Node
{
public:
  ParamClient() : Node("param_client_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "参数服务客户端已经创建");
    // 创建参数客户端
    // 参数1：当前对象依赖的节点
    // 参数2：参数服务节点的名称
    param_client_ = std::make_shared<rclcpp::SyncParametersClient>(
      this, "param_server_node_cpp");
    /**
     * 问题：服务通信是通过话题关联的，为什么参数服务器中是使用服务端节点名称关联的？
     *    1. 参数服务端启动后，底层会封装多个服务通信的服务端
     *    2. 每个服务端的话题，都采用 /服务端节点名称/服务名称 的方式进行关联
     *    3. 参数客户端创建后，也会封装多个服务通信的客户端
     *    4. 每个客户端会通过 /服务端节点名称/服务名称 的方式进行关联（服务端和客户端相呼应）
     */
  }

  // 连接服务器端
  bool connect_server()
  {
    while (!param_client_->wait_for_service(1s)) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(this->get_logger(), "参数服务连接失败");
        return false;
      }
      RCLCPP_INFO(this->get_logger(), "连接中...");
    }
    RCLCPP_INFO(this->get_logger(), "参数服务连接成功");
    return true;
  }

  // 参数获取
  void get_param()
  {
    RCLCPP_INFO(this->get_logger(), "-----参数查询-----");
    // 获取指定参数
    std::string car_name = param_client_->get_parameter<std::string>("car_name");
    double size = param_client_->get_parameter<double>("size");
    int64_t wheels = param_client_->get_parameter<int64_t>("wheels");
    RCLCPP_INFO(this->get_logger(), "car_name: %s", car_name.c_str());
    RCLCPP_INFO(this->get_logger(), "size: %.2f", size);
    RCLCPP_INFO(this->get_logger(), "wheels: %ld", wheels);
    // 获取多个参数
    auto params = param_client_->get_parameters({"car_name", "size", "wheels"});
    for (auto && param : params) {
      RCLCPP_INFO(
        this->get_logger(), "name: %s, value: %s",
        param.get_name().c_str(), param.value_to_string().c_str());
    }
    // 判断参数是否存在
    RCLCPP_INFO(
      this->get_logger(), "是否包含car_name: %d",
      param_client_->has_parameter("car_name"));
    RCLCPP_INFO(
      this->get_logger(), "是否包含sizes: %d",
      param_client_->has_parameter("sizes"));
  }

  // 修改参数
  void update_param()
  {
    RCLCPP_INFO(this->get_logger(), "-----参数修改-----");
    param_client_->set_parameters({
      rclcpp::Parameter("car_name", "yu7"),
      rclcpp::Parameter("size", 7.10),
      rclcpp::Parameter("wheels", static_cast<int64_t>(5)),
      // 不存在的参数修改使用方式，是有依赖限制的（服务端需 allow_undeclared_parameters）
      rclcpp::Parameter("type", "suv")
    });
    std::string type = param_client_->get_parameter<std::string>("type");
    RCLCPP_INFO(this->get_logger(), "type: %s", type.c_str());
  }

private:
  rclcpp::SyncParametersClient::SharedPtr param_client_;
};

int main(int argc, char const * argv[])
{
  rclcpp::init(argc, argv);
  // 创建自定义节点类对象
  auto client = std::make_shared<ParamClient>();
  // 调用连接参数服务
  bool flag = client->connect_server();
  if (!flag) {
    return 0;
  }
  // 查询参数
  client->get_param();
  // 参数修改
  client->update_param();
  client->get_param();
  rclcpp::shutdown();
  return 0;
}
