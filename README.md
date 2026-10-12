# ROS 2 机器人扫图、自动巡航与避障仿真

在 Gazebo 里启动 TurtleBot3 Burger，用激光雷达边走边建图，并在前方有障碍时转向。仓库里同时放了一套 ROS 2 通信练习：自定义接口、话题、服务、动作、参数和 launch 文件。

工作空间目录是 `learing_topic/`（colcon 工作空间）。根目录的 `map.yaml` 和 `map.pgm` 是一次仿真建图保存下来的地图。

## 环境

建议在 Ubuntu 上使用 **ROS 2 Jazzy**（速度指令使用 `geometry_msgs/TwistStamped`，仿真使用 Gazebo `gz sim`）。需要预先安装：

- ROS 2 桌面版
- [TurtleBot3](https://github.com/ROBOTIS-GIT/turtlebot3) 仿真与 Cartographer 包：`turtlebot3_gazebo`、`turtlebot3_cartographer`、`turtlebot3_teleop`
- Nav2 的 `nav2_map_server`（自动建图结束时用它保存地图）
- `turtlesim`（仅 launch 练习需要）

```bash
sudo apt update
sudo apt install ros-jazzy-desktop ros-jazzy-nav2-map-server ros-jazzy-turtlesim
```

TurtleBot3 相关包按官方说明安装到同一工作空间，或使用系统已安装的发行包。启动前设置模型：

```bash
export TURTLEBOT3_MODEL=burger
```

## 编译

```bash
cd learing_topic
source /opt/ros/jazzy/setup.bash
colcon build
source install/setup.bash
```

之后每个新终端都要重新 `source install/setup.bash`。

## 仿真建图与避障

### 自动巡航并保存地图

一条命令拉起 Gazebo 世界、Cartographer 建图和自动行驶节点。默认行驶约 180 秒后，把地图写到 `~/map`（生成 `map.yaml` 与 `map.pgm`）。

```bash
ros2 launch tb3_bringup tb3_auto_slam.launch.py
```

可改时长和保存路径（路径不要带扩展名）：

```bash
ros2 launch tb3_bringup tb3_auto_slam.launch.py duration_sec:=120.0 map_path:=$HOME/my_map
```

行驶规则在 `auto_mapper` 里：前方 0.5 米内有障碍就原地转向更空的一侧，否则以 0.15 m/s 直行。

### 键盘手动建图

同样启动仿真和 Cartographer，并用 `gnome-terminal` 打开键盘遥控（`w/a/s/d`）。

```bash
ros2 launch tb3_bringup tb3_slam.launch.py
```

手动保存地图：

```bash
ros2 run nav2_map_server map_saver_cli -f ~/map --ros-args -p use_sim_time:=true
```

### 激光避障

`tb3_avoid_cpp` 订阅 `/scan`，发布 `/cmd_vel`。前方 ±30° 内距离小于 `stop_distance`（默认 0.5 米）时停车，并向左右两侧中更空的一侧转弯。

先让仿真里的机器人发布激光和接收速度，再另开终端：

```bash
source learing_topic/install/setup.bash
ros2 launch tb3_avoid_cpp avoid.launch.py
```

可调参数：`stop_distance`、`linear_speed`（默认 0.15）、`angular_speed`（默认 0.8）。

## 功能包

| 包 | 作用 |
| --- | --- |
| `tb3_bringup` | Gazebo + Cartographer 一键启动；`auto_mapper` 自动行驶并保存地图 |
| `tb3_avoid_cpp` | 基于激光扇区的避障节点 |
| `base_interfaces` | 自定义消息 `Student`、服务 `Addnum`、动作 `Progress` |
| `test_topic_cpp` | 发布 / 订阅 `Student`（话题 `chatter`） |
| `test_service_cpp` | 两数相加服务（服务名 `add_nums`） |
| `test_action_cpp` | `Progress` 动作的服务端与客户端 |
| `param_api_cpp` | 参数的增删改查，以及参数客户端 |
| `launch_py` | turtlesim 的 Python / XML / YAML launch 示例 |

## 通信练习

先编译并 source，再开两个终端。

话题：

```bash
ros2 run test_topic_cpp test_pub
ros2 run test_topic_cpp test_sub
```

服务（客户端示例参数以节点内实现为准，也可直接调用服务）：

```bash
ros2 run test_service_cpp demo01_server
ros2 run test_service_cpp demo02_client
ros2 service call /add_nums base_interfaces/srv/Addnum "{num1: 1, num2: 2}"
```

动作：

```bash
ros2 run test_action_cpp action_server
ros2 run test_action_cpp action_client
```

参数：

```bash
ros2 run param_api_cpp param_api_cpp
ros2 run param_api_cpp param_client
```

Launch（在 `launch_py` 包内，按子目录区分三种写法）：

```bash
ros2 launch launch_py py/turtlesim_launch.py
ros2 launch launch_py xml/turtlesim_launch.xml
ros2 launch launch_py yaml/turtlesim_launch.yaml
```

同目录下还有参数、分组命名空间和文件包含的示例：`param_launch`、`group_launch`、`include_launch`。

## 目录

```text
.
├── map.yaml / map.pgm          # 一次仿真保存的地图
└── learing_topic/
    └── src/
        ├── tb3_bringup/
        ├── tb3_avoid_cpp/
        ├── base_interfaces/
        ├── test_topic_cpp/
        ├── test_service_cpp/
        ├── test_action_cpp/
        ├── param_api_cpp/
        └── launch_py/
```

`build/`、`install/`、`log/` 由 colcon 生成，已在 `.gitignore` 中忽略，克隆后需要重新编译。
