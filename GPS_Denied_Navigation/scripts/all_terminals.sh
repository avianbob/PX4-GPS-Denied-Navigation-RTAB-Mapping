#!/bin/bash

gnome-terminal --title="Micro XRCE Agent" -- bash -c "MicroXRCEAgent udp4 -p 8888; exec bash"
gnome-terminal --title="Clock Bridge" -- bash -c "ros2 run ros_gz_bridge parameter_bridge /clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock; exec bash"
gnome-terminal --title="RGB Image Bridge" -- bash -c "ros2 run ros_gz_bridge parameter_bridge /world/<world>/model/x500_depth_0/link/camera_link/sensor/IMX214/image@sensor_msgs/msg/Image[gz.msgs.Image --ros-args --remap /world/<world>/model/x500_depth_0/link/camera_link/sensor/IMX214/image:=/rgb/image; exec bash"
gnome-terminal --title="Camera Info Bridge" -- bash -c "ros2 run ros_gz_bridge parameter_bridge /world/<world>/model/x500_depth_0/link/camera_link/sensor/IMX214/camera_info@sensor_msgs/msg/CameraInfo[gz.msgs.CameraInfo --ros-args --remap /world/<world>/model/x500_depth_0/link/camera_link/sensor/IMX214/camera_info:=/rgb/camera_info; exec bash"
gnome-terminal --title="Depth Bridge" -- bash -c "ros2 run ros_gz_bridge parameter_bridge /depth_camera@sensor_msgs/msg/Image[gz.msgs.Image --ros-args --remap /depth_camera:=/depth/image; exec bash"
gnome-terminal --title="IMU Bridge" -- bash -c "ros2 run ros_gz_bridge parameter_bridge /world/<world>/model/x500_depth_0/link/base_link/sensor/imu_sensor/imu@sensor_msgs/msg/Imu[gz.msgs.IMU --ros-args --remap /world/<world>/model/x500_depth_0/link/base_link/sensor/imu_sensor/imu:=/imu; exec bash"
gnome-terminal --title="Odometry Bridge" -- bash -c "ros2 run ros_gz_bridge parameter_bridge /model/x500_depth_0/odometry@nav_msgs/msg/Odometry[gz.msgs.Odometry   --ros-args --remap /model/x500_depth_0/odometry:=/odom_px4; exec bash"
gnome-terminal --title="TF Publisher" -- bash -c "ros2 run odometry_converter odom_ned_to_enu_node; exec bash"


