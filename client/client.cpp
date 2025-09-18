#include "asio.hpp"
#include "Logger.hpp"
#include "UI.hpp"
#include "ConsoleUI/ConsoleUI.hpp"
#include <memory>

using asio::ip::tcp;

void read_loop(tcp::socket& socket, Logger& logger, std::shared_ptr<UI> ui) {
    try {
        for (;;) {
            asio::streambuf buffer;
            asio::error_code error;
            
            asio::read_until(socket, buffer, '\n', error);

            if (error == asio::error::eof) {
                ui->displayMessage("\nConnection closed by server.");
                break;
            } else if (error) {
                throw asio::system_error(error);
            }

            std::istream is(&buffer);
            std::string message;
            std::getline(is, message);
            ui->displayMessage(message);
        }
    } catch (const std::exception& e) {
        logger.log("Read loop error: " + std::string(e.what()));
    }
}

int main(int argc, char* argv[]) {
    auto ui = std::make_shared<ConsoleUI>();
    Logger logger(ui);

    try {
        if (argc > 3) {
            ui->displayMessage("Usage: client <port>");
            return 1;
        }
        std::string port = (argc > 2) ? argv[2] : "12345";

        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("localhost", port);
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        ui->displayMessage("Enter your username: ");
        std::string username;
        std::getline(std::cin, username);
        asio::write(socket, asio::buffer(username + "\n"));

        ui->displayMessage("Welcome, " + username + "!");

        std::thread reader_thread(read_loop, std::ref(socket), std::ref(logger), ui);
        reader_thread.detach();

        std::string line;
        while (std::getline(std::cin, line)) {
            if (!line.empty()) {
                asio::write(socket, asio::buffer(line + "\n"));
            }
        }
        
        socket.close();

    } catch (const std::exception& e) {
        logger.log("Exception: " + std::string(e.what()));
    }

    return 0;
}