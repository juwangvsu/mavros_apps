/**
 * @file offb_node.cpp
 * @brief offboard example node, written with mavros version 0.14.2, px4 flight
 * stack and tested in Gazebo SITL
 */

#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <mavros_msgs/CommandBool.h>
#include <mavros_msgs/SetMode.h>
#include <mavros_msgs/State.h>
float c_radius=10;
mavros_msgs::State current_state;
void state_cb(const mavros_msgs::State::ConstPtr& msg){
    current_state = *msg;
    //std::cout<<current_state;
}

int main(int argc, char **argv)
{
    std::string fcutype;
    int alt; //altitude 
    int uavinst; //altitude 
    ros::init(argc, argv,"offb_node");
    ros::NodeHandle nh("~");
nh.param<std::string>("fcutype", fcutype, "px4");
nh.param<int>("alt", alt, 5);
nh.param<int>("uavinst", uavinst, 1);
    //ros::init(argc, argv, "offb_node"+std::to_string(uavinst));
//uavinst is the instance num of uav, 
//wang 6/6/19 fcutype is px4|iris|solo|apm
//iris , solo, apm all use ardupilot as firmware
std::string mavros="mavros";
 if (uavinst==1){
    mavros="mavros";
 }else{
    mavros="mavros2";
 }
    ros::Subscriber state_sub = nh.subscribe<mavros_msgs::State>
            ("/"+mavros+"/state", 10, state_cb);
    ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("/"+mavros+"/setpoint_position/local", 10);
    ros::ServiceClient arming_client = nh.serviceClient<mavros_msgs::CommandBool>
            ("/"+mavros+"/cmd/arming");
    ros::ServiceClient set_mode_client = nh.serviceClient<mavros_msgs::SetMode>
            ("/"+mavros+"/set_mode");
    //the setpoint publishing rate MUST be faster than 2Hz
    ros::Rate rate(3);
 
    mavros_msgs::SetMode offb_set_mode;
    if (fcutype=="px4")
    {
	std::cout<<"fcutype is px4\n";
    	offb_set_mode.request.custom_mode = "OFFBOARD";
    //the setpoint publishing rate MUST be faster than 2Hz
        rate = ros::Rate(3.0);
    }
    else if (fcutype=="iris" | fcutype=="solo" | fcutype=="apm")
    {
	std::cout<<"fcutype is ardupilot\n";
    	offb_set_mode.request.custom_mode = "GUIDED";
        rate=ros::Rate(2);
	//apm guided mode is not the same as px4 offboard mode, it require
	//relative slow cmd
    }

    // wait for FCU connection
    while(ros::ok() && current_state.connected){
        ros::spinOnce();
        rate.sleep();
    }

    geometry_msgs::PoseStamped pose;
    pose.pose.position.x = 0;
    pose.pose.position.y = 0;
    pose.pose.position.z = alt;

    //send a few setpoints before starting this seems to be necessar
    // to turn off fail safe mode, the fail safe mode will be
    // on again if the setposition cmd stop
    if (fcutype=="px4")
    for(int i = 10; ros::ok() && i > 0; --i){
        local_pos_pub.publish(pose);
        ros::spinOnce();
        rate.sleep();
    }

	//while(1);


    mavros_msgs::CommandBool arm_cmd;
    arm_cmd.request.value = true;

    ros::Time last_request = ros::Time::now();
    int circletheta=0;
    while(ros::ok()){
      if (fcutype=="px4")
        if( current_state.mode != "OFFBOARD" &&
            (ros::Time::now() - last_request > ros::Duration(5.0))){
            if( set_mode_client.call(offb_set_mode) &&
                offb_set_mode.response.mode_sent){
                ROS_INFO("Offboard enabled");
            }
            last_request = ros::Time::now();
        } else {
            if( !current_state.armed &&
                (ros::Time::now() - last_request > ros::Duration(5.0))){
                if( arming_client.call(arm_cmd) &&
                    arm_cmd.response.success){
                    ROS_INFO("Vehicle armed");
                }
                last_request = ros::Time::now();
            }
        }
	circletheta++;

        ROS_INFO_STREAM("xypos "<<c_radius*sin(circletheta*2*3.14/360)<<" "
		<<c_radius*cos(circletheta*2*3.14));
        pose.pose.position.x = c_radius*sin(circletheta*2*3.14/360);
        pose.pose.position.y = c_radius*cos(circletheta*2*3.14/360);
        local_pos_pub.publish(pose);

        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}
