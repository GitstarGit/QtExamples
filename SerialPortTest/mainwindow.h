#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initSerialPort();//初始化串口函数
private slots:
    void serialRead();
    void on_comboBox_activated(const QString &arg1);

    void on_btnSend_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort serial;//串口实例
};

#endif // MAINWINDOW_H
