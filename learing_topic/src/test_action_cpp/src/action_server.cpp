/*
  需求：编写动作通信，动作客户端提交一个 bool 数据 enable，动作服务端接收请求数据并
        根据 bool 数据决定是否开启运算；请求为 true 时模拟 360 度旋转，每累加 30 度
        反馈一次进度，最终结果返回给动作客户端。
  实现：
    1.创建动作服务对象
    2.处理提交的目标值
    3.生成连续反馈
    4.响应最终结果
    5.处理取消请求
  代码结构：
    1.包含头文件
    2.初始化ROS2客户端
    3.自定义节点类
      3-1.创建动作服务器端对象
      3-2.处理提交的目标值
      3-3.处理取消请求
      3-4.生成连续反馈与最终结果
    4.调用spin函数，并且传入节点对象指针
    5.释放资源
*/
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include "base_interfaces/action/progress.hpp"

#include <thread>

using base_interfaces::action::Progress;
using GoalHandleProgress = rclcpp_action::ServerGoalHandle<Progress>;
using namespace std::placeholders;

class ProgressActionServer : public rclcpp::Node
{
public:
  ProgressActionServer() : Node("progress_action_server_node_cpp")
  {
    RCLCPP_INFO(this->get_logger(), "服务端创建!");
    // 3-1.创建动作服务器端对象
    this->action_server_ = rclcpp_action::create_server<Progress>(
      this,
      "progress",
      std::bind(&ProgressActionServer::handle_goal, this, _1, _2),
      std::bind(&ProgressActionServer::handle_cancel, this, _1),
      std::bind(&ProgressActionServer::handle_accepted, this, _1));
  }

private:
  rclcpp_action::Server<Progress>::SharedPtr action_server_;

  // 3-2.处理提交的目标值
  rclcpp_action::GoalResponse handle_goal(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const Progress::Goal> goal)
  {
    (void)uuid;
    RCLCPP_INFO(this->get_logger(), "收到目标请求 enable=%d", goal->enable);
    if (!goal->enable) {
      RCLCPP_INFO(this->get_logger(), "enable 为 false，拒绝请求!");
      return rclcpp_action::GoalResponse::REJECT;
    }
    RCLCPP_INFO(this->get_logger(), "目标有效，接受请求!");
    return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
  }

  // 3-3.处理取消请求
  rclcpp_action::CancelResponse handle_cancel(
    const std::shared_ptr<GoalHandleProgress> goal_handle)
  {
    (void)goal_handle;
    RCLCPP_INFO(this->get_logger(), "收到取消请求，允许取消!");
    return rclcpp_action::CancelResponse::ACCEPT;
  }

  // 目标被接受后，新开线程执行，避免阻塞 spin
  void handle_accepted(const std::shared_ptr<GoalHandleProgress> goal_handle)
  {
    std::thread{std::bind(&ProgressActionServer::execute, this, goal_handle)}.detach();
  }

  // 3-4.生成连续反馈与最终结果
  void execute(const std::shared_ptr<GoalHandleProgress> goal_handle)
  {
    RCLCPP_INFO(this->get_logger(), "开始执行动作!");
    rclcpp::Rate loop_rate(1);
    auto feedback = std::make_shared<Progress::Feedback>();
    auto result = std::make_shared<Progress::Result>();

    // 模拟 0~360 度旋转，每 30 度反馈一次
    for (int i = 0; i <= 360 && rclcpp::ok(); i += 30) {
      if (goal_handle->is_canceling()) {
        result->finish = false;
        goal_handle->canceled(result);
        RCLCPP_INFO(this->get_logger(), "动作已取消!");
        return;
      }

      feedback->state = i;
      goal_handle->publish_feedback(feedback);
      RCLCPP_INFO(this->get_logger(), "连续反馈: state=%d", feedback->state);
      loop_rate.sleep();
    }

    if (rclcpp::ok()) {
      result->finish = true;
      goal_handle->succeed(result);
      RCLCPP_INFO(this->get_logger(), "动作完成! finish=%d", result->finish);
    }
  }
};

int main(int argc, char const * argv[])
{
  // 2.初始化ROS2客户端
  rclcpp::init(argc, argv);
  // 4.调用spin函数，并且传入节点对象指针
  rclcpp::spin(std::make_shared<ProgressActionServer>());
  // 5.释放资源
  rclcpp::shutdown();
  return 0;
}
