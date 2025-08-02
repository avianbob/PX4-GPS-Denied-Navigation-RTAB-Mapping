
import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    common_parameters = [{
        'subscribe_depth': True,
        'subscribe_rgbd': False,
        'subscribe_stereo': False,
        'subscribe_odom_info': True,
        'subscribe_imu': True,
        'approx_sync': True,
        'use_sim_time': False,
        'always_check_imu_tf': True,
        'odom_sensor_sync': True,
        'frame_id': 'camera_link',
        'odom_frame_id': 'odom',
        'map_frame_id': 'map',
        'publish_tf': True,
        'reliability': 'best_effort', 
        'approx_sync_max_interval': 0.01,
        'wait_imu_to_init': False     
    }]

    return LaunchDescription([

        # RTAB-Map Odometry Node (RGB-D + IMU)
        Node(
            package='rtabmap_odom',
            executable='rgbd_odometry',
            output='screen',
            parameters=common_parameters + [{'publish_tf': True}],
            remappings=[
                ('rgb/image', '/camera/camera/color/image_raw'),
                ('rgb/camera_info', '/camera/camera/color/camera_info'),
                ('depth/image', '/camera/camera/depth/image_rect_raw'),
                ('odom', '/odom'),
                ('imu', '/camera/camera/imu')
            ]
        ),

        # RTAB-Map SLAM Node
        Node(
            package='rtabmap_slam',
            executable='rtabmap',
            output='screen',
            parameters=common_parameters + [{'subscribe_odom': True}],
                remappings=[
                ('rgb/image', '/camera/camera/color/image_raw'),
                ('rgb/camera_info', '/camera/camera/color/camera_info'),
                ('depth/image', '/camera/camera/depth/image_rect_raw'),
                ('odom', '/odom'),
                ('imu', '/camera/camera/imu')
            ]
        ),

        # RTAB-Map Visualization
        Node(
            package='rtabmap_viz',
            executable='rtabmap_viz',
            output='screen',
            parameters=common_parameters,
            remappings=[
                ('rgb/image', '/camera/camera/color/image_raw'),
                ('rgb/camera_info', '/camera/camera/color/camera_info'),
                ('depth/image', '/camera/camera/depth/image_rect_raw'),
                ('odom', '/odom')
            ]
        ),

        # RViz2 with RTAB-Map configuration
        Node(
            package='rviz2',
            executable='rviz2',
            parameters=[{'use_sim_time': False}],
            arguments=['-d', os.path.join(
                get_package_share_directory('rtabmap_rviz_plugins'), 'launch', 'rtabmap.rviz')],
            output='screen'
        )
    ])

