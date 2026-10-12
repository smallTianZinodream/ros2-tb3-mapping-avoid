import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, SetEnvironmentVariable, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node


def generate_launch_description():
    # 包含的官方 launch 在解析时就读取这个变量，所以要先写进当前进程。
    os.environ['TURTLEBOT3_MODEL'] = 'burger'
    os.environ['GZ_SIM_RENDER_ENGINE'] = 'ogre'
    os.environ['LIBGL_ALWAYS_SOFTWARE'] = '1'

    world_launch = os.path.join(
        get_package_share_directory('turtlebot3_gazebo'),
        'launch',
        'turtlebot3_world.launch.py',
    )
    cartographer_launch = os.path.join(
        get_package_share_directory('turtlebot3_cartographer'),
        'launch',
        'cartographer.launch.py',
    )

    # 等仿真和机器人起来后再开建图和键盘，避免抢在传感器前面启动。
    delayed_tools = TimerAction(
        period=8.0,
        actions=[
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(cartographer_launch),
                launch_arguments={'use_sim_time': 'true'}.items(),
            ),
            Node(
                package='turtlebot3_teleop',
                executable='teleop_keyboard',
                name='teleop_keyboard',
                output='screen',
                # 键盘节点必须在自己的窗口里，才能收到 w/a/s/d。
                prefix='gnome-terminal --wait --',
            ),
        ],
    )

    return LaunchDescription([
        SetEnvironmentVariable('TURTLEBOT3_MODEL', 'burger'),
        SetEnvironmentVariable('GZ_SIM_RENDER_ENGINE', 'ogre'),
        SetEnvironmentVariable('LIBGL_ALWAYS_SOFTWARE', '1'),
        IncludeLaunchDescription(PythonLaunchDescriptionSource(world_launch)),
        delayed_tools,
    ])
