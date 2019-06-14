cmake_minimum_required(VERSION 2.8.3)
project(radar_driver)

find_package(catkin REQUIRED
  COMPONENTS
    class_loader
    can_msgs
    roscpp
    socketcan_interface
)

find_package(Boost REQUIRED
  COMPONENTS
    chrono
    system
    thread
)

find_package(Threads REQUIRED)

catkin_package(
  INCLUDE_DIRS
  LIBRARIES
  CATKIN_DEPENDS
    socketcan_interface
  DEPENDS
    Boost
    can_msgs
    roscpp
)

include_directories(
  include
  ${Boost_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)

add_executable(cantopic_to_data cantopic_to_data.cpp
)

target_link_libraries(cantopic_to_data
   ${catkin_LIBRARIES}
   ${Boost_LIBRARIES}
   ${CMAKE_THREAD_LIBS_INIT}
)
