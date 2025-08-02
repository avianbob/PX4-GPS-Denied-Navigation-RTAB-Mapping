import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # Shared parameters
    common_parameters = [{
        'subscribe_depth': True,
        'subscribe_rgbd': False,
        'subscribe_stereo': False,
        'subscribe_odom_info': True,
        'approx_sync': True,
        'subscribe_imu': True,
        'use_sim_time': True,
        'always_check_imu_tf': True,
        'load_db': False,
        'wait_imu_to_init': True,
        'approx_sync_max_interval': 0.01
        #'queue_size': 100,
        #'sync_queue_size': 100,
    }]

    return LaunchDescription([

        # RTAB-Map Odometry Node
        Node(
            package='rtabmap_odom',
            executable='rgbd_odometry',
            output='screen',
            parameters=common_parameters + [{
                'frame_id': 'x500_depth_0/base_footprint',
                'odom_frame_id': 'x500_depth_0/odom',
                'publish_tf': False
            }],
            remappings=[
                ('rgb/image', '/rgb/image'),
                ('rgb/camera_info', '/rgb/camera_info'),
                ('depth/image', '/depth/image'),
                ('odom_px4', '/odom_px4'),
                ('imu', '/imu')
            ]
        ),

        # RTAB-Map SLAM Node
        Node(
            package='rtabmap_slam',
            executable='rtabmap',
            output='screen',
            parameters=common_parameters + [{
                'frame_id': 'x500_depth_0/base_footprint',
                'map_frame_id': 'map',
                'odom_frame_id': 'x500_depth_0/odom',
                'publish_tf': True,
                'odom_sensor_sync': True
            }],
            remappings=[
                ('rgb/image', '/rgb/image'),
                ('rgb/camera_info', '/rgb/camera_info'),
                ('depth/image', '/depth/image'),
                ('odom_px4', '/odom_px4'),
                ('imu', '/imu')
            ]
        ),

        # RTAB-Map Visualization Node
        Node(
            package='rtabmap_viz',
            executable='rtabmap_viz',
            output='screen',
            parameters=common_parameters,
            remappings=[
                ('rgb/image', '/rgb/image'),
                ('rgb/camera_info', '/rgb/camera_info'),
                ('depth/image', '/depth/image'),
                ('odom_px4', '/odom_px4')
            ]
        ),

        Node(
            package='rviz2',
            executable='rviz2',
            parameters=common_parameters,
            arguments=['-d', os.path.join(
                get_package_share_directory('rtabmap_rviz_plugins'), 'launch', 'rtabmap.rviz')],
            output='screen'
        )
    ])