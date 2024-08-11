include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/spdlog/include/)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/Catch2/src/)


add_executable( sim sim.cpp )
find_package( Boost REQUIRED COMPONENTS program_options )
target_link_libraries( sim PUBLIC Boost::program_options)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/streamGtor.py DESTINATION ${CMAKE_BINARY_DIR}/src/)