/* FOR RGBD USING FAKE DEPTH
#include "rclcpp/rclcpp.hpp"
#include "px4_msgs/msg/vehicle_odometry.hpp"
#include "px4_ros_com/frame_transforms.h"
#include <Eigen/Dense>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/static_transform_broadcaster.h>

class OdometryConverterNode : public rclcpp::Node {
public:
    OdometryConverterNode()
        : Node("odometry_converter_node", rclcpp::NodeOptions())
    {
        if (!this->has_parameter("use_sim_time")) {
            this->declare_parameter("use_sim_time", true);
        }
        this->set_parameter(rclcpp::Parameter("use_sim_time", true));

        odom_sub_ = this->create_subscription<px4_msgs::msg::VehicleOdometry>(
            "/fmu/out/vehicle_odometry",
            rclcpp::SensorDataQoS(),
            std::bind(&OdometryConverterNode::odomCallback, this, std::placeholders::_1));

        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        static_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

        publishStaticTransforms();
    }


private:
    void publishStaticTransforms() {
        std::vector<geometry_msgs::msg::TransformStamped> static_transforms;

        rclcpp::Time zero_time(0, 0, this->get_clock()->get_clock_type());

        // Static: base_footprint -> camera_link
        geometry_msgs::msg::TransformStamped base_to_cam;
        base_to_cam.header.stamp = zero_time;
        base_to_cam.header.frame_id = "x500_mono_cam_down_0/base_footprint";
        base_to_cam.child_frame_id = "x500_mono_cam_down_0/camera_link";
        base_to_cam.transform.translation.x = 0.0;
        base_to_cam.transform.translation.y = 0.0;
        base_to_cam.transform.translation.z = 0.1;
        tf2::Quaternion cam_q;
        cam_q.setRPY(0.0, 0.0, 0.0);  // 90 deg pitch for downward-facing camera
        base_to_cam.transform.rotation.x = cam_q.x();
        base_to_cam.transform.rotation.y = cam_q.y();
        base_to_cam.transform.rotation.z = cam_q.z();
        base_to_cam.transform.rotation.w = cam_q.w();
        static_transforms.push_back(base_to_cam);

        // Static: camera_link -> imager
        geometry_msgs::msg::TransformStamped cam_to_imager;
        cam_to_imager.header.stamp = zero_time;  // static frame
        cam_to_imager.header.frame_id = "x500_mono_cam_down_0/camera_link";
        cam_to_imager.child_frame_id = "x500_mono_cam_down_0/camera_link/imager";
        cam_to_imager.transform.translation.x = 0.0;
        cam_to_imager.transform.translation.y = 0.0;
        cam_to_imager.transform.translation.z = 0.0;
        cam_to_imager.transform.rotation.x = 0.0;
        cam_to_imager.transform.rotation.y = 0.0;
        cam_to_imager.transform.rotation.z = 0.0;
        cam_to_imager.transform.rotation.w = 1.0;
        static_transforms.push_back(cam_to_imager);
        
        
        // Static: base_footprint -> imu_sensor
        geometry_msgs::msg::TransformStamped base_to_imu;
        base_to_imu.header.stamp = zero_time;
        base_to_imu.header.frame_id = "x500_mono_cam_down_0/base_footprint";
        base_to_imu.child_frame_id = "x500_mono_cam_down_0/base_link/imu_sensor";

// Set translation — update these values based on your sensor's real position
        base_to_imu.transform.translation.x = 0.0;
        base_to_imu.transform.translation.y = 0.0;
        base_to_imu.transform.translation.z = 0.0;  // Example offset

// Set rotation — assuming IMU is aligned with body frame
        base_to_imu.transform.rotation.x = 0.0;
        base_to_imu.transform.rotation.y = 0.0;
        base_to_imu.transform.rotation.z = 0.0;
        base_to_imu.transform.rotation.w = 1.0;

        static_transforms.push_back(base_to_imu);


        static_broadcaster_->sendTransform(static_transforms);
    }

    void odomCallback(const px4_msgs::msg::VehicleOdometry::SharedPtr msg) {
        // Convert NED to ENU
        Eigen::Vector3d pos_ned(msg->position[0], msg->position[1], msg->position[2]);
        Eigen::Quaterniond q_ned(msg->q[0], msg->q[1], msg->q[2], msg->q[3]);

        Eigen::Vector3d pos_enu = px4_ros_com::frame_transforms::ned_to_enu_local_frame(pos_ned);
        Eigen::Quaterniond q_enu = px4_ros_com::frame_transforms::ned_to_enu_orientation(q_ned);

        // Dynamic TF: odom -> base_footprint
        geometry_msgs::msg::TransformStamped tf_msg;
        tf_msg.header.stamp = this->get_clock()->now();
        tf_msg.header.frame_id = "x500_mono_cam_down_0/odom";
        tf_msg.child_frame_id = "x500_mono_cam_down_0/base_footprint";
        tf_msg.transform.translation.x = pos_enu.x();
        tf_msg.transform.translation.y = pos_enu.y();
        tf_msg.transform.translation.z = pos_enu.z();
        tf_msg.transform.rotation.x = q_enu.x();
        tf_msg.transform.rotation.y = q_enu.y();
        tf_msg.transform.rotation.z = q_enu.z();
        tf_msg.transform.rotation.w = q_enu.w();

        tf_broadcaster_->sendTransform(tf_msg);
    }

    rclcpp::Subscription<px4_msgs::msg::VehicleOdometry>::SharedPtr odom_sub_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_broadcaster_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OdometryConverterNode>());
    rclcpp::shutdown();
    return 0;
}
*/

