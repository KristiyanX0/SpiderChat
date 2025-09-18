#include "ChatServer.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

ChatServer::ChatServer(std::shared_ptr<UI> ui, Logger& logger, asio::io_context& io_context, short port)
    : ui_(ui),
      io_context_(io_context),
      acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      logger_(logger) {
    start_accept();
}

void ChatServer::run_admin_console() {
    if (ui_) {
        ui_->displayMessage("Admin console running. Commands: list <user>, kick <user>");
    }

    std::string line;
    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "list") {
            list_clients();
        }
        else if (cmd == "kick") {
            std::string username;
            ss >> username;
            kick_client(username);
        }
    }
}

void ChatServer::start_accept() {
    acceptor_.async_accept(
        [this](asio::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                auto on_message = [this](const std::string& msg, std::shared_ptr<ChatSession> sender){
                    this->broadcast(msg, sender);
                };
                auto on_join = [this](std::shared_ptr<ChatSession> session){
                    this->add_client(session);
                };
                auto on_leave = [this](std::shared_ptr<ChatSession> session){
                    this->remove_client(session);
                };

                std::make_shared<ChatSession>(ui_, std::move(socket), on_message, on_join, on_leave)->start();
            }

            start_accept();
        });
}

void ChatServer::broadcast(const std::string& message, std::shared_ptr<ChatSession> sending_client) {
    logger_.log(message);

    std::lock_guard<std::mutex> lock(session_mutex_);
    for (auto& session : active_sessions_) {
        if (session != sending_client) {
            session->deliver(message);
        }
    }
}

void ChatServer::add_client(std::shared_ptr<ChatSession> client_session) {
    std::lock_guard<std::mutex> lock(session_mutex_);
    active_sessions_.push_back(client_session);
}

void ChatServer::remove_client(std::shared_ptr<ChatSession> client_session) {
    std::lock_guard<std::mutex> lock(session_mutex_);
    active_sessions_.erase(std::remove(active_sessions_.begin(), active_sessions_.end(), client_session), active_sessions_.end());
}

void ChatServer::list_clients() {
    std::lock_guard<std::mutex> lock(session_mutex_);
    ui_->displayMessage("Connected clients: " + std::to_string(active_sessions_.size()));

    for (const auto& session : active_sessions_) {
        auto online_duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - session->get_connect_time());
        ui_->displayMessage("  - " + session->get_username() + " (online for " + std::to_string(online_duration.count()) + "s)");
    }
}

void ChatServer::kick_client(const std::string& username) {
    asio::post(io_context_, [this, username]() {
        std::lock_guard<std::mutex> lock(session_mutex_);
        auto client_to_kick = std::find_if(active_sessions_.begin(), active_sessions_.end(),
            [&](const auto& s){ return s->get_username() == username; });

        if (client_to_kick != active_sessions_.end()) {
            if (ui_) {
                ui_->displayMessage("Kicking " + username + ".");
            }
            (*client_to_kick)->stop();
        } else {
            if (ui_) {
                ui_->displayMessage("User '" + username + "' not found.");
            }
        }
    });
}