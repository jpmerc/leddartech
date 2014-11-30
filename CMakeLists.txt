cmake_minimum_required(VERSION 2.8.3)
project(leddartech)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  sensor_msgs
  std_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES test
#  CATKIN_DEPENDS other_catkin_pkg
#  DEPENDS system_lib
)

include_directories(include)

link_directories(${PROJECT_SOURCE_DIR}/lib)

## Declare a cpp executable
add_executable(leddartech_node src/leddartech.cpp)
target_link_libraries(leddartech_node ${catkin_LIBRARIES} LeddarTech Leddar LeddarC)

add_executable(test_node src/test.cpp)
target_link_libraries(test_node ${catkin_LIBRARIES})
