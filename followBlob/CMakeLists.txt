# cmake needs this line
cmake_minimum_required(VERSION 3.1)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Define project name
project(FollowBlob)

# Find OpenCV, you may need to set OpenCV_DIR variable
# to the absolute path to the directory containing OpenCVConfig.cmake file
# via the command line or GUI
find_package(OpenCV REQUIRED)

#find_package(wiringPi REQUIRED)
#set(WIRINGPI /usr/local/include)
#find_library(WIRING_PI wiringPi)
#add_library(wiringPi SHARED IMPORTED GLOBAL)

#link_directories(${CMAKE_BINARY_DIR}/res)
#set_target_properties( wiringPi PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/res/wiringPi.so)

#include_directories(${WIRINGPI_INCLIDE_DIRS})


# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them
message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# Declare the executable target built from your sources
add_executable(${PROJECT_NAME} followBlob.cpp piBoard.c RS485Devices.c)

# Link your application with OpenCV libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} wiringPi)
