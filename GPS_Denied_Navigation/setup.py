from setuptools import find_packages, setup

package_name = 'GPS_Denied_Navigation'

setup(
    name=package_name,
    version='0.0.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/launch', [
            'launch/depth_camera_hardware.launch.py',
            'launch/depth_camera_simulation.launch.py',
            'launch/odom_to_px4_vision.launch.py',
        ])
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='avianb',
    maintainer_email='avianb@todo.todo',
    description='Launch and config files for RTAB-Map integration',
    license='MIT',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
        ],
    },
)
