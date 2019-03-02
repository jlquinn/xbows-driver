cmake_minimum_required(VERSION 3.5)

add_compile_options(-Wall)

set(SRCS packet.cc driver_layer.cc keymap.cc custom_layer.cc driver_parse.cc)

add_executable(kbdtest kbdtest.cc ${SRCS})

target_link_libraries(kbdtest ${HIDAPI_LIBRARY} ${CRC_LIBRARY} ${UDEV_LIBRARIES})
target_include_directories(kbdtest PUBLIC "${HIDAPI_PATH}/hidapi")
target_include_directories(kbdtest PUBLIC "${CRC_PATH}/include")
