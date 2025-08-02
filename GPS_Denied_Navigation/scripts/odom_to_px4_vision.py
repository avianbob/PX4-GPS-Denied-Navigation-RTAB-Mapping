#!/usr/bin/env python3
import numpy as np
if not hasattr(np, 'float'):
    np.float = float

import rclpy
from rclpy.node import Node
from nav_msgs.msg import Odometry
from px4_msgs.msg import VehicleOdometry
import tf_transformations

class OdomToPX4VisionNode(Node):
    def __init__(self):
        super().__init__('odom_to_px4_vision_node')

        self.subscription = self.create_subscription(
            Odometry,
            '/odom',
            self.odom_callback,
            10
        )

        self.publisher = self.create_publisher(
            VehicleOdometry,
            '/fmu/in/vehicle_vision_odometry',
            10
        )

        self.get_logger().info("Publishing /odom to /fmu/in/vehicle_vision_odometry with ENU → NED conversion")

    def odom_callback(self, msg: Odometry):
        odom_msg = VehicleOdometry()
        odom_msg.timestamp = self.get_clock().now().nanoseconds // 1000  # PX4 expects microseconds
        odom_msg.timestamp_sample = odom_msg.timestamp

        # Position: ENU → NED
        odom_msg.position = [
            msg.pose.pose.position.x,
            -msg.pose.pose.position.y,
            -msg.pose.pose.position.z,
        ]

        # Orientation: ENU → NED (flip Y and Z via quaternion conjugation)
        q_enu = [
            msg.pose.pose.orientation.x,
            msg.pose.pose.orientation.y,
            msg.pose.pose.orientation.z,
            msg.pose.pose.orientation.w,
        ]
        q_rot = [0, 1, 0, 0]  # 180° around Y
        q_ned = tf_transformations.quaternion_multiply(
            tf_transformations.quaternion_multiply(q_rot, q_enu),
            tf_transformations.quaternion_conjugate(q_rot)
        )
        odom_msg.q = [q_ned[0], q_ned[1], q_ned[2], q_ned[3]]  # [x, y, z, w]

        # Velocity: ENU → NED
        odom_msg.velocity = [
            msg.twist.twist.linear.x,
            -msg.twist.twist.linear.y,
            -msg.twist.twist.linear.z,
        ]

        # Angular velocity: ENU → NED
        odom_msg.angular_velocity = [
            msg.twist.twist.angular.x,
            -msg.twist.twist.angular.y,
            -msg.twist.twist.angular.z,
        ]

        # Covariances → use fixed low variance (or you can extract diagonals)
        odom_msg.position_variance = [0.01, 0.01, 0.01]
        odom_msg.orientation_variance = [0.01, 0.01, 0.01]
        odom_msg.velocity_variance = [0.01, 0.01, 0.01]

        # Metadata
        odom_msg.pose_frame = VehicleOdometry.POSE_FRAME_NED
        odom_msg.velocity_frame = VehicleOdometry.VELOCITY_FRAME_NED
        odom_msg.reset_counter = 0
        odom_msg.quality = 1

        self.publisher.publish(odom_msg)

def main(args=None):
    rclpy.init(args=args)
    node = OdomToPX4VisionNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
