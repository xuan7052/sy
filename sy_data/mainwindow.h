#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "database.h"
#include "devices.h"
#include <qmqtt.h>
#include <QMessageBox>
#include <QTimer>
#include <QFile>
#include <QDialog>
#include <QString>
#include <QChartView>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void ttest();

private slots:

    void slot_mqtt_received(const QMQTT::Message &msg);

    void ReData_Slot();

    void timer_send_Slot();

    void on_close_triggered();

    void on_pbConnectToHost_clicked();

    void on_pcSubscribeTopic_clicked();

    void on_pbUnsubscribeTopic_clicked();

    void on_data_triggered();

    void on_devices_triggered();

    void on_exit_triggered();

private:
    Ui::MainWindow *ui;

    //表创建
    void createChart();

    //表刷新
    void DisplayChart();

    //数据解析
    void BackDataParsing(QString strBuf);

    //声明指针
    QMQTT::Client *m_client;

    //定时器
    QTimer *m_timer1s;

    //定时器 表格更新
    QTimer *timer;
    //定时器 发送更新
    QTimer *timer_send;

    //数据库窗口
    database *dbui;

    //设备管理窗口
    devices *dev;

private:
    QString get_cfg_str(const QString& str);

};
#endif // MAINWINDOW_H
