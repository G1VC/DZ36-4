#include "MessageListWidget.h"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QHelpEvent>
#include "MessageListItem.h"

// Konstruktor widgeta spiska soobsheniy
MessageListWidget::MessageListWidget(QWidget* parent)
    : QListWidget(parent),
    contextMenu(new QMenu(this))
{
    // Nastrojka stiley
    messageStyle = "background-color: white; border: 1px solid #e0e0e0; padding: 5px;";
    systemMessageStyle = "background-color: #f0f0f0; border: 1px solid #d0d0d0; padding: 5px;";
    privateMessageStyle = "background-color: #f9f9f9; border: 1px solid #c0c0c0; padding: 5px;";

    // Sozdanie deystviy dlya kontekstnogo menyu
    contextMenu->addAction(tr("Copy"), this, SLOT(onCopyMessage()));
    contextMenu->addAction(tr("Delete"), this, SLOT(onDeleteMessage()));
    contextMenu->addAction(tr("Details"), this, SLOT(onShowDetails()));
}

MessageListWidget::~MessageListWidget()
{
    delete contextMenu;
}

// Dobavlenie novogo soobsheniya
void MessageListWidget::addMessage(const Message& message)
{
    MessageListItem* item = new MessageListItem(message, this);

    // Ustanovka stiley
    if (message.isSystemMessage()) {
        item->setBackground(QBrush(QColor(240, 240, 240)));
    }
    else if (message.isPrivateMessage()) {
        item->setBackground(QBrush(QColor(249, 249, 249)));
    }

    addItem(item);
    scrollToBottom();
    currentMessage = message;
}

// Poluchenie tekushchego soobsheniya
Message MessageListWidget::getCurrentMessage() const
{
    return currentMessage;
}

// Ochistka spiska soobsheniy
void MessageListWidget::clearMessages()
{
    clear();
}

// Obrabotka kontekstnogo menyu
void MessageListWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QListWidgetItem* item = itemAt(event->pos());
    if (item) {
        currentMessage = static_cast<MessageListItem*>(item)->getMessage();
        contextMenu->exec(event->globalPos());
    }
}

// Obrabotka dvoynogo klika
void MessageListWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    QListWidgetItem* item = itemAt(event->pos());
    if (item) {
        currentMessage = static_cast<MessageListItem*>(item)->getMessage();
        emit messageSelected(currentMessage);
    }
    QListWidget::mouseDoubleClickEvent(event);
}

// Obrabotka klika
void MessageListWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QListWidgetItem* item = itemAt(event->pos());
        if (item) {
            currentMessage = static_cast<MessageListItem*>(item)->getMessage();
        }
    }
    QListWidget::mousePressEvent(event);
}

// Kopirovanie soobsheniya
void MessageListWidget::onCopyMessage()
{
    QApplication::clipboard()->setText(currentMessage.getContent());
}

// Udalenie soobsheniya
void MessageListWidget::onDeleteMessage()
{
    // Logika udaleniya
    emit contextMenuRequested(currentMessage);
}

// Prosmotr detaley
void MessageListWidget::onShowDetails()
{
    // Logika pokaza detaley
    emit contextMenuRequested(currentMessage);
}

// Realizatsiya MessageListMenu
MessageListMenu::MessageListMenu(QWidget* parent)
 : QMenu(parent)
{
 addAction(tr("Copy"), this, SLOT(copyMessage()));
 addAction(tr("Delete"), this, SLOT(deleteMessage()));
 addAction(tr("Details"), this, SLOT(showDetails()));
}

// Slot kopirovaniya
void MessageListMenu::copyMessage()
{
 QApplication::clipboard()->setText(currentMessage.getContent());
}

// Slot udaleniya
void MessageListMenu::deleteMessage()
{
 // Realizatsiya udaleniya
}

// Slot pokaza detaley
void MessageListMenu::showDetails()
{
 // Realizatsiya pokaza detaley soobsheniya
 QMessageBox::information(this, tr("Message details"),
 QString("Sender: %1\n"
 "Recipient: %2\n"
 "Time: %3\n"
 "Type: %4\n"
 "Message: %5")
 .arg(currentMessage.getSender()->getUsername())
 .arg(currentMessage.getRecipient())
 .arg(currentMessage.getFormattedTimestamp())
 .arg(currentMessage.getMessageType() == Message::PrivateMessage ? tr("Private") :
 (currentMessage.getMessageType() == Message::GroupMessage ? tr("Group") : tr("System")))
 .arg(currentMessage.getContent())
 );
}

// Pereopredelenie risovaniya elementov
void MessageListWidget::paintEvent(QPaintEvent* event)
{
 QStyleOptionViewItem option;
 option.state = QStyle::State_Enabled;
 option.rect = visualRect(currentItem());

 QPainter painter(this);
 style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, &painter, this);

 QListWidget::paintEvent(event);
}

// Obnovlenie stiley
void MessageListWidget::updateItemStyle(QListWidgetItem* item, const Message& message)
{
 if (message.isSystemMessage()) {
 item->setBackground(QBrush(QColor(240, 240, 240)));
 item->setForeground(QBrush(QColor(128, 128, 128)));
 }
 else if (message.isPrivateMessage()) {
 item->setBackground(QBrush(QColor(249, 249, 249)));
 item->setForeground(QBrush(QColor(0, 0, 255)));
 }
 else {
 item->setBackground(QBrush(QColor(255, 255, 255)));
 item->setForeground(QBrush(QColor(0, 0, 0)));
 }
}

// Sohranenie soobsheniya v fayl
void MessageListWidget::saveMessageToFile(const Message& message)
{
 QFile file(QString("message_%1.txt").arg(message.getId()));
 if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
 QTextStream out(&file);
 out << "Sender: " << message.getSender()->getUsername() << endl;
 out << "Recipient: " << message.getRecipient() << endl;
 out << "Time: " << message.getFormattedTimestamp() << endl;
 out << "Type: " << (message.isPrivateMessage() ? "Private" :
 (message.isGroupMessage() ? "Group" : "System")) << endl;
 out << "Message: " << message.getContent() << endl;
 file.close();
 }
}

// Kontekstnoe meny u dlya elementov
void MessageListWidget::customContextMenuRequested(const QPoint& pos)
{
 QListWidgetItem* item = itemAt(pos);
 if (item) {
 currentMessage = static_cast<MessageListItem*>(item)->getMessage();
 contextMenu->exec(mapToGlobal(pos));
 }
}
