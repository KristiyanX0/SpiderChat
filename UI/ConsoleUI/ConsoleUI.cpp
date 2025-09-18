#include"ConsoleUI.hpp"

void ConsoleUI::displayDebugMessage(const std::string& message) {
    if (debug) {
        std::cout << message << std::endl;
    }
}

void ConsoleUI::displayMessage(const std::string& message) {
    std::cout << message << std::endl;
}

std::string ConsoleUI::getUserInput() {
    return "";
}

void ConsoleUI::setDebug(bool state)  {
    debug = state;
}