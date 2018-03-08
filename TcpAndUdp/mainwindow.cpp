#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHostInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QNetworkInterface>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    port  = 45454;
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    sendMessage(NewParticipant);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newParticipant(QString userName, QString localHostName, QString ipAddress)
{
    bool isEmpty = ui->userTableWidget->findItems(localHostName,Qt::MatchExactly).isEmpty();
    if(isEmpty){
        QTableWidgetItem *user = new QTableWidgetItem(userName);
        QTableWidgetItem *host = new QTableWidgetItem(localHostName);
        QTableWidgetItem *ip = new QTableWidgetItem(ipAddress);

        ui->userTableWidget->insertRow(0);
        ui->userTableWidget->setItem(0,0,user);
        ui->userTableWidget->setItem(0,1,host);
        ui->userTableWidget->setItem(0,2,ip);

        ui->messageBrowser->setTextColor(Qt::gray);
        ui->messageBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->messageBrowser->append(tr("%1").arg(userName));
        ui->userNamelabel->setText(tr("%1").arg(ui->userTableWidget->rowCount()));
        sendMessage(NewParticipant);
    }
}

void MainWindow::participantLeft(QString userName, QString localHostName, QString time)
{
    int rowNum = ui->userTableWidget->findItems(localHostName,Qt::MatchExactly).first()->row();
    ui->userTableWidget->removeRow(rowNum);
    ui->messageBrowser->setTextColor(Qt::gray);
    ui->messageBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->messageBrowser->append(tr("%1 at %2 left! ").arg(userName).arg(time));
    ui->userNamelabel->setText(tr("%1").arg(ui->userTableWidget->rowCount()));
}

void MainWindow::sendMessage(MessageType type, QString serverAddress)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();
    QString address = getIp();

    out << type << getUserName() << localHostName;

    switch (type) {
    case Message:
        if(ui->messageTextEdit->toPlainText() == ""){
            QMessageBox::warning(0,tr("warning"),tr("kong"),QMessageBox::Ok);
            return;
        }
        out << address << getMessage();
        ui->messageBrowser->verticalScrollBar()->setValue(ui->messageTextEdit->verticalScrollBar()->maximum());
        break;
    case NewParticipant:
        out << address;
        break;
    case ParticipantLeft:
        break;
    case FileName:
        break;
    case Refuse:
        break;
    default:
        break;
    }
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast,port);
}
//获取ip地址
QString MainWindow::getIp()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol){
            return address.toString();
        }
    }
    return 0;
}

QString MainWindow::getUserName()
{
    QStringList enVariables;
    enVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*" << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList enviroment = QProcess::systemEnvironment();
    foreach (QString string, enVariables) {
        int index = enviroment.indexOf(QRegExp(string));
        if(index != -1){
            QStringList stringList = enviroment.at(index).split('=');
            if(stringList.size() == 2){
                return stringList.at(1);
                break;
            }
        }
    }
    return "unknown";
}

QString MainWindow::getMessage()
{
    QString msg = ui->messageTextEdit->toHtml();
    ui->messageTextEdit->clear();
    ui->messageTextEdit->setFocus();
    return msg;
}

void MainWindow::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        int messageType;
        in >> messageType;
        QString userName,localHostName,ipAddress,message;
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh::ss::mm");
        switch (messageType) {
        case Message:
            in >> userName >> localHostName >> ipAddress >> message;
            ui->messageBrowser->setTextColor(Qt::blue);
            ui->messageBrowser->setCurrentFont(QFont("Times New Roman",12));
            ui->messageBrowser->append("[" + userName + "]" + time);
            ui->messageBrowser->append(message);
            break;
        case NewParticipant:
            in >> userName >> localHostName >> ipAddress;
            newParticipant(userName,localHostName,ipAddress);
            break;
        case FileName:
            break;
        case Refuse:
            break;
        default:
            break;
        }
    }
}

void MainWindow::on_sendButton_clicked()
{
    sendMessage(Message);
}
