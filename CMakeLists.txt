cmake_minimum_required(VERSION 3.10)

project(SpiderChat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Create a library for the Logger ---
add_library(Logger
    Logger/src/Logger.cpp
)
target_include_directories(Logger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Logger/include
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)

# --- Create a library for the shared UI code ---
add_library(ConsoleUI
    UI/ConsoleUI/ConsoleUI.cpp
)
target_include_directories(ConsoleUI PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
)

# --- Find necessary packages ---
find_package(Threads REQUIRED)

# --- Define the server executable ---
add_executable(server
    server/main.cpp
    server/src/ChatServer.cpp
    server/src/ChatSession.cpp
)
target_include_directories(server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/server/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib # For asio.hpp
)
# Link server against its library dependencies
target_link_libraries(server PRIVATE
    Logger
    ConsoleUI
    Threads::Threads
)

# --- Define the client executable ---
add_executable(client
    client/client.cpp
)
target_include_directories(client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib # For asio.hpp
)
# Link client against its library dependencies
target_link_libraries(client PRIVATE
    Logger
    ConsoleUI
    Threads::Threads
)