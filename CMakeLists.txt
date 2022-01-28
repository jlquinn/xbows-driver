cmake_minimum_required(VERSION 3.5)

project(xbows)

# XXX FIX THIS
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(ExternalProject)

find_package(HIDAPI REQUIRED)

### Set up hidapi library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Support NixOS
find_library(HIDAPI_LIBRARY hidapi-libusb)
if (HIDAPI_LIBRARY-NOTFOUND)
  add_subdirectory(hidapi)
  set(HIDAPI_PATH "${CMAKE_SOURCE_DIR}/hidapi")
  set(HIDAPI_LIBRARY hidapi)
endif()

# Set up crc library
set(CRC_PATH "${CMAKE_SOURCE_DIR}/libcrc")
ExternalProject_Add(crc
  SOURCE_DIR ${CRC_PATH}
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND ""
  )
set(CRC_LIBRARY "${CRC_PATH}/lib/libcrc${CMAKE_STATIC_LIBRARY_SUFFIX}")

# Set up yaml-cpp
find_library(YAML_LIBRARY yaml-cpp)
if (YAML_LIBRARY-NOTFOUND)
  error("Please install libyaml-cpp 0.5 or greated.  On Ubuntu, the package is libyaml-cpp-dev")
else()
  message("-- libyaml-cpp ${YAML_LIBRARY}")
endif()

add_subdirectory(src)

#add_executable(kbdtest kbdtest.cc packet.cc)
#target_link_libraries(kbdtest ${HIDAPI_LIBRARY} ${CRC_LIBRARY} ${UDEV_LIBRARIES})
#target_include_directories(kbdtest PUBLIC "${HIDAPI_PATH}/hidapi")
#target_include_directories(kbdtest PUBLIC "${CRC_PATH}/include")
