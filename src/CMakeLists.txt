cmake_minimum_required(VERSION 3.5)

add_compile_options(-Wall)

set(SRCS xbows.cc packet.cc layer.cc driver_layer.cc keymap.cc custom_layer.cc
  driver_parse.cc)
add_library(xbows SHARED ${SRCS})
target_link_libraries(xbows
  ${HIDAPI_LIBRARY} ${CRC_LIBRARY} ${YAML_LIBRARY} ${UDEV_LIBRARIES})
target_include_directories(xbows PUBLIC ${HIDAPI_PATH}/hidapi)
target_include_directories(xbows PUBLIC "${CRC_PATH}/include")
message("crc ${CRC_PATH}/include" )

link_libraries(xbows)

add_executable(kbdtest kbdtest.cc)
add_executable(testparse testparse.cc)
add_executable(kbdtest2 kbdtest2.cc)
