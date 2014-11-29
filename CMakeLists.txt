cmake_minimum_required(VERSION 2.8.3)
project(test)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  std_msgs
)

include_directories(include)

link_directories(${PROJECT_SOURCE_DIR}/lib)

## Declare a cpp executable
 add_executable(leddartech src/leddartech.cpp)
 target_link_libraries(leddartech
   ${catkin_LIBRARIES}
   LeddarTech
   Leddar
   LeddarC
 )

