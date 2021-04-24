#include "usb_hw_interface/arduino_com.h"
#include "vector_drive/thrusterPercents.h"
#include <unistd.h>
#include <cmath>
#include <ros/ros.h>

// Helper functions
template <class T>
T map(T input, T inMin, T inMax, T outMin, T outMax){
    T output = (input - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    return output;
}

template <class T>
T constrain(T value, T min, T max){
    if(value > max){
        value = max;
    } else if(value < min){
        value = min;
    }
    return value;
}

int t1(0);
int t2(0);
int t3(0);
int t4(0);
int t5(0);
int t6(0);
int t7(0);
int t8(0);

void update_horizontals(const vector_drive::thrusterPercents::ConstPtr& msg){
    t1 = msg->t1;
    t2 = msg->t2;
    t3 = msg->t3;
    t4 = msg->t4;
}

void update_verticals(const vector_drive::thrusterPercents::ConstPtr& msg){
    t5 = msg->t1;
    t6 = msg->t2;
    t7 = msg->t3;
    t8 = msg->t4;}

int main(int argc, char * argv[])
{
   ros::init(argc, argv, "hardware_interface");
   ros::NodeHandle nh;

   HardwareCom connection("/dev/ttyUSB0", 115200);

   ros::Subscriber verticals_sub = nh.subscribe("rov/cmd_vertical_vdrive", 1, update_verticals);
   ros::Subscriber horizontals_sub = nh.subscribe("rov/cmd_horizontal_vdrive", 1, update_horizontals);


//    ros::Publisher rwheel_encoder_pub = nh.advertise<std_msgs::Int32>("rwheel_encoder", 1);

   ros::Rate rate(50); //50Hz
   while(ros::ok()){
      connection.setController(t1, t2, t3, t4, t5, t6, t7, t8);
//      if(connection.readController()){
//      }
      ros::spinOnce();
      rate.sleep();
  }
}
