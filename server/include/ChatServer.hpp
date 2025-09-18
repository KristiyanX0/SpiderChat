#pragma once
#include "asio.hpp"
#include "Logger.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <string>
#include "UI.hpp"
#include "ChatSession.hpp"

class ChatServer {
public:
    ChatServer(std::shared_ptr<UI> ui, Logger& logger, asio::io_context& io_context, short port);

    void run_admin_console();

private:
    void start_accept();
    void broadcast(const std::string& message, std::shared_ptr<ChatSession> sending_client);
    void add_client(std::shared_ptr<ChatSession> client_session);
    void remove_client(std::shared_ptr<ChatSession> client_session);
    void list_clients();
    void kick_client(const std::string& username);

private:
    std::shared_ptr<UI> ui_;
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<ChatSession>> active_sessions_;
    std::mutex session_mutex_;
    Logger& logger_;
};