/* FOR RGBD USING REAL DEPTH*/
#include "rclcpp/rclcpp.hpp"
#include "px4_msgs/msg/vehicle_odometry.hpp"
#include "px4_ros_com/frame_transforms.h"

#include <Eigen/Dense>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/static_transform_broadcaster.h>

class OdometryConverterNode : public rclcpp::Node {
public:
    OdometryConverterNode()
        : Node("odometry_converter_node", rclcpp::NodeOptions())
    {
        if (!this->has_parameter("use_sim_time")) {
            this->declare_parameter("use_sim_time", true);
        }
        this->set_parameter(rclcpp::Parameter("use_sim_time", true));

        odom_sub_ = this->create_subscription<px4_msgs::msg::VehicleOdometry>(
            "/fmu/out/vehicle_odometry",
            rclcpp::SensorDataQoS(),
            std::bind(&OdometryConverterNode::odomCallback, this, std::placeholders::_1));

        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        static_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

        publishStaticTransforms();
    }

private:
    void publishStaticTransforms() {
        std::vector<geometry_msgs::msg::TransformStamped> static_transforms;
        rclcpp::Time stamp = this->get_clock()->now();

        // base_footprint -> base_link
        geometry_msgs::msg::TransformStamped basefootprint_to_baselink;
        basefootprint_to_baselink.header.stamp = stamp;
        basefootprint_to_baselink.header.frame_id = "x500_depth_0/base_footprint";
        basefootprint_to_baselink.child_frame_id = "x500_depth_0/base_link";
        basefootprint_to_baselink.transform.translation.x = 0.0;
        basefootprint_to_baselink.transform.translation.y = 0.0;
        basefootprint_to_baselink.transform.translation.z = 0.0;
        basefootprint_to_baselink.transform.rotation.w = 1.0;
        static_transforms.push_back(basefootprint_to_baselink);

        // base_link -> imu_sensor
        geometry_msgs::msg::TransformStamped base_to_imu;
        base_to_imu.header.stamp = stamp;
        base_to_imu.header.frame_id = "x500_depth_0/base_link";
        base_to_imu.child_frame_id = "x500_depth_0/base_link/imu_sensor";
        base_to_imu.transform.translation.x = 0.0;
        base_to_imu.transform.translation.y = 0.0;
        base_to_imu.transform.translation.z = 0.0;
        base_to_imu.transform.rotation.w = 1.0;
        static_transforms.push_back(base_to_imu);

        // base_link -> camera_link
        geometry_msgs::msg::TransformStamped base_to_camlink;
        base_to_camlink.header.stamp = stamp;
        base_to_camlink.header.frame_id = "x500_depth_0/base_link";
        base_to_camlink.child_frame_id = "x500_depth_0/camera_link";
        base_to_camlink.transform.translation.x = 0.0;
        base_to_camlink.transform.translation.y = 0.03;
        base_to_camlink.transform.translation.z = 0.080;
        tf2::Quaternion q_cam;
        q_cam.setRPY(0.0, 0.0, 0.0);  // Adjust pitch if needed
        base_to_camlink.transform.rotation.x = q_cam.x();
        base_to_camlink.transform.rotation.y = q_cam.y();
        base_to_camlink.transform.rotation.z = q_cam.z();
        base_to_camlink.transform.rotation.w = q_cam.w();
        static_transforms.push_back(base_to_camlink);

        // camera_link -> IMX214 (RGB)
        geometry_msgs::msg::TransformStamped camlink_to_rgb;
        camlink_to_rgb.header.stamp = stamp;
        camlink_to_rgb.header.frame_id = "x500_depth_0/camera_link";
        camlink_to_rgb.child_frame_id = "x500_depth_0/camera_link/IMX214";
        camlink_to_rgb.transform.rotation.w = 1.0;
        static_transforms.push_back(camlink_to_rgb);

        // camera_link -> StereoOV7251 (Depth)
        geometry_msgs::msg::TransformStamped camlink_to_depth;
        camlink_to_depth.header.stamp = stamp;
        camlink_to_depth.header.frame_id = "x500_depth_0/camera_link";
        camlink_to_depth.child_frame_id = "x500_depth_0/camera_link/StereoOV7251";
        camlink_to_depth.transform.rotation.w = 1.0;
        static_transforms.push_back(camlink_to_depth);

        static_broadcaster_->sendTransform(static_transforms);
    }

    void odomCallback(const px4_msgs::msg::VehicleOdometry::SharedPtr msg) {
        Eigen::Vector3d pos_ned(msg->position[0], msg->position[1], msg->position[2]);
        Eigen::Quaterniond q_ned(msg->q[0], msg->q[1], msg->q[2], msg->q[3]);

        Eigen::Vector3d pos_enu = px4_ros_com::frame_transforms::ned_to_enu_local_frame(pos_ned);
        Eigen::Quaterniond q_enu = px4_ros_com::frame_transforms::ned_to_enu_orientation(q_ned);

        geometry_msgs::msg::TransformStamped tf_msg;
        tf_msg.header.stamp = this->get_clock()->now();
        tf_msg.header.frame_id = "x500_depth_0/odom";
        tf_msg.child_frame_id = "x500_depth_0/base_footprint";
        tf_msg.transform.translation.x = pos_enu.x();
        tf_msg.transform.translation.y = pos_enu.y();
        tf_msg.transform.translation.z = pos_enu.z();
        tf_msg.transform.rotation.x = q_enu.x();
        tf_msg.transform.rotation.y = q_enu.y();
        tf_msg.transform.rotation.z = q_enu.z();
        tf_msg.transform.rotation.w = q_enu.w();

        tf_broadcaster_->sendTransform(tf_msg);
    }

    rclcpp::Subscription<px4_msgs::msg::VehicleOdometry>::SharedPtr odom_sub_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_broadcaster_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OdometryConverterNode>());
    rclcpp::shutdown();
    return 0;
}


