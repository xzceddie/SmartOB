cmake_minimum_required(VERSION 3.15)

project( SmartOB )

set(CMAKE_CXX_STANDARD 17)
set(CMKAKE_EXPORT_COMPONENTS ON)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(include)
add_subdirectory(thirdparty)
