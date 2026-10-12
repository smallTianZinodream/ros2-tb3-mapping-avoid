/*
  需求：编写动作客户端，提交 enable=true，接收连续反馈 state 与最终结果 finish。
  代码结构：
    1.包含头文件
    2.初始化ROS2客户端
    3.自定义节点类
      3-1.创建动作客户端对象
      3-2.发送请求
      3-3.处理目标响应
      3-4.处理连续反馈
      3-5.处理最终结果
    4.调用spin函数，并且传入节点对象指针
    5.释放资源
*/
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "base_interfaces/action/progress.hpp"

using base_interfaces::action::Progress;
using GoalHandleProgress = rclcpp_action::ClientGoalHandle<Progress>;
using namespace std::placeholders;
using namespace std::chrono_literals;

class ProgressActionClient : public rclcpp::Node
{
public:
  ProgressActionClient() : Node("progress_action_client_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "客户端创建!");
    // 3-1.创建动作客户端对象
    this->client_ = rclcpp_action::create_client<Progress>(this, "progress");
  }

  // 3-2.发送请求
  void send_goal(bool enable)
  {
    if (!this->client_->wait_for_action_server(5s)) {
      RCLCPP_ERROR(this->get_logger(), "动作服务端未上线!");
      return;
    }

    auto goal_msg = Progress::Goal();
    goal_msg.enable = enable;

    RCLCPP_INFO(this->get_logger(), "发送目标: enable=%d", enable);
    auto send_goal_options = rclcpp_action::Client<Progress>::SendGoalOptions();
    send_goal_options.goal_response_callback =
      std::bind(&ProgressActionClient::goal_response_callback, this, _1);
    send_goal_options.feedback_callback =
      std::bind(&ProgressActionClient::feedback_callback, this, _1, _2);
    send_goal_options.result_callback =
      std::bind(&ProgressActionClient::result_callback, this, _1);
    this->client_->async_send_goal(goal_msg, send_goal_options);
  }

private:
  rclcpp_action::Client<Progress>::SharedPtr client_;

  // 3-3.处理目标响应
  void goal_response_callback(GoalHandleProgress::SharedPtr goal_handle)
  {
    if (!goal_handle) {
      RCLCPP_ERROR(this->get_logger(), "目标被拒绝!");
    } else {
      RCLCPP_INFO(this->get_logger(), "目标被接收，等待执行结果...");
    }
  }

  // 3-4.处理连续反馈
  void feedback_callback(
    GoalHandleProgress::SharedPtr,
    const std::shared_ptr<const Progress::Feedback> feedback)
  {
    RCLCPP_INFO(this->get_logger(), "收到反馈: state=%d", feedback->state);
  }

  // 3-5.处理最终结果
  void result_callback(const GoalHandleProgress::WrappedResult & result)
  {
    switch (result.code) {
      case rclcpp_action::ResultCode::SUCCEEDED:
        break;
      case rclcpp_action::ResultCode::ABORTED:
        RCLCPP_ERROR(this->get_logger(), "动作被中止!");
        return;
      case rclcpp_action::ResultCode::CANCELED:
        RCLCPP_ERROR(this->get_logger(), "动作被取消!");
        return;
      default:
        RCLCPP_ERROR(this->get_logger(), "未知结果码!");
        return;
    }
    RCLCPP_INFO(this->get_logger(), "动作成功! finish=%d", result.result->finish);
  }
};

int main(int argc, char const * argv[])
{
  // 2.初始化ROS2客户端
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ProgressActionClient>();
  node->send_goal(true);
  // 4.调用spin函数，并且传入节点对象指针
  rclcpp::spin(node);
  // 5.释放资源
  rclcpp::shutdown();
  return 0;
}
