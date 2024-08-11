include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/spdlog/include/)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/Catch2/src/)

find_package( Boost REQUIRED COMPONENTS system )
find_package( Boost REQUIRED COMPONENTS program_options )
include_directories(${Boost_INCLUDE_DIRS})

add_library( IdGen SHARED IdGen.cpp )

add_library( L3OrderBook SHARED L3OrderBook.cpp )
target_link_libraries( L3OrderBook PUBLIC IdGen )

add_executable( simOB simOB.cpp )
# find_package( Boost REQUIRED COMPONENTS program_options )
target_link_libraries( simOB PUBLIC Boost::program_options IdGen L3OrderBook )

add_executable( simSOB simSOB.cpp )
# find_package( Boost REQUIRED COMPONENTS program_options )
target_link_libraries( simSOB PUBLIC Boost::program_options IdGen L3OrderBook )

file(COPY ${CMAKE_CURRENT_LIST_DIR}/streamGtor.py DESTINATION ${CMAKE_BINARY_DIR}/src/)
