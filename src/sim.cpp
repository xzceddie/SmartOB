#include <L3OrderBook.h>
#include <common.h>
#include <vector>
#include <spdlog/spdlog.h>
#include <boost/program_options.hpp>
#include <fstream>

namespace po = boost::program_options;

namespace sob {



template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
L3Book<BuffType> runSim( std::vector<Order>& orders )
{
    L3Book<BuffType> res{ orders };
    return res;
}

template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
L3Book<BuffType> runSim( std::vector<std::string>& order_strs )
{
    std::vector<Order> orders;
    for( const auto& str : order_strs ) {
        orders.push_back( Order{ str } );
    }
    return runSim<BuffType>( orders );
}

template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
L3Book<BuffType> runSim( const std::string& file_name )
{
    std::vector<std::string> order_strs;
    std::ifstream ifs{ file_name };
    std::string line;
    while( std::getline( ifs, line ) ) {
        order_strs.push_back( line );
    }
    return runSim<BuffType>( order_strs );
}

L2Book runSim( std::vector<Order>& orders )
{
    L2Book res{ orders };
    return res;
}

L2Book runSim( std::vector<std::string>& order_strs )
{
    std::vector<Order> orders;
    for( const auto& str : order_strs ) {
        orders.push_back( Order{ str } );
    }
    return runSim( orders );
}

L2Book runSim( const std::string& file_name )
{
    std::vector<std::string> order_strs;
    std::ifstream ifs{ file_name };
    std::string line;
    while( std::getline( ifs, line ) ) {
        order_strs.push_back( line );
    }
    return runSim( order_strs );
}

} // namespace sob

int main( int argc, char** argv )
{
    spdlog::info( "[::main] hello world" );
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("L2", "use L2OrderBook, if unspecified, will use L3OrderBook")
        ("sim_file", po::value<std::string>(), "the simulation file you would like to input")
        ("dBufferType", po::value<std::string>(), "what type of dBuffer you would like to use, which is the buffer type used in the L3PriceLevel, choose from: [ list, circular_buffer ], default to circular_buffer")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    std::string sim_file;
    if (vm.count("sim_file")) {
        spdlog::info("[::main] sim_file: {}", vm["sim_file"].as<std::string>());
        sim_file = vm["sim_file"].as<std::string>();
    } else {
        throw std::runtime_error("sim_file not specified");
    }

    std::string dBufferType;
    if (vm.count("dBufferType")) {
        spdlog::info("[::main] dBufferType: {}", vm["dBufferType"].as<std::string>());
        dBufferType = vm["dBufferType"].as<std::string>();
    } else {
        spdlog::warn("[::main] dBufferType not specified, default to circular_buffer" );
        dBufferType = "circular_buffer";
    }

    if (vm.count("L2")) {
        spdlog::info("[::main] using L2OrderBook" );
        auto res_book = sob::runSim( sim_file );
        spdlog::info( "[::main] Got result book: \n{}", res_book.toString() );
    } else {
        spdlog::info("[::main] using L3OrderBook" );
        if (dBufferType == "list") {
            auto res_book = sob::runSim<std::list>( sim_file );
            spdlog::info( "[::main] Got result book: \n{}", res_book.toString() );
        } else if (dBufferType == "circular_buffer") {  
            auto res_book = sob::runSim<boost::circular_buffer>( sim_file );
            spdlog::info( "[::main] Got result book: \n{}", res_book.toString() );
        }
    }

    return 0;
}




