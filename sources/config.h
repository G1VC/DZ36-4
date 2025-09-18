#pragma once

#define APP_VERSION "5.0.0"

// Nastroiki podklyucheniya
#define SERVER_ADDRESS "chat.server.com"
#define SERVER_PORT 9999

// Protokoly
#define PROTOCOL_VERSION "1.0"
#define ENCRYPTION_ENABLED true

// Formaty dannykh
#define MESSAGE_MAX_LENGTH 4096
#define USERNAME_MAX_LENGTH 32
#define PASSWORD_MIN_LENGTH 6

// Vremennye intervaly
#define HEARTBEAT_INTERVAL 30000    // 30 sekund
#define AUTO_SAVE_INTERVAL 60000    // 60 sekund
#define MESSAGE_TIMEOUT 120000      // 2 minuty

// Rezhimy raboty
enum class AppMode {
    NORMAL,
    DEBUG,
    TEST
};

// Konstanty dlya loggirovaniya
#define LOG_LEVEL_DEBUG 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_ERROR 4

// Puti k faylam
#define CONFIG_FILE "config.ini"
#define LOG_FILE "chat.log"
#define DATA_DIR "data/"
#define TEMP_DIR "temp/"

// Sistemnye nastroiki
#define USE_SYSTEM_TRAY true
#define AUTO_START false
#define NOTIFICATIONS_ENABLED true

// Bezopasnost
#define SECURE_CONNECTION true
#define SSL_CERT_FILE "cert.pem"
#define SSL_KEY_FILE "privkey.pem"

// Interfeys
#define DEFAULT_FONT "Arial"
#define DEFAULT_FONT_SIZE 12
#define THEME_LIGHT "light"
#define THEME_DARK "dark"
#define DEFAULT_THEME THEME_LIGHT

// Ogranicheniya
#define MAX_CONTACTS 1000
#define MAX_GROUPS 100
#define MAX_ATTACHMENTS 5

// Sistemnye soobsheniya
#define WELCOME_MESSAGE "Dobro pozhalovat v chat!"
#define GOODBYE_MESSAGE "Do svidaniya!"
#define ERROR_MESSAGE "Proizoshla oshibka"

// Nastroiki avtosohraneniya
#define AUTO_SAVE_HISTORY true
#define AUTO_SAVE_SETTINGS true
#define AUTO_SAVE_CONTACTS true

// Otladka
#ifdef _DEBUG
#define DEBUG_MODE true
#else
#define DEBUG_MODE false
#endif

// Makrosy dlya otladki
#ifdef DEBUG_MODE
#define LOG_DEBUG(msg) qDebug() << msg
#else
#define LOG_DEBUG(msg)
#endif

// Nastroiki API
#define API_KEY "your_api_key_here"
#define API_VERSION "2.0"
#define API_ENDPOINT "https://api.chat.com"

// Nastroiki uvedomleniy
#define NOTIFICATION_SOUND "notification.mp3"
#define NOTIFICATION_ICON "icon.png"
