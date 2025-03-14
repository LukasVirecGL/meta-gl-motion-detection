cmake_minimum_required(VERSION 3.1)
project(motion-detector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SOURCES
  parsers/parser_mt76.cpp
  estimators/kurtosis_motion_estimator.cpp
  wifi_drv_api/mt76_api.cpp
  motion_detector.cpp
  md.cpp
)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(motion-detector ${SOURCES} main.cpp)
add_library(motiondetector STATIC ${SOURCES})

find_library(LIBNL_TINY nl-tiny PATHS ${LIBNL_TINY_PATH})

add_compile_options(-Wall -Wextra)
find_package(Threads REQUIRED)
target_link_libraries(motion-detector PRIVATE Threads::Threads ${LIBNL_TINY})

set(CMAKE_INSTALL_PREFIX /usr)
install(TARGETS motion-detector RUNTIME DESTINATION z)
