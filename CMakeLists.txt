cmake_minimum_required(VERSION 3.5)

include(ExternalProject)

# Set up hidapi library
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(hidapi)
set(HIDAPI_PATH "${CMAKE_SOURCE_DIR}/hidapi")
find_library(HIDAPI_LIBRARY hidapi PATHS "${HIDAPI_PATH}/linux")

# Set up crc library
ExternalProject_Add(crc
  SOURCE_DIR libcrc
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND ""
  )
set(CRC_PATH "${CMAKE_SOURCE_DIR}/libcrc")
find_library(CRC_LIBRARY crc PATHS "${CRC_PATH}/lib")

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
