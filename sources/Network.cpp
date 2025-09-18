#include "Network.h"
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <WS2tcpip.h>
#include "Logger.h"

// Konstruktor
NetworkManager::NetworkManager(Logger* log) : logger(log) {
    isRunning = false;
    incomingMessages = std::queue<std::string>();
    clients = std::vector<Client>();
}

// Destruktor
NetworkManager::~NetworkManager() {
    stop();
    WSACleanup();
}

// Initializatsiya setevogo soedineniya
void NetworkManager::init() {
    if (!isRunning) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            logger->log("Initialization error Winsock");
            return;
        }

        isRunning = true;
        networkThread = std::thread(&NetworkManager::startListening, this);
        networkThread.detach();
    }
}

// Ostanovka setevogo soedineniya
void NetworkManager::stop() {
    isRunning = false;

    if (listenSocket != INVALID_SOCKET) {
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
    }

    // Closing all client connections
    for (auto& client : clients) {
        if (client.socket != INVALID_SOCKET) {
            closesocket(client.socket);
        }
    }
    clients.clear();
}

// Sending message
bool NetworkManager::sendMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);

    for (auto& client : clients) {
        if (client.socket != INVALID_SOCKET) {
            if (send(client.socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
                logger->log("Sending error to client");
                return false;
            }
        }
    }
    return true;
}

// Receiving message
std::string NetworkManager::receiveMessage() {
    std::lock_guard<std::mutex> lock(mtx);

    if (!incomingMessages.empty()) {
        std::string message = incomingMessages.front();
        incomingMessages.pop();
        return message;
    }
    return "";
}

// Getting connected users list
std::vector<std::string> NetworkManager::getConnectedUsers() {
    std::vector<std::string> userList;

    for (const auto& client : clients) {
        if (!client.username.empty()) {
            userList.push_back(client.username);
        }
    }
    return userList;
}

// Processing incoming messages
void NetworkManager::processIncomingMessages() {
    std::string message;

    while (isRunning) {
        message = receiveMessage();
        if (!message.empty()) {
            // Here you can add message processing
            logger->log("Processed incoming message: " + message);
        }
    }
}

// Function for broadcast
void NetworkManager::broadcastMessage(const std::string& message, SOCKET excludeSocket) {
    std::lock_guard<std::mutex> lock(mtx);

    for (auto& client : clients) {
        if (client.socket != excludeSocket) {
            if (send(client.socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
                logger->log("Broadcast error");
            }
        }
    }
}

// Handling client connection
void NetworkManager::handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while (isRunning) {
        bytesReceived = recv(clientSocket, buffer, 1024, 0);

        if (bytesReceived > 0) {
            std::string message(buffer, bytesReceived);
            logger->log("Received message from client: " + message);

            // Adding message to queue
            std::lock_guard<std::mutex> lock(mtx);
            incomingMessages.push(message);
        }
        else if (bytesReceived == 0) {
            // Client disconnected
            logger->log("Client disconnected");
            removeClient(clientSocket);
            closesocket(clientSocket);
            break;
        }
        else {
            // Error receiving data
            logger->log("Error receiving data from client");
            removeClient(clientSocket);
            closesocket(clientSocket);
            break;
        }
    }
}

// Removing client from list
void NetworkManager::removeClient(SOCKET clientSocket) {
    std::lock_guard<std::mutex> lock(mtx);

    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (it->socket == clientSocket) {
            logger->log("Removing client " + it->username);
            clients.erase(it);
            break;
        }
    }
}

// Handling new client connection
void NetworkManager::startListening() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        logger->log("Winsock initialization error");
        return;
    }

    // Creating listening socket
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        logger->log("Socket creation error");
        WSACleanup();
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
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    // Starting listening
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        logger->log("Listening error");
        closesocket(listenSocket);
        WSACleanup();
        return;
    }

    logger->log("Server started on port " + std::to_string(PORT));

    while (isRunning) {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket != INVALID_SOCKET) {
            logger->log("New client connection");

            // Creating new client
            Client newClient = { clientSocket, "" };
            clients.push_back(newClient);

            // Starting client handling in separate thread
            std::thread clientHandler(&NetworkManager::handleClient, this, clientSocket).detach();
        }
    }
}

// Function for broadcast
void NetworkManager::broadcastMessage(const std::string& message, SOCKET excludeSocket) {
    std::lock_guard<std::mutex> lock(mtx);

    for (auto& client : clients) {
        if (client.socket != excludeSocket) {
            if (send(client.socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
                logger->log("Broadcast error");
            }
        }
    }
}
