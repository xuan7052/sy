#ifndef DATABASE_H
#define DATABASE_H

#include <QWidget>
#include <qmqtt.h>
#include <QTimer>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace Ui {
class database;
}

class database : public QWidget
{
    Q_OBJECT

public:
    explicit database(QWidget *parent = nullptr);
    ~database();

private slots:

    //刷新
    void RD();

    //订阅发送主题
    void on_GetData_clicked();

    //数据获取
    void GeDa(const QMQTT::Message &msg);

private:
    Ui::database *ui;

    //声明指针
    QMQTT::Client *m_client;

    //定时器
    QTimer *timer;
    QTimer *m_timer1s;

    //连接MQTT服务器
    void ConToHost();

};

#endif // DATABASE_H
