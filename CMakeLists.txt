cmake_minimum_required(VERSION 3.5)

include(ExternalProject)

# Set up hidapi library
add_subdirectory(hidapi)
set(HIDAPI_PATH "/home/jlquinn/projects/kbd/hidapi")
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

add_subdirectory(src)

#add_executable(kbdtest kbdtest.cc packet.cc)
#target_link_libraries(kbdtest ${HIDAPI_LIBRARY} ${CRC_LIBRARY} ${UDEV_LIBRARIES})
#target_include_directories(kbdtest PUBLIC "${HIDAPI_PATH}/hidapi")
#target_include_directories(kbdtest PUBLIC "${CRC_PATH}/include")
