from launch import LaunchDescription
from launch.actions import ExecuteProcess, TimerAction
from launch_ros.actions import Node


def generate_launch_description():
    # 两个独立窗口的小乌龟（用命名空间避免话题冲突）
    turtle_win1 = Node(
        package='turtlesim',
        executable='turtlesim_node',
        name='sim',
        namespace='t1',
        output='screen'
    )
    turtle_win2 = Node(
        package='turtlesim',
        executable='turtlesim_node',
        name='sim',
        namespace='t2',
        output='screen'
    )

    # 持续发布角速度，让两只乌龟一直旋转
    spin1 = ExecuteProcess(
        cmd=[
            'ros2', 'topic', 'pub', '--rate', '10',
            '/t1/turtle1/cmd_vel',
            'geometry_msgs/msg/Twist',
            '{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 1.5}}'
        ],
        output='screen'
    )
    spin2 = ExecuteProcess(
        cmd=[
            'ros2', 'topic', 'pub', '--rate', '10',
            '/t2/turtle1/cmd_vel',
            'geometry_msgs/msg/Twist',
            '{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: -1.5}}'
        ],
        output='screen'
    )

    # 等 turtlesim 起来后再发速度
    return LaunchDescription([
        turtle_win1,
        turtle_win2,
        TimerAction(period=2.0, actions=[spin1, spin2]),
    ])
