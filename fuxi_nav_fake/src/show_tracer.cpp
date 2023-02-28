#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <nav_msgs/Path.h>
#include <geometry_msgs/PoseStamped.h>

double distance(double x, double y, double x1, double y1)
{
    return sqrt(pow(x-x1,2)+pow(y-y1,2));
}

int main(int argc, char** argv){
    ros::init(argc, argv, "demo_tf_listener");  //初始化一个监听器

    ros::NodeHandle n("~"); //创建一个句柄
    std::string father_frame_;
    n.param<std::string>("father_frame", father_frame_, std::string("odom_combined"));
    ros::Publisher path_pub = n.advertise<nav_msgs::Path>("trajectory_odom", 1, true);
    tf::TransformListener listener; //声明一个监听器 

    int circle_time = 0;
    nav_msgs::Path  path;
    double pre_x = 0,pre_y = 0;
    ros::Rate rate(20.0);
    while (ros::ok()){
        tf::StampedTransform transform;
        geometry_msgs::PoseStamped this_pose_stamped;
        try{
            ros::Time now=ros::Time(0);
            //订阅的是以robot作为父坐标系，world为子坐标系
            listener.waitForTransform(father_frame_,"base_link",now,ros::Duration(0.5));
            listener.lookupTransform(father_frame_, "base_link",now, transform);
            this_pose_stamped.pose.position.x = transform.getOrigin().x();
            this_pose_stamped.pose.position.y = transform.getOrigin().y();
            this_pose_stamped.pose.position.z = transform.getOrigin().z();
            if(distance(pre_x,pre_y,this_pose_stamped.pose.position.x,this_pose_stamped.pose.position.y) < 0.001)
                circle_time ++;
            else
                circle_time = 0;
            if(circle_time == 25)
                {
                    printf("位置误差为：%.3f (m)\n",distance(transform.getOrigin().x(),transform.getOrigin().y(),0,0));
                    printf("角度误差为：%.3f (deg)\n",transform.getRotation().getAngle());
                    circle_time = 0;
                }
            pre_x = this_pose_stamped.pose.position.x;
            pre_y = this_pose_stamped.pose.position.y;
            //this_pose_stamped.pose.orientation = transform.getRotation();

            this_pose_stamped.header.stamp = ros::Time::now();
            this_pose_stamped.header.frame_id = father_frame_;

            path.poses.push_back(this_pose_stamped);

            path.header.stamp = ros::Time::now();
            path.header.frame_id = father_frame_;
            path_pub.publish(path);
        }
        catch (tf::TransformException ex){
            ROS_ERROR("%s",ex.what());
            ros::Duration(1.0).sleep();
        }
        rate.sleep();
    }
  return 0;
}

