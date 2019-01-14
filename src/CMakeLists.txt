cmake_minimum_required(VERSION 3.5)

add_executable(kbdtest kbdtest.cc packet.cc)
target_link_libraries(kbdtest ${HIDAPI_LIBRARY} ${CRC_LIBRARY} ${UDEV_LIBRARIES})
target_include_directories(kbdtest PUBLIC "${HIDAPI_PATH}/hidapi")
target_include_directories(kbdtest PUBLIC "${CRC_PATH}/include")
