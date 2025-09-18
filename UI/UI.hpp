#pragma once

#include<string>

class UI {
public:
    virtual ~UI() = default;

    virtual void displayDebugMessage(const std::string& message) = 0;
    virtual void displayMessage(const std::string& message) = 0;
    virtual std::string getUserInput() = 0;
};