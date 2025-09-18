#include "Logger.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std::chrono;

Logger::Logger(std::shared_ptr<UI> ui) : ui_(ui) {
    last_log_hour_ = time_point_cast<hours>(system_clock::now() - hours(24)); 
    open_new_log_file(system_clock::now());
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto now = system_clock::now();
    auto current_hour = time_point_cast<hours>(now);

    if (current_hour != last_log_hour_) {
        open_new_log_file(now);
        last_log_hour_ = current_hour;
    }

    std::time_t tt = system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&tt);
    log_stream_ << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S] ") << message << std::endl;
}

void Logger::open_new_log_file(const system_clock::time_point& now) {
    if (log_stream_.is_open()) {
        log_stream_.close();
    }
    auto tt = system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&tt);
    
    std::stringstream ss;
    ss << "log_" << std::put_time(&tm, "%Y-%m-%d_%H") << ".txt";
    
    log_stream_.open(ss.str(), std::ios::app);
    if (log_stream_.is_open()) {
        if (ui_) {
            ui_->displayDebugMessage("LOGGER: New log file created: " + ss.str());
        }
    } else {
        if (ui_) {
            ui_->displayDebugMessage("LOGGER: Failed to open log file: " + ss.str());
        }
    }
}
