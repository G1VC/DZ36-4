// main.cpp : Zdes' nachinaetsya i zakanchivaetsya vypolneniye programmy.

#include <QApplication>
#include <QTranslator>
#include "MainWindow.h"
#include "Logger.h"
#include "ChatManager.h"
#include "NetworkManager.h"
#include "SecurityManager.h"
#include "ServerMainWindow.h"
#include "ClientMainWindow.h"
using namespace std;

// Global'nye ob"ekty prilozheniya
Logger logger;
ChatManager chatManager;
NetworkManager networkManager;
SecurityManager securityManager;

using namespace std;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Podklyuchenie perevodchikov
    QTranslator appTranslator;
    appTranslator.load("translations/my_ru.qm");
    app.installTranslator(&appTranslator);

    QTranslator qtTranslator;
    qtTranslator.load("translations/qt_ru.qm");
    app.installTranslator(&qtTranslator);

    // Proverka rezhima zapuska
    if (argc > 1 && QString(argv[1]) == "-server") {
        // Zapusk servernoj chasti
        ServerMainWindow* serverWindow = new ServerMainWindow();
        serverWindow->show();
    }
    else {
        // Zapusk klientskoj chasti
        ClientMainWindow* clientWindow = ClientMainWindow::createClient();
        if (clientWindow) {
            clientWindow->show();
        }
        else {
            return 0;
        }
    }

    // Initsializatsiya loggera
    Logger logger;
    logger.log("Prilozhenie zapushcheno");

    return app.exec();
}
