#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>



namespace sob {

// Create a logger with a colorful output to stdout
static auto console = [](){

    spdlog::stdout_color_mt("console");
    spdlog::set_default_logger(console);
    // Set the log level to trace
    spdlog::set_level(spdlog::level::info);

}();


} // namespace sob




