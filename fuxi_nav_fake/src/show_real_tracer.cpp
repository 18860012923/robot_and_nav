#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ros/ros.h>
#include <nav_msgs/Path.h>
#include <std_msgs/String.h>

#include <nav_msgs/Odometry.h>



nav_msgs::Path  path;

ros::Publisher  path_pub;
ros::Subscriber odomSub;
std::string father_frame_;
 void odomCallback(const nav_msgs::Odometry::ConstPtr& odom)
 {
    geometry_msgs::PoseStamped this_pose_stamped;
    this_pose_stamped.pose.position.x = odom->pose.pose.position.x;
    this_pose_stamped.pose.position.y = odom->pose.pose.position.y;

    this_pose_stamped.pose.orientation = odom->pose.pose.orientation;

    this_pose_stamped.header.stamp = ros::Time::now();
    this_pose_stamped.header.frame_id = father_frame_;

    path.poses.push_back(this_pose_stamped);

    path.header.stamp = ros::Time::now();
    path.header.frame_id=father_frame_;
    path_pub.publish(path);
 }


int main (int argc, char **argv)
{
    ros::init (argc, argv, "showpath");

    ros::NodeHandle ph("~");

    path_pub = ph.advertise<nav_msgs::Path>("/pose_truth",10, true);
    odomSub  = ph.subscribe<nav_msgs::Odometry>("/base_pose_truth", 10, odomCallback);
    ph.param<std::string>("father_frame", father_frame_, std::string("odom"));
    ros::Rate loop_rate(50);

    while (ros::ok())
    {
        ros::spinOnce();               // check for incoming messages
        loop_rate.sleep();
    }

    return 0;
}




