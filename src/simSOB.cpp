#include <L3OrderBook.h>
#include <SmartOrderBook.h>
#include <common.h>
#include <vector>
#include <spdlog/spdlog.h>
#include <boost/program_options.hpp>
#include <fstream>

namespace po = boost::program_options;

namespace sob {


template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
SmartOrderBook<BuffType> runSimSOB( std::vector<std::string>& order_strs )
{
    
    SmartOrderBook<BuffType> sob;
    for( const auto& str : order_strs ) {
        sob.applyMessage( str );
    }
    return sob;
}

template <template <typename T, typename AllocT=std::allocator<T> > class BuffType = boost::circular_buffer>
SmartOrderBook<BuffType> runSimSOB( const std::string& file_name )
{
    std::vector<std::string> order_strs;
    std::ifstream ifs{ file_name };
    std::string line;
    while( std::getline( ifs, line ) ) {
        order_strs.push_back( line );
    }
    return runSimSOB<BuffType>( order_strs );
}

} // namespace sob


int main( int argc, char** argv )
{
    spdlog::info( "[::main] hello world" );
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("sim_file", po::value<std::string>(), "the simulation file you would like to input")
        ("dBufferType", po::value<std::string>(),
                 "what type of dBuffer you would like to use, "
                 "which is the buffer type used in the L3PriceLevel, "
                 "\nchoose from: [ list, circular_buffer ], "
                 "\ndefault to: circular_buffer"
                 "\n**More of a perf consideration, result is unaffected**"
         )
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

    spdlog::info("[::main] using L3OrderBook" );
    if (dBufferType == "list") {
        auto res_book = sob::runSimSOB<std::list>( sim_file );
        spdlog::info( "[::main] Got result book: \n{}", res_book.getLeaderBook()->toString() );
    } else if (dBufferType == "circular_buffer") {  
        auto res_book = sob::runSimSOB<boost::circular_buffer>( sim_file );
        spdlog::info( "[::main] Got result book: \n{}", res_book.getLeaderBook()->toString() );
    }

    return 0;
}