/*FOR STEREO
#include "rclcpp/rclcpp.hpp"
#include "px4_msgs/msg/vehicle_odometry.hpp"
#include "px4_ros_com/frame_transforms.h"

#include <Eigen/Dense>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/static_transform_broadcaster.h>

class OdometryConverterNode : public rclcpp::Node {
public:
    OdometryConverterNode()
        : Node("odometry_converter_node", rclcpp::NodeOptions())
    {
        if (!this->has_parameter("use_sim_time")) {
            this->declare_parameter("use_sim_time", true);
        }
        this->set_parameter(rclcpp::Parameter("use_sim_time", true));

        odom_sub_ = this->create_subscription<px4_msgs::msg::VehicleOdometry>(
            "/fmu/out/vehicle_odometry",
            rclcpp::SensorDataQoS(),
            std::bind(&OdometryConverterNode::odomCallback, this, std::placeholders::_1));

        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        static_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

        publishStaticTransforms();
    }

private:
    void publishStaticTransforms() {
        std::vector<geometry_msgs::msg::TransformStamped> static_transforms;
        rclcpp::Time stamp = this->get_clock()->now();

        // base_footprint -> base_link
        geometry_msgs::msg::TransformStamped basefootprint_to_baselink;
        basefootprint_to_baselink.header.stamp = stamp;
        basefootprint_to_baselink.header.frame_id = "x500_mono_cam_0/base_footprint";
        basefootprint_to_baselink.child_frame_id = "x500_mono_cam_0/base_link";
        basefootprint_to_baselink.transform.translation.x = 0.0;
        basefootprint_to_baselink.transform.translation.y = 0.0;
        basefootprint_to_baselink.transform.translation.z = 0.0;
        basefootprint_to_baselink.transform.rotation.w = 1.0;
        static_transforms.push_back(basefootprint_to_baselink);

        // base_link -> imu_sensor
        geometry_msgs::msg::TransformStamped base_to_imu;
        base_to_imu.header.stamp = stamp;
        base_to_imu.header.frame_id = "x500_mono_cam_0/base_link";
        base_to_imu.child_frame_id = "x500_mono_cam_0/base_link/imu_sensor";
        base_to_imu.transform.translation.x = 0.0;
        base_to_imu.transform.translation.y = 0.0;
        base_to_imu.transform.translation.z = 0.0;
        base_to_imu.transform.rotation.w = 1.0;
        static_transforms.push_back(base_to_imu);

        // base_link -> camera_link
        geometry_msgs::msg::TransformStamped base_to_camlink;
        base_to_camlink.header.stamp = stamp;
        base_to_camlink.header.frame_id = "x500_mono_cam_0/base_link";
        base_to_camlink.child_frame_id = "x500_mono_cam_0/camera_link";
        base_to_camlink.transform.translation.x = 0.0;
        base_to_camlink.transform.translation.y = 0.0;
        base_to_camlink.transform.translation.z = 0.1;
        tf2::Quaternion q_cam;
        q_cam.setRPY(0.0, 0.0, 0.0);  // No rotation needed
        base_to_camlink.transform.rotation.x = q_cam.x();
        base_to_camlink.transform.rotation.y = q_cam.y();
        base_to_camlink.transform.rotation.z = q_cam.z();
        base_to_camlink.transform.rotation.w = q_cam.w();
        static_transforms.push_back(base_to_camlink);

        // camera_link -> stereo_left
        geometry_msgs::msg::TransformStamped camlink_to_left;
        camlink_to_left.header.stamp = stamp;
        camlink_to_left.header.frame_id = "x500_mono_cam_0/camera_link";
        camlink_to_left.child_frame_id = "x500_mono_cam_0/camera_link/stereo_left";
        camlink_to_left.transform.translation.x = 0.0;
        camlink_to_left.transform.translation.y = 0.0;
        camlink_to_left.transform.translation.z = 0.0;
        camlink_to_left.transform.rotation.w = 1.0;
        static_transforms.push_back(camlink_to_left);

        // camera_link -> stereo_right
        geometry_msgs::msg::TransformStamped camlink_to_right;
        camlink_to_right.header.stamp = stamp;
        camlink_to_right.header.frame_id = "x500_mono_cam_0/camera_link";
        camlink_to_right.child_frame_id = "x500_mono_cam_0/camera_link/stereo_right";
        camlink_to_right.transform.translation.x = 0.05;  // Approx stereo baseline
        camlink_to_right.transform.translation.y = 0.0;
        camlink_to_right.transform.translation.z = 0.0;
        camlink_to_right.transform.rotation.w = 1.0;
        static_transforms.push_back(camlink_to_right);

        static_broadcaster_->sendTransform(static_transforms);
    }

    void odomCallback(const px4_msgs::msg::VehicleOdometry::SharedPtr msg) {
        Eigen::Vector3d pos_ned(msg->position[0], msg->position[1], msg->position[2]);
        Eigen::Quaterniond q_ned(msg->q[0], msg->q[1], msg->q[2], msg->q[3]);

        Eigen::Vector3d pos_enu = px4_ros_com::frame_transforms::ned_to_enu_local_frame(pos_ned);
        Eigen::Quaterniond q_enu = px4_ros_com::frame_transforms::ned_to_enu_orientation(q_ned);

        geometry_msgs::msg::TransformStamped tf_msg;
        tf_msg.header.stamp = this->get_clock()->now();
        tf_msg.header.frame_id = "x500_mono_cam_0/odom";
        tf_msg.child_frame_id = "x500_mono_cam_0/base_footprint";
        tf_msg.transform.translation.x = pos_enu.x();
        tf_msg.transform.translation.y = pos_enu.y();
        tf_msg.transform.translation.z = pos_enu.z();
        tf_msg.transform.rotation.x = q_enu.x();
        tf_msg.transform.rotation.y = q_enu.y();
        tf_msg.transform.rotation.z = q_enu.z();
        tf_msg.transform.rotation.w = q_enu.w();

        tf_broadcaster_->sendTransform(tf_msg);
    }

    rclcpp::Subscription<px4_msgs::msg::VehicleOdometry>::SharedPtr odom_sub_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_broadcaster_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<OdometryConverterNode>());
    rclcpp::shutdown();
    return 0;
}
*/

/*FOR VIO
#include "rclcpp/rclcpp.hpp"
#include "px4_msgs/msg/vehicle_odometry.hpp"
#include "px4_ros_com/frame_transforms.h"

#include <Eigen/Dense>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <tf2_ros/static_transform_broadcaster.h>

class VIOFramePublisher : public rclcpp::Node {
public:
    VIOFramePublisher()
        : Node("vio_frame_publisher", rclcpp::NodeOptions())
    {
        if (!this->has_parameter("use_sim_time")) {
            this->declare_parameter("use_sim_time", true);
        }
        this->set_parameter(rclcpp::Parameter("use_sim_time", true));

        odom_sub_ = this->create_subscription<px4_msgs::msg::VehicleOdometry>(
            "/fmu/out/vehicle_odometry",
            rclcpp::SensorDataQoS(),
            std::bind(&VIOFramePublisher::odomCallback, this, std::placeholders::_1));

        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);
        static_broadcaster_ = std::make_shared<tf2_ros::StaticTransformBroadcaster>(this);

        publishStaticTransforms();
    }

private:
    void publishStaticTransforms() {
        std::vector<geometry_msgs::msg::TransformStamped> static_transforms;
        rclcpp::Time stamp = this->get_clock()->now();

        // base_footprint -> base_link
        geometry_msgs::msg::TransformStamped basefootprint_to_baselink;
        basefootprint_to_baselink.header.stamp = stamp;
        basefootprint_to_baselink.header.frame_id = "x500_mono_cam_down_0/base_footprint";
        basefootprint_to_baselink.child_frame_id = "x500_mono_cam_down_0/base_link";
        basefootprint_to_baselink.transform.rotation.w = 1.0;
        static_transforms.push_back(basefootprint_to_baselink);

        // base_link -> imu_sensor
        geometry_msgs::msg::TransformStamped base_to_imu;
        base_to_imu.header.stamp = stamp;
        base_to_imu.header.frame_id = "x500_mono_cam_down_0/base_link";
        base_to_imu.child_frame_id = "x500_mono_cam_down_0/base_link/imu_sensor";
        base_to_imu.transform.rotation.w = 1.0;
        static_transforms.push_back(base_to_imu);

        // base_link -> camera_link
        geometry_msgs::msg::TransformStamped base_to_cam;
        base_to_cam.header.stamp = stamp;
        base_to_cam.header.frame_id = "x500_mono_cam_down_0/base_link";
        base_to_cam.child_frame_id = "x500_mono_cam_down_0/camera_link";
        base_to_cam.transform.translation.x = 0.0;
        base_to_cam.transform.translation.y = 0.03;
        base_to_cam.transform.translation.z = 0.08;
        tf2::Quaternion q_cam;
        q_cam.setRPY(0.0, 0.0, 0.0);
        base_to_cam.transform.rotation.x = q_cam.x();
        base_to_cam.transform.rotation.y = q_cam.y();
        base_to_cam.transform.rotation.z = q_cam.z();
        base_to_cam.transform.rotation.w = q_cam.w();
        static_transforms.push_back(base_to_cam);

        // camera_link -> imager
        geometry_msgs::msg::TransformStamped cam_to_img;
        cam_to_img.header.stamp = stamp;
        cam_to_img.header.frame_id = "x500_mono_cam_down_0/camera_link";
        cam_to_img.child_frame_id = "x500_mono_cam_down_0/camera_link/imager";
        cam_to_img.transform.rotation.w = 1.0;
        static_transforms.push_back(cam_to_img);

        static_broadcaster_->sendTransform(static_transforms);
    }

    void odomCallback(const px4_msgs::msg::VehicleOdometry::SharedPtr msg) {
        Eigen::Vector3d pos_ned(msg->position[0], msg->position[1], msg->position[2]);
        Eigen::Quaterniond q_ned(msg->q[0], msg->q[1], msg->q[2], msg->q[3]);

        Eigen::Vector3d pos_enu = px4_ros_com::frame_transforms::ned_to_enu_local_frame(pos_ned);
        Eigen::Quaterniond q_enu = px4_ros_com::frame_transforms::ned_to_enu_orientation(q_ned);

        geometry_msgs::msg::TransformStamped tf_msg;
        tf_msg.header.stamp = this->get_clock()->now();
        tf_msg.header.frame_id = "x500_mono_cam_down_0/odom";
        tf_msg.child_frame_id = "x500_mono_cam_down_0/base_footprint";
        tf_msg.transform.translation.x = pos_enu.x();
        tf_msg.transform.translation.y = pos_enu.y();
        tf_msg.transform.translation.z = pos_enu.z();
        tf_msg.transform.rotation.x = q_enu.x();
        tf_msg.transform.rotation.y = q_enu.y();
        tf_msg.transform.rotation.z = q_enu.z();
        tf_msg.transform.rotation.w = q_enu.w();

        tf_broadcaster_->sendTransform(tf_msg);
    }

    rclcpp::Subscription<px4_msgs::msg::VehicleOdometry>::SharedPtr odom_sub_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    std::shared_ptr<tf2_ros::StaticTransformBroadcaster> static_broadcaster_;
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<VIOFramePublisher>());
    rclcpp::shutdown();
    return 0;
}
*/