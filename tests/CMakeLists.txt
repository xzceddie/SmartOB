include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/spdlog/include/)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/Catch2/src/)


add_executable( test_OrderBook test_OrderBook.cpp )
find_package( Boost REQUIRED COMPONENTS system )
target_link_libraries( test_OrderBook PUBLIC Catch2::Catch2 Catch2::Catch2WithMain Boost::system)

add_executable( test_CopyBook test_CopyBook.cpp )
find_package( Boost REQUIRED COMPONENTS system )
target_link_libraries( test_CopyBook PUBLIC Catch2::Catch2 Catch2::Catch2WithMain Boost::system)
