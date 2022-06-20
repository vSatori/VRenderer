cmake_minimum_required(VERSION 3.0)

project(VRenderer)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS core widgets gui REQUIRED)

file(GLOB SRCS ${PROJECT_SOURCE_DIR}/src/*)
file(GLOB MMDSRCS ${PROJECT_SOURCE_DIR}/src/MikuMikuFormats/*)
set(STB_IMAGE_INCLUDE_DIR CACHE PATH "")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(${STB_IMAGE_INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${SRCS} ${MMDSRCS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui)
