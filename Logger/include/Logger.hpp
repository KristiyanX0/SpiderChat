#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <memory>
#include "UI.hpp"

class Logger {
public:
    Logger(std::shared_ptr<UI> ui);
    void log(const std::string& message);

private:
    void open_new_log_file(const std::chrono::system_clock::time_point& now);

    std::mutex mutex_;
    std::ofstream log_stream_;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::hours> last_log_hour_;
    std::shared_ptr<UI> ui_;
};
