from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(
            package='tb3_avoid_cpp',
            executable='avoid_node',
            name='avoid_node',
            output='screen',
            parameters=[{'use_sim_time': True}],
        ),
    ])
