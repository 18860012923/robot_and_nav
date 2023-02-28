#include "ros/ros.h"  //ros需要的头文件
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include "sensor_msgs/Imu.h"
#include "std_msgs/String.h"
#include <tf/tf.h>
#include <tf/transform_listener.h>
//以下为串口通讯需要的头文件     
#include <iostream>
#include <string>
#include <fstream>
//时间同步
#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

std::string filename_;

int circle_time = 0;
ros::Time old_time;
void callback(const nav_msgs::Odometry::ConstPtr& odom_real,const sensor_msgs::Imu::ConstPtr& imu,const nav_msgs::Odometry::ConstPtr& odom)//订阅/odom主题回调函数
{
	float x,y,fi,fi_imu,v_lin,v_gla;
    ros::Time odom_time_ = odom_real->header.stamp;
    //std::cout << odom_time_.useSystemTime();
    if(circle_time == 0)
    {
        old_time = odom_time_;
        
        x = odom -> pose.pose.position.x;
	    y = odom -> pose.pose.position.y;


        fi = tf::getYaw(odom->pose.pose.orientation); 

        fi_imu = tf::getYaw(imu->orientation);
        v_lin = odom->twist.twist.linear.x;
        v_gla = odom->twist.twist.angular.z;
    
        std::ofstream pointout("/home/zzw/pose_save/" + filename_,std::ios::app);
    
        pointout << x << " " << y << " " << fi << " " << fi_imu << " " << v_lin << " " << v_gla; 
        pointout.close();
    }
    circle_time ++;
	x = odom_real -> pose.pose.position.x;
	y = odom_real -> pose.pose.position.y;


    fi = tf::getYaw(odom_real->pose.pose.orientation);
    fi_imu = tf::getYaw(imu->orientation);
    v_lin = odom->twist.twist.linear.x;
    v_gla = odom->twist.twist.angular.z;
 	std::ofstream pointout("/home/zzw/pose_save/" + filename_,std::ios::app);

    double dt = (odom_time_ - old_time).toSec();
 	pointout << " " << dt << "\n" << x <<" "<< y <<" "<< fi << " "<< fi_imu << " " << v_lin << " " << v_gla;  
    pointout.close();
	old_time = odom_time_;

    if(!ros::ok())
    {
        std::ofstream pointout("/home/zzw/pose_save/" + filename_,std::ios::app);
        pointout << " " << 0;  
        pointout.close();
    }
}
	  


int main(int argc, char **argv)
{
    ros::init(argc, argv, "odom_record");//初始化串口节点
    
    ros::NodeHandle n("~");  //定义节点进程句柄
 
    //订阅的话题
    n.param("filename",filename_,std::string("save.txt"));
    message_filters::Subscriber<nav_msgs::Odometry> pose_sub(n, "/tracked_pose", 1);// topic1 输入
    message_filters::Subscriber<sensor_msgs::Imu> imu_sub(n, "/imu_data", 1);// topic2 输入
    message_filters::Subscriber<nav_msgs::Odometry> odom_sub(n, "/odom", 1);// topic1 输入
    typedef message_filters::sync_policies::ApproximateTime<nav_msgs::Odometry, sensor_msgs::Imu, nav_msgs::Odometry> testSyncPolicy;
	message_filters::Synchronizer<testSyncPolicy> sync(testSyncPolicy(10), pose_sub, imu_sub, odom_sub);// 同步
    sync.registerCallback(boost::bind(&callback, _1, _2, _3));

    ros::Rate rate(30);
    while(ros::ok())
    {
        rate.sleep();
        ros::spinOnce();
    }
    ros::spin();
    return 0;
}
