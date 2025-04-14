#include <memory>
#include <mutex>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

inline std::shared_ptr<spdlog::logger> get_logger() {
  static std::shared_ptr<spdlog::logger> logger_instance;
  static std::mutex initialization_lock;
  if (logger_instance == nullptr) {
    initialization_lock.lock();
    if (logger_instance == nullptr) {
      logger_instance = spdlog::stderr_color_mt(
          "default", spdlog::color_mode::always);
      logger_instance -> set_level(spdlog::level::trace);
      logger_instance -> set_pattern(
          "%^[%l] [%D %T.%F] [%@] [thread %t] %v%$");
    }
    initialization_lock.unlock();
  }
  return logger_instance;
}