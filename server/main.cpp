#include "ChatServer.hpp"
#include "UI.hpp"
#include "ConsoleUI/ConsoleUI.hpp"
#include <iostream>
#include <thread>
#include <string>

#define DEFAULT_PORT 12345

int main(int argc, char* argv[]) {
    try {
        unsigned short port = DEFAULT_PORT;
        if (argc > 1) {
            port = static_cast<unsigned short>(std::stoi(argv[1]));
        }

        auto ui = std::make_shared<ConsoleUI>();
        Logger logger(ui);
        asio::io_context io_context;
        ChatServer server(ui, logger, io_context, port);
        std::thread admin_thread([&server](){ server.run_admin_console(); });
        
        ui->displayDebugMessage("Server started on port " + std::to_string(port));
        io_context.run();
        admin_thread.join();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}