cmake_minimum_required(VERSION 3.5)

add_compile_options(-Wall)

include_directories("${HIDAPI_PATH}/hidapi")

set(SRCS xbows.cc packet.cc driver_layer.cc keymap.cc custom_layer.cc driver_parse.cc)
add_library(xbows SHARED ${SRCS})


target_include_directories(xbows PUBLIC "${CRC_PATH}/include")
message("crc ${CRC_PATH}/include" )
target_link_libraries(xbows ${HIDAPI_LIBRARY} ${CRC_LIBRARY}
  ${UDEV_LIBRARIES} ${YAML_LIBRARY})

link_libraries(xbows)

add_executable(kbdtest kbdtest.cc)
add_executable(testparse testparse.cc)
add_executable(crap crap.cc)
