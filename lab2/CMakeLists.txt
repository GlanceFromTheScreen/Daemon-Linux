cmake_minimum_required(VERSION 3.12)
project(lab2)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES
    connections/conn_mq.h
    connections/conn_mq.cpp
    connections/conn_fifo.h
    connections/conn_fifo.cpp
    connections/conn_shm.h
    connections/conn_shm.cpp

    host/host.cpp

    aux_methods.cpp
    aux_methods.h
)

add_executable(execu ${SOURCE_FILES})
