#include "ChatSession.hpp"
#include <iostream>
#include <vector>

using namespace std::chrono;

ChatSession::ChatSession(std::shared_ptr<UI> ui, asio::ip::tcp::socket socket, 
    MessageCallback on_message, JoinCallback on_join, LeaveCallback on_leave)
    : ui_(ui),
      socket_(std::move(socket)),
      on_message_(on_message), 
      on_join_(on_join), 
      on_leave_(on_leave),
      inactivity_timer_(socket_.get_executor()), 
      last_activity_time_(steady_clock::now()) {
    inactivity_timer_.expires_at(steady_clock::time_point::max());
}

void ChatSession::start() {
    read_username();
}

void ChatSession::deliver(const std::string& msg) {
    asio::async_write(socket_, asio::buffer(msg), [](asio::error_code, std::size_t){});
}

void ChatSession::stop() {
    if (socket_.is_open()) {
        socket_.close();
    }
    inactivity_timer_.cancel();
}

std::string ChatSession::get_username() const { 
    return username_; 
}

system_clock::time_point ChatSession::get_connect_time() const { 
    return connection_time_; 
}

void ChatSession::read_username() {
    asio::async_read_until(socket_, read_buffer_, '\n',
        [self = shared_from_this()](const asio::error_code& error, std::size_t size) {
            if (!error) {
                std::istream is(&self->read_buffer_);
                std::getline(is, self->username_);
                self->connection_time_ = system_clock::now();

                if (self->ui_) {
                    self->ui_->displayMessage("-> User '" + self->username_ + "' joined.");
                }

                self->on_join_(self);
                self->read_message();
                self->check_inactivity();
            } else {
                self->on_leave_(self);
            }
        });
}

void ChatSession::read_message() {
    asio::async_read_until(socket_, read_buffer_, '\n',
        [self = shared_from_this()](const asio::error_code& error, std::size_t size) {
            if (!error) {
                std::istream is(&self->read_buffer_);
                std::string message_body;
                std::getline(is, message_body);

                std::string formatted_msg = self->username_ + ": " + message_body + "\n";
                self->on_message_(formatted_msg, self);
                self->last_activity_time_ = steady_clock::now();
                self->read_message();
            } else {
                if (self->ui_) {
                    self->ui_->displayMessage("<- User '" + self->username_ + "' left.");
                }
                self->stop();
                self->on_leave_(self);
            }
        });
}

void ChatSession::check_inactivity() {
    if (steady_clock::now() - last_activity_time_ > minutes(10)) {
        if (ui_) {
            ui_->displayDebugMessage("!! Kicking '" + username_ + "' due to inactivity.");
        }
        stop();
        on_leave_(shared_from_this());
        return;
    }

    inactivity_timer_.expires_after(seconds(15));
    inactivity_timer_.async_wait([self = shared_from_this()](const asio::error_code& ec){
        if (!ec) {
            self->check_inactivity();
        }
    });
}