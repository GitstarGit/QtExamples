#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

namespace Ui {
class MainWindow;
}

enum MessageType{
    Message,
    NewParticipant,
    ParticipantLeft,
    FileName,
    Refuse
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void newParticipant(QString userName,QString localHostName,QString ipAddress);
    void participantLeft(QString userName,QString localHostName,QString time);
    void sendMessage(MessageType type,QString serverAddress = "");

    QString getIp();
    QString getUserName();
    QString getMessage();
private:
    Ui::MainWindow *ui;
    QUdpSocket *udpSocket;
    qint16 port;
private slots:
    void processPendingDatagrams();
    void on_sendButton_clicked();
};

#endif // MAINWINDOW_H
