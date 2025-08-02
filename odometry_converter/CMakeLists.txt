cmake_minimum_required(VERSION 3.5)
project(odometry_converter)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(px4_msgs REQUIRED)
find_package(px4_ros_com REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)

find_package(tf2_ros REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(odom_ned_to_enu_node src/odom_ned_to_enu_node.cpp)

target_link_libraries(odom_ned_to_enu_node
  px4_ros_com::frame_transforms
)

ament_target_dependencies(odom_ned_to_enu_node
  rclcpp
  px4_msgs
  px4_ros_com
  Eigen3
  tf2
  tf2_geometry_msgs
  tf2_ros
  geometry_msgs
  nav_msgs
)

install(TARGETS
  odom_ned_to_enu_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()

