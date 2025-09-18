#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <vector>

// Tipy logov
enum class LogType {
    SYSTEM,    // Sistemnyye soobshcheniya
    CHAT,      // Soobshcheniya chata
    PRIVATE,   // Privatnyye soobshcheniya
    MODERATION // Dejstviya moderatsii
};

class Logger {
private:
    std::fstream logFile;
    std::shared_mutex mtx;  // Myuteks dlya potokobezopasnosti
    std::vector<std::string> logHistory;  // Istoriya logov

public:
    // Konstruktor otkryvaet fayl v rezhime dobavleniya
    Logger();

    // Destruktor zakryvaet fayl
    ~Logger();

    // Zapis' stroki v log s ukazaniem tipa
    void log(const std::string& message, LogType type);

    // Chteniye odnoy stroki iz loga
    std::string readLine();

    // Ochistka logov
    void clearLogs();

    // Polucheniye istorii logov
    std::vector<std::string> getLogHistory() const;

    // Sokhraneniye istorii logov v fayl
    void saveLogHistory();

    // Zagruzka istorii logov iz fayla
    void loadLogHistory();
};
