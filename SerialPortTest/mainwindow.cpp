#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initSerialPort();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSerialPort()
{
    connect(&serial,SIGNAL(readyRead()),this,SLOT(serialRead()));
    //获取计算机上所有串口并添加到combox
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty()){
        ui->comboBox->addItem("noSerialPort");
        return;
    }
    foreach (QSerialPortInfo info, infos) {
        ui->comboBox->addItem(info.portName());
        qDebug() << info.portName();
    }
}

void MainWindow::serialRead()
{
    QByteArray qa = serial.readAll();
    ui->textEdit->append(qa);
}

void MainWindow::on_comboBox_activated(const QString &arg1)
{
    QSerialPortInfo info;
    QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    int i = 0;
    foreach (info, infos) {
        if(info.portName() == arg1)
            break;
        i++;
    }
    if(i != infos.size()){//can find
        ui->label->setText("serial port open success");
        serial.close();
        serial.setPort(info);
        bool ret = serial.open(QIODevice::ReadWrite);//读写打开
        qDebug() << ret;
        //serial.setBaudRate(QSerialPort::Baud9600);//波特率
        //serial.setDataBits(QSerialPort::Data8);//数据位
        //serial.setParity(QSerialPort::NoParity);//无奇偶校验
        //serial.setStopBits(QSerialPort::OneStop);//无停止位
        //serial.setFlowControl(QSerialPort::NoFlowControl);//无控制
    }else{
        serial.close();
        ui->label->setText("serial port open failure");
    }
}
//串口发送
void MainWindow::on_btnSend_clicked()
{
    char dat[] = {0x55,0x01,0x00,0xAA};
    serial.write(dat,5);
}
