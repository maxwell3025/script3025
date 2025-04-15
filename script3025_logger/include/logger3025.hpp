#ifndef SCRIPT3025_LOGGER3025_LOGGER3025_HPP
#define SCRIPT3025_LOGGER3025_LOGGER3025_HPP

#include <memory>
#include <mutex>
#include <sstream>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> get_logger();

template <typename T>
std::string to_string(const T &a) {
    std::stringstream output;
    output << a;
    return output.str();
}

#define LOGGER3025_TRACE(...) SPDLOG_LOGGER_TRACE(get_logger(), __VA_ARGS__)
#define LOGGER3025_DEBUG(...) SPDLOG_LOGGER_DEBUG(get_logger(), __VA_ARGS__)
#define LOGGER3025_INFO(...) SPDLOG_LOGGER_INFO(get_logger(), __VA_ARGS__)
#define LOGGER3025_WARN(...) SPDLOG_LOGGER_WARN(get_logger(), __VA_ARGS__)
#define LOGGER3025_ERROR(...) SPDLOG_LOGGER_ERROR(get_logger(), __VA_ARGS__)
#define LOGGER3025_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(get_logger(), __VA_ARGS__)

#endif