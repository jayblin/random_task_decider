cmake_minimum_required(VERSION 3.20)

project(random_task_decider VERSION 0.0.1 LANGUAGES CXX)

add_executable(random_task_decider src/main.cpp)

set_property(TARGET random_task_decider PROPERTY CXX_STANDARD 20)
