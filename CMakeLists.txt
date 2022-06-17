cmake_minimum_required(VERSION 3.3)

project(VRenderer)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS core widgets gui REQUIRED)


file(GLOB SRCS src/*)
file(GLOB MMDSRCS src/MikuMikuFormats/*)
set(STB_IMAGE_INCLUDE_DIR CACHE PATH "")




include_directories(${STB_IMAGE_INCLUDE_DIR})
add_executable(${PROJECT_NAME} ${SRCS} ${MMDSRCS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Widgets Qt5::Gui)
