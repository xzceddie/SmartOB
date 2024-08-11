include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/spdlog/include/)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty/Catch2/src/)

enable_testing()

add_executable( test_IdGen test_IdGen.cpp )
find_package( Boost REQUIRED COMPONENTS system )
target_link_libraries( test_IdGen PUBLIC Catch2::Catch2WithMain Boost::system IdGen)
add_test( NAME test_IdGen COMMAND test_IdGen )

add_executable( test_messages test_messages.cpp )
find_package( Boost REQUIRED COMPONENTS system )
target_link_libraries( test_messages PUBLIC Catch2::Catch2WithMain Boost::system IdGen)
add_test( NAME test_messages COMMAND test_IdGen )

add_executable( test_OrderBook test_OrderBook.cpp )
find_package( Boost REQUIRED COMPONENTS system )
target_link_libraries( test_OrderBook PUBLIC Catch2::Catch2WithMain Boost::system)
add_test( NAME test_OrderBook COMMAND test_OrderBook )

add_executable( test_L3OrderBook test_L3OrderBook.cpp )
find_package( Boost REQUIRED COMPONENTS system )
target_link_libraries( test_L3OrderBook PUBLIC Catch2::Catch2WithMain Boost::system)
add_test( NAME test_L3OrderBook COMMAND test_L3OrderBook )

add_executable( test_CopyBook test_CopyBook.cpp )
find_package( Boost REQUIRED COMPONENTS system )
target_link_libraries( test_CopyBook PUBLIC Catch2::Catch2WithMain Boost::system)
add_test( NAME test_CopyBook COMMAND test_CopyBook )
