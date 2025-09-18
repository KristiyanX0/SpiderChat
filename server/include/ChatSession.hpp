#pragma once
#include "asio.hpp"
#include <memory>
#include <string>
#include <functional>
#include "UI.hpp"
#include <chrono>

class ChatSession : public std::enable_shared_from_this<ChatSession> {
public:
    using MessageCallback = std::function<void(const std::string&, std::shared_ptr<ChatSession>)>;
    using JoinCallback = std::function<void(std::shared_ptr<ChatSession>)>;
    using LeaveCallback = std::function<void(std::shared_ptr<ChatSession>)>;

    ChatSession(std::shared_ptr<UI> ui, asio::ip::tcp::socket socket, 
        MessageCallback on_message, JoinCallback on_join, LeaveCallback on_leave);
    void start();
    void deliver(const std::string& msg);
    void stop();

    std::string get_username() const;
    std::chrono::system_clock::time_point get_connect_time() const;

private:
    void read_username();
    void read_message();
    void check_inactivity();

    asio::ip::tcp::socket socket_;
    asio::streambuf read_buffer_;
    std::string username_;
    asio::steady_timer inactivity_timer_;
    std::chrono::steady_clock::time_point last_activity_time_;
    std::chrono::system_clock::time_point connection_time_;
    std::shared_ptr<UI> ui_;

    MessageCallback on_message_;
    JoinCallback on_join_;
    LeaveCallback on_leave_;
};