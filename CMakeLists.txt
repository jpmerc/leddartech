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
#  LIBRARIES leddartech
#  CATKIN_DEPENDS roscpp rospy sensor_msgs std_msgs
#  DEPENDS system_lib
)

include_directories(
	${catkin_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}/Leddar	
	${PROJECT_SOURCE_DIR}/SDK/LeddarC
)


link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(leddartech leddartech.cpp)
target_link_libraries(${catkin_LIBRARIES} leddartech LeddarC Leddar LeddarTech)
