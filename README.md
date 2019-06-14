# Continental-ARS-308-21-SSAO-Radar-ROS-driver

ROS node for Continental-ARS-308-21-SSAO-Radar. 

    
## Requirments

1- Ubuntu 14.04 or above

2- ROS

3- socketcan_interface

4- ros_canopen


## Installation 

First set the CAN driver of the linux kernell (SocketCAN). Below is an example sniplet for setting can device can1:

    $ modprobe can_dev
    $ modprobe can
    $ modprobe can_raw
    $ sudo ip link set can1 type can bitrate 500000
    $ sudo ifconfig can1 up
    $ sudo apt-get install can-utils
    
   
The radar must be configured first to receive tracked object information. This device does not transmit raw data from the radar scans. Instead, its microcontroller reads the raw sensing data and detects/tracks objects with its own algorithm (this algorithm is not accesable). The sensor sends the detected/tracked object information through the CAN bus.

The default behavior of the device is to send detected objects (not tracked). In order to receive tracked object messages, 0x60A, 0x60B and 0x60C, a configuration message has to be sent. The following command will send the configuration message using the can-utils for receiving tracked object messages:

    $ cansend can1 200#0832000200000000 
    
 install Ros CAN driver:
 
    $ sudo apt-get install ros-kinetic-ros-canopen
    
Start ros and get the CAN messages from the canbus

    $ roscore
    $ rosrun socketcan_bridge socketcan_to_topic_node _can_device:="can1"
    
Now run cantopic_to_data node provided here to convert the CAN messages into object locations. Good luck!
