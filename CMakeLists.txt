cmake_minimum_required(VERSION 3.0.0)
project(IC40Utils VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(SerialIOLib)


add_executable(IC40Utils main.cpp
      SWGCmd.cpp
      SWGPacket.cpp
      BasicPacketProcessor.cpp
      )
target_include_directories(IC40Utils PRIVATE 
      plog/include
      SerialIOLib
      )

target_link_libraries(IC40Utils PRIVATE pthread SerialIOLib)

