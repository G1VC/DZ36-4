#pragma once

#include <WS2tcpip.h>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <queue>
#include "Logger.h"

class NetworkManager {
private:
    WSADATA wsaData;
    SOCKET listenSocket;
    std::thread networkThread;
    std::mutex mtx;
    std::queue<std::string> incomingMessages;
    bool isRunning;
    Logger* logger;

    // Connection settings
    const int PORT = 5000;
    const std::string IP_ADDRESS = "127.0.0.1";

    // Structure for storing connections
    struct Client {
        SOCKET socket;
        std::string username;
    };
    std::vector<Client> clients;

    // Function for handling connections
    void startListening();
    void handleClient(SOCKET clientSocket);
    void broadcastMessage(const std::string& message, SOCKET excludeSocket = INVALID_SOCKET);

public:
    NetworkManager(Logger* log);
    ~NetworkManager();

    // Network management methods
    void init();
    void stop();
    bool sendMessage(const std::string& message);
    std::string receiveMessage();

    // Getting connected users list
    std::vector<std::string> getConnectedUsers();

    // Processing incoming messages
    void processIncomingMessages();
};

// Method prototypes

void NetworkManager::startListening() {
    // Winsock initialization
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Creating socket
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        logger->log("Socket creation error");
        return;
    }

    // Setting socket parameters
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS.c_str());
    serverAddr.sin_port = htons(PORT);

    // Binding socket
    if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        logger->log("Socket binding error");
        return;
    }

    // Starting listening
    listen(listenSocket, SOMAXCONN);
    logger->log("Server started on port " + std::to_string(PORT));

    while (isRunning) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            logger->log("New client connection");
            clients.push_back({ clientSocket, "" });
            std::thread(&NetworkManager::handleClient, this, clientSocket).detach();
        }
    }
}

void NetworkManager::handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while (isRunning) {
        bytesReceived = recv(clientSocket, buffer, 1024, 0);
        if (bytesReceived > 0) {
            std::string message(buffer, bytesReceived);
            logger->log("Message received: " + message);
            incomingMessages.push(message);
        }
        else {
            // Handling client disconnection
            logger->log("Client disconnected");
            closesocket(clientSocket);
            break;
        }
    }
}

bool NetworkManager::sendMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto& client : clients) {
        if (send(client.socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
            logger->log("Message sending error");
            return false;
        }
    }
    return true;
}
