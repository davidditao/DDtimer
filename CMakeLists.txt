cmake_minimum_required(VERSION 3.0)

project(main)

set(CMAKE_CXX_FLAGES ${CMAKE_CXX_FLAGES})

# set(SRC_LIST sort_timer_lst.cpp)
add_executable(main_signal_lst main_signal_lst.cpp sort_timer_lst.cpp)
add_executable(main_epoll_lst main_epoll_lst.cpp)
add_executable(main_epoll_tw main_epoll_tw.cpp time_wheel.cpp)