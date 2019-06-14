#include <iostream>
#include <boost/unordered_set.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/make_shared.hpp>
#include <class_loader/class_loader.h>

#include <ros/ros.h>
#include <can_msgs/Frame.h>
#include <geometry_msgs/Pose.h>

#include <visualization_msgs/MarkerArray.h>
#include <visualization_msgs/Marker.h>


class radarDriver
{
  public:
    radarDriver();

  private:
    ros::NodeHandle node_handle_;
    ros::Subscriber cantopic_sub_;
    ros::Publisher radar_detection_vis_pub_;

    visualization_msgs::MarkerArray marker_array_msg_;

    void cantopic_callback(const can_msgs::Frame::ConstPtr& msg);
    void publish_marker(const geometry_msgs::Pose &pose_radar, const int &id);
};

radarDriver::radarDriver(): node_handle_("~")
{
  cantopic_sub_ = node_handle_.subscribe("/received_messages", 1000, &radarDriver::cantopic_callback, this);
  radar_detection_vis_pub_ = node_handle_.advertise<visualization_msgs::MarkerArray>( "visualization_marker_array", 100 );
}

void radarDriver::publish_marker(const geometry_msgs::Pose &pose_radar, const int &id)
{

  marker_array_msg_.markers.clear();

  visualization_msgs::Marker marker;
  marker.header.frame_id = "continental_radar";
  marker.header.stamp = ros::Time();
  marker.ns = "radar";
  marker.id = id;
  marker.type = visualization_msgs::Marker::SPHERE;
  marker.pose.position.x = pose_radar.position.x;
  marker.pose.position.y = pose_radar.position.y;
  marker.pose.position.z = 0.0;
  marker.pose.orientation.x = 0.0;
  marker.pose.orientation.y = 0.0;
  marker.pose.orientation.z = 0.0;
  marker.pose.orientation.w = 0.0;
  marker.scale.x = 0.1;
  marker.scale.y = 0.1;
  marker.scale.z = 0.1;
  marker.color.a = 1.0;
  marker.color.r = 0.0;
  marker.color.g = 1.0;
  marker.color.b = 0.0;
  marker.lifetime = ros::Duration(0.2);

  marker_array_msg_.markers.push_back(marker);
  radar_detection_vis_pub_.publish(marker_array_msg_);
}

void radarDriver::cantopic_callback(const can_msgs::Frame::ConstPtr& msg)
{
    geometry_msgs::Pose radar_pose;
    int id;
    // OBJECT MESSAGE

    if (msg->id == 0x60B)
    {

      if(msg->is_error)
      {
          std::cout << "E " << std::hex << msg->id << std::dec;
      }
      else if(msg->is_extended)
      {
          std::cout << "e " << std::hex << msg->id << std::dec;
      }
      else
      {
          std::cout << "s " << std::hex << msg->id << std::dec;
      }
      std::cout << "\t";

      if(msg->is_rtr){
        std::cout << "r";
      }
      else
      {

        for(int i=0; i < msg->dlc; ++i)
        {
          // convert from big-endian to little endian
          //uint64_t data = msg->data[i];
          //uint64_t __builtin_bswap64 (uint64_t data);

          if (i==0)
          {
            unsigned int temp_data = msg->data[i];
            temp_data = temp_data & 0b11111100;
            id = temp_data = temp_data & 0b11111100;

            std::cout << "Object id: "<<std::dec << temp_data << " ";
          }

          else if (i==1)
          {
            unsigned int temp_data = msg->data[i];
            temp_data = temp_data << 5;
            temp_data = temp_data + msg->data[i+1] & 0b11100000;
            radar_pose.position.x = temp_data*0.1;
            std::cout << "Long. displacement: "<<std::dec << temp_data*0.1 << "m";

          }

          else if (i == 2)
          {
            unsigned int temp_data = msg->data[i];
            temp_data = temp_data & 0b00011111 << 7;
            temp_data = temp_data + msg->data[i+1] & 0b11111110;

            std::cout << "Long. Vel: "<<std::dec << temp_data*0.0625 << "m/s ";// should be temp_data*0.0625-128 according to the user  guide
          }
          else if (i == 5)
          {
            unsigned int temp_data = msg->data[i];
            temp_data = temp_data << 2;
            temp_data = temp_data + msg ->data[i+1] & 0b11000000;
            radar_pose.position.y = temp_data*0.1 - 51;
            std::cout << "Lat. displacement: "<<std::dec << temp_data*0.1 - 51<< "m";
          }
        }
      }
      if (radar_pose.position.x != 0)
      {
          publish_marker(radar_pose, id);
      }

  }
  std::cout << std::dec << std::endl;
}



int main (int argc, char** argv)
{
  ros::init (argc, argv, "cantopic_to_data");
  radarDriver node;
  ros::spin ();

  return 0;
}
