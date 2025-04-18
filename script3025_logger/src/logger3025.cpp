#include "logger3025.hpp"

std::shared_ptr<spdlog::logger> get_logger() {
  static std::shared_ptr<spdlog::logger> logger_instance;
  static std::mutex initialization_lock;
  if (logger_instance == nullptr) {
    initialization_lock.lock();
    if (logger_instance == nullptr) {
      logger_instance = spdlog::stderr_color_mt(
          "default", spdlog::color_mode::always);
      logger_instance -> set_level(spdlog::level::trace);
      logger_instance -> set_pattern(
          "%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$");
    }
    initialization_lock.unlock();
  }
  return logger_instance;
}