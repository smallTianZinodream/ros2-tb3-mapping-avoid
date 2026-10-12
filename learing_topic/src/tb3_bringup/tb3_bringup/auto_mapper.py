import math
import os
import subprocess

import rclpy
from geometry_msgs.msg import TwistStamped
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import LaserScan


class AutoMapper(Node):
    def __init__(self):
        super().__init__('auto_mapper')
        self.declare_parameter('duration_sec', 180.0)
        self.declare_parameter('map_path', os.path.expanduser('~/map'))
        self.duration_sec = float(self.get_parameter('duration_sec').value)
        self.map_path = os.path.expanduser(str(self.get_parameter('map_path').value))

        self.front = float('inf')
        self.left = float('inf')
        self.right = float('inf')
        self.have_scan = False
        self.start = None
        self.done = False
        self.saved = False

        self.cmd_pub = self.create_publisher(TwistStamped, 'cmd_vel', 10)
        self.create_subscription(
            LaserScan, 'scan', self.on_scan, qos_profile_sensor_data)
        self.create_timer(0.1, self.on_timer)
        self.get_logger().info(
            f'自动建图已启动，约 {self.duration_sec:.0f} 秒后保存到 {self.map_path}')

    def on_scan(self, msg):
        front = left = right = float('inf')
        for index, distance in enumerate(msg.ranges):
            if not math.isfinite(distance):
                distance = msg.range_max
            angle = msg.angle_min + index * msg.angle_increment
            if abs(angle) < math.radians(30.0):
                front = min(front, distance)
            elif math.radians(30.0) <= angle < math.radians(100.0):
                left = min(left, distance)
            elif math.radians(-100.0) < angle <= math.radians(-30.0):
                right = min(right, distance)
        self.front = front
        self.left = left
        self.right = right
        self.have_scan = True

    def on_timer(self):
        if self.done:
            self.publish_speed(0.0, 0.0)
            self.save_map()
            return
        if not self.have_scan:
            return

        now = self.get_clock().now()
        if self.start is None and now.nanoseconds > 0:
            self.start = now
        if self.start is not None:
            elapsed = (now - self.start).nanoseconds / 1e9
            if elapsed >= self.duration_sec:
                self.done = True
                self.publish_speed(0.0, 0.0)
                self.get_logger().info('行驶结束，准备保存地图')
                return

        # 前方近就原地转向更空的一侧，否则直行。
        if self.front < 0.5:
            angular = 0.8 if self.left >= self.right else -0.8
            self.publish_speed(0.0, angular)
        else:
            self.publish_speed(0.15, 0.0)

    def publish_speed(self, linear, angular):
        message = TwistStamped()
        message.header.stamp = self.get_clock().now().to_msg()
        message.header.frame_id = 'base_link'
        message.twist.linear.x = linear
        message.twist.angular.z = angular
        self.cmd_pub.publish(message)

    def save_map(self):
        if self.saved:
            return
        self.saved = True
        self.get_logger().info(f'正在保存地图到 {self.map_path}')
        result = subprocess.run(
            [
                'ros2', 'run', 'nav2_map_server', 'map_saver_cli',
                '-f', self.map_path,
                '--ros-args', '-p', 'use_sim_time:=true',
                '-p', 'save_map_timeout:=20.0',
            ],
            capture_output=True,
            text=True,
            timeout=90,
            check=False,
        )
        if result.returncode == 0:
            self.get_logger().info(
                f'地图已保存：{self.map_path}.yaml 和 {self.map_path}.pgm')
        else:
            detail = (result.stderr or result.stdout or '').strip()
            self.get_logger().error(f'保存地图失败：{detail}')


def main():
    rclpy.init()
    node = AutoMapper()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()


if __name__ == '__main__':
    main()
