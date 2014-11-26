cmake_minimum_required(VERSION 2.4.6)
project(leddartech)

include_directories(${PROJECT_SOURCE_DIR}/Leddar)
include_directories(${PROJECT_SOURCE_DIR}/SDK/LeddarC)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(leddartech ${PROJECT_SOURCE_DIR}/SDK/LeddarCDemo/Main.c)
target_link_libraries(leddartech LeddarC Leddar LeddarTech)
