cmake_minimum_required(VERSION 3.15)

project( SmartOB )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(include)
add_subdirectory(thirdparty)
add_subdirectory(assets)
