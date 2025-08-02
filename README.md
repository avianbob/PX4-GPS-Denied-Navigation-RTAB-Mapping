# PX4-GPS-Denied-Navigation-RTAB-Mapping

*A complete simulation and SLAM stack for autonomous drone navigation in GPS-denied environments using PX4, ROS 2, RTAB-Map, and Gazebo Harmonic.*
---

## Table of Contents
- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [1. Setting Up Workspaces](#1-setting-up-workspaces)
- [2. PX4 Model Integration](#2-px4-model-integration)
- [3. Terminal Launch Breakdown](#3-terminal-launch-breakdown)
- [4. Bridging and Remapping Topics](#4-bridging-and-remapping-topics)
- [5. RTAB-Map Launch (Terminal 10)](#5-rtab-map-launch-terminal-10)
- [6. ENU to NED Conversion for PX4](#6-enu-to-ned-conversion-for-px4)
- [7. Parameter Configuration for PX4](#7-parameter-configuration-for-px4)
- [8. TF Tree Reference](#8-tf-tree-reference)


## Overview

This repository integrates the following:

- PX4 Autopilot SITL with a depth camera drone.
- ROS 2 Gazebo Harmonic with full RTAB-Map support
- Visual-Inertial Odometry pipeline with ENU to NED conversion for PX4
- Custom odometry bridge to PX4.
- ROS 2 TF publishing and RTAB-Map mapping stack
- Multi-terminal orchestration for GPS-denied SLAM navigation
- Shell script for launching all bridge nodes efficiently

---

## Prerequisites

Make sure you clone and build the following repositories:
- [PX4-Autopilot](https://github.com/PX4/PX4-Autopilot)
- [px4_msgs](https://github.com/PX4/px4_ros_com)
- [rtabmap](https://github.com/introlab/rtabmap)
- [rtabmap_ros](https://github.com/introlab/rtabmap_ros)
- [ros_gz_bridge](https://github.com/gazebosim/ros_gz)

---

## 1. Setting Up Workspaces

### Clone the repo using:

  ```bash
  cd ~/ros2_ws
  mkdir -p src && cd src
  git clone https://github.com/avianbob/PX4-GPS-Denied-Navigation-RTAB-Mapping.git
  ```

### GPS_Denied_Navigation and odometry_converter

```bash
cd ~/ros2_ws
colcon build --packages-select GPS_Denied_Navigation odometry_converter
source install/setup.bash
```

---

## 2. PX4 Model Integration

The model `gz_x500_depth` is used. Copy the model directory provided in this repo to:

```bash
rm -rf ~/PX4-Autopilot/Tools/simulation/gz/models/x500_depth
cp -r ~/ros2_ws/src/Model_Files/x500_depth ~/PX4-Autopilot/Tools/simulation/gz/models/
```

This ensures PX4 launches the model correctly with depth camera and odometry plugins.

---

## 3. Terminal Launch Breakdown

You can use manual terminals or the provided shell script. 

Navigate to the script folder:

```bash
cd ~/ros2_ws/src/GPS_Denied_Navigation/scripts
chmod +x all_terminals.sh
./all_terminals.sh
```

This launches 8 terminals automatically.

---

### Manual Terminal Plan

| Terminal | Purpose |
|----------|---------|
| 1 | Run PX4 SITL: `make px4_sitl gz_x500_depth` |
| 2 | Run QGroundControl: `./QGroundControl.AppImage` |
| 3 | Run MicroXRCEAgent: `MicroXRCEAgent udp4 -p 8888` |
| 4–9 | Run `ros_gz_bridge` for 6 essential topics (see below) |
| 10 | Run Odometry Converter Node (RTAB Odometry to PX4): `python3 odom_to_px4_vision.py` |
| 11 | Launch RTAB-Map: see below |

---

## 4. Bridging and Remapping Topics

Use `ros_gz_bridge` to map the following Gazebo topics to ROS 2:

| Gazebo Topic | ROS 2 Topic |
|--------------|-------------|
| `/clock` | `/clock` |
| `/world/<world>/model/x500_depth_0/link/camera_link/sensor/IMX214/image` | `/rgb/image` |
| `/world/<world>/model/x500_depth_0/link/camera_link/sensor/IMX214/camera_info` | `/rgb/camera_info` |
| `/depth_camera` | `/depth/image` |
| `/world/<world>/model/x500_depth_0/link/base_link/sensor/imu_sensor/imu` | `/imu` |
| `/model/x500_depth_0/odometry` | `/odom_px4` |

Replace `<world>` with your actual Gazebo world name.

---

## 5. RTAB-Map Launch (Terminal 10)

```bash
ros2 launch GPS_Denied_Navigation depth_camera_simulation.launch.py \
  rgb_topic:=/rgb/image \
  camera_info_topic:=/rgb/camera_info \
  depth_topic:=/depth/image \
  odom_topic:=/odom_px4 \
  imu_topic:=/imu \
  use_sim_time:=true \
  subscribe_odom:=true \
  wait_imu_to_init:=true
```

---

## 6. ENU to NED Conversion for PX4

Use the script `odom_to_px4_vision.py` to convert `/odom` (ENU) from RTAB-Map into `/fmu/in/vehicle_visual_odometry` (NED) for PX4.

To run:

```bash
cd ~/ros2_ws/src/GPS_Denied_Navigation/scripts
chmod +x odom_to_px4_vision.py
python3 odom_to_px4_vision.py
```

This ensures PX4 can use visual-inertial pose estimates for control.

---

## 7. Parameter Configuration for PX4

Copy the PX4 parameters file from:

```
~/ros2_ws/src/GPS_Denied_Navigation/parameters.params
```

Load it into QGroundControl.

---

## 8. TF Tree Reference

Refer to:

```
~/ros2_ws/src/GPS_Denied_Navigation/TF Tree.pdf
```

For understanding TF between base_link, camera, and imu.

---
