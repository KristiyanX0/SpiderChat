#pragma once

#include"../UI.hpp"
#include<iostream>

class ConsoleUI : public virtual UI {
    bool debug = false;
public:
    void displayDebugMessage(const std::string& message) override;
    void displayMessage(const std::string& message) override;
    std::string getUserInput() override;
    void setDebug(bool state);
};