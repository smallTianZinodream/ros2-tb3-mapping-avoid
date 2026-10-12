import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, SetEnvironmentVariable, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    os.environ['TURTLEBOT3_MODEL'] = 'burger'
    os.environ['GZ_SIM_RENDER_ENGINE'] = 'ogre'
    os.environ['LIBGL_ALWAYS_SOFTWARE'] = '1'

    duration_sec = LaunchConfiguration('duration_sec')
    map_path = LaunchConfiguration('map_path')

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

    delayed_tools = TimerAction(
        period=8.0,
        actions=[
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(cartographer_launch),
                launch_arguments={'use_sim_time': 'true'}.items(),
            ),
            Node(
                package='tb3_bringup',
                executable='auto_mapper',
                name='auto_mapper',
                output='screen',
                parameters=[{
                    'use_sim_time': True,
                    'duration_sec': ParameterValue(duration_sec, value_type=float),
                    'map_path': map_path,
                }],
            ),
        ],
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            'duration_sec',
            default_value='180.0',
            description='自动行驶多少仿真秒后保存地图',
        ),
        DeclareLaunchArgument(
            'map_path',
            default_value=os.path.expanduser('~/map'),
            description='地图保存路径，不带扩展名',
        ),
        SetEnvironmentVariable('TURTLEBOT3_MODEL', 'burger'),
        SetEnvironmentVariable('GZ_SIM_RENDER_ENGINE', 'ogre'),
        SetEnvironmentVariable('LIBGL_ALWAYS_SOFTWARE', '1'),
        IncludeLaunchDescription(PythonLaunchDescriptionSource(world_launch)),
        delayed_tools,
    ])
