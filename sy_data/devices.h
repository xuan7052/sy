#ifndef DEVICES_H
#define DEVICES_H

#include <QWidget>

#include "qmqtt.h"
#include <QDebug>
#include <QTimer>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace Ui {
class devices;
}

class devices : public QWidget
{
    Q_OBJECT

public:
    explicit devices(QWidget *parent = nullptr);
    ~devices();

private slots:

    //获取温度
    void GetTemp(const QMQTT::Message &msg1);
    void on_get_wendu_clicked();
    //获取湿度
    void GetHum(const QMQTT::Message &msg1);
    void on_get_shidu_clicked();

    //获取状态
    void GetState(const QMQTT::Message &msg1);
    void on_get_state_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

//    void on_pushButton_9_clicked();


private:
    Ui::devices *ui;

    //订阅主题
//    void SubTopic();

    //连接MQTT服务器
    void ConToHost();

    //声明指针
    QMQTT::Client *m_client1;

    //定时器
    QTimer *m_timer1s;

    //状态值
    int m_stateValue10;
    int m_stateValue11;
    int m_stateValue12;
    int m_stateValue13;
    int m_stateValue14;
    int m_stateValue15;
    int m_stateValue16;
    int m_stateValue17;

};

#endif // DEVICES_H
