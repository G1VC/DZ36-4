#include "Logger.h"
#include <iostream>
#include <fstream>
#include <mutex>
#include <chrono>
#include <string>
#include <vector>

Logger::Logger() {
    // Otkryvaem fayl v rezhime dobavleniya s sozdaniem, esli ne suschestvuet
    logFile.open("logs/chat_log.txt", std::ios::app | std::ios::in);
    if (!logFile.is_open()) {
        std::cerr << "Oshibka otkrytiya fayla logov\n";
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::log(const std::string& message, LogType type) {
    // Blokiruyem dlya zapisi
    std::unique_lock<std::shared_mutex> lock(mtx);

    // Dobavlyaem vremennuyu metku
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    // Formatiruyem soobshcheniye v zavisimosti ot tipa
    std::string prefix;
    switch (type) {
    case SYSTEM:
        prefix = "[SISTEMNOE] ";
        break;
    case CHAT:
        prefix = "[CHAT] ";
        break;
    case PRIVATE:
        prefix = "[PRIVAT] ";
        break;
    case MODERATION:
        prefix = "[MODERATSIYA] ";
        break;
    default:
        prefix = "[INFORMATSIYA] ";
    }

    logFile << "[" << std::ctime(&time) << "]"
        << prefix
        << message << "\n";
    logFile.flush();  // Garantiruem zapis' v fayl
}

std::string Logger::readLine() {
    // Blokiruyem dlya chteniya
    std::shared_lock<std::shared_mutex> lock(mtx);

    std::string line;
    if (std::getline(logFile, line)) {
        // Vozvrashchaemsya k nachalu fayla posle chteniya
        logFile.seekg(0, std::ios::beg);
        return line;
    }
    return "";
}

void Logger::clearLogs() {
    std::unique_lock<std::shared_mutex> lock(mtx);
    logFile.clear();
    logFile.seekp(0);
    logFile.close();
    logFile.open("logs/chat_log.txt", std::ios::trunc);
}

std::vector<std::string> Logger::getLogHistory() {
    std::shared_lock<std::shared_mutex> lock(mtx);
    std::vector<std::string> logHistory;
    std::string line;

    logFile.seekg(0, std::ios::beg);
    while (std::getline(logFile, line)) {
        logHistory.push_back(line);
    }

    return logHistory;
}
