#include "devices.h"
#include "ui_devices.h"

devices::devices(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::devices)
{
    ui->setupUi(this);

    //设置选项卡窗口小部件
    ui->tabWidget->setTabText(0,"电控箱1");
    ui->tabWidget->setTabText(1,"电控箱2");

    //设置窗口固定大小
    setFixedSize(815,671);

    //设置窗口名称
    setWindowTitle("——设备管理——");

    //创建一个QMQTT客户端对象并连接到本地服务器
    m_client1 = new QMQTT::Client(QHostAddress::LocalHost,1883,this);
    connect(m_client1,SIGNAL(received(QMQTT::Message)),this,SLOT(GetTemp(QMQTT::Message)));
    connect(m_client1,SIGNAL(received(QMQTT::Message)),this,SLOT(GetHum(QMQTT::Message)));
    connect(m_client1,SIGNAL(received(QMQTT::Message)),this,SLOT(GetState(QMQTT::Message)));

    //定时器检查mqtt客户端连接状态
    m_timer1s = new QTimer(this);
    m_timer1s->start(1000);
    connect(this->m_timer1s,&QTimer::timeout,[this](void){
       if(this->m_client1->isConnectedToHost() == false){
           ConToHost();
       }
    });

}



devices::~devices()
{
    delete m_timer1s;
    m_timer1s = nullptr;
    delete m_client1;
    delete ui;
}



//连接MQTT服务器
void devices::ConToHost()
{
    if(m_client1->isConnectedToHost() == false)
    {
//        connect(m_client1,&QMQTT::Client::connected,[](void){});
        m_client1->setHostName("broker.emqx.io");
        m_client1->setPort(1883);
//        m_client1->setUsername("");
//        m_client1->setPassword("");
        m_client1->setVersion(QMQTT::V3_1_1);
        m_client1->setKeepAlive(60);
        m_client1->cleanSession();
        m_client1->connectToHost();

    }
    else{
//        m_client1->unsubscribe("wendu_out");
//        m_client1->unsubscribe("shidu_out");
        m_client1->disconnectFromHost();
    }
}



//订阅发送主题
void devices::on_get_wendu_clicked()
{
    if(m_client1->isConnectedToHost())
    {
    //订阅主题wendu_out
    m_client1->subscribe("wendu_out",0);
    qDebug() << "订阅主题wendu_out成功！";

    //发送主题wendu
    QMQTT::Message msg(0,"wendu","GetTemp",0);
    m_client1->publish(msg);
    qDebug() << "发送主题wendu成功！";
    }
}



//获取温度信息
void devices::GetTemp(const QMQTT::Message &msg1)
{
    if(m_client1->isConnectedToHost())
    {
        //接收数据并解析数据
        QString jsonData = msg1.payload();
        QJsonDocument JsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if(JsonDocument.isArray()){
            QJsonArray jsonArray = JsonDocument.array();
            if(!jsonArray.isEmpty()){
                //获取第一个元素
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if(jsonObject.contains("wendu")){
                    QString wenduValue = jsonObject.value("wendu").toString();
                    ui->temp->setText("   " + wenduValue);
                    qDebug() << "温度值：" << wenduValue;
                }
            }
        }
    }

}



//订阅发送主题
void devices::on_get_shidu_clicked()
{
    if(m_client1->isConnectedToHost())
    {
    //订阅主题shidu_out
    m_client1->subscribe("shidu_out",0);
    qDebug() << "订阅主题shidu_out成功！";

    //发送主题shidu
    QMQTT::Message msg(0,"shidu","GetHum",0);
    m_client1->publish(msg);
    qDebug() << "发送主题shidu成功！";
    }
}



//获取湿度信息
void devices::GetHum(const QMQTT::Message &msg1)
{
    if(m_client1->isConnectedToHost())
    {
        //接收数据并解析数据
        QString jsonData = msg1.payload();
        QJsonDocument JsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if(JsonDocument.isArray()){
            QJsonArray jsonArray = JsonDocument.array();
            if(!jsonArray.isEmpty()){
                //获取第一个元素
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if(jsonObject.contains("shidu")){
                    QString shiduValue = jsonObject.value("shidu").toString();
                    ui->humidity->setText("   " + shiduValue);
                    qDebug() << "湿度值：" << shiduValue;
                }
            }
        }
    }

}



//获取状态
void devices::on_get_state_clicked()
{
    if(m_client1->isConnectedToHost())
    {
    //订阅主题out
    m_client1->subscribe("out",0);
    qDebug() << "订阅主题out成功！";

    //发送主题get
    QMQTT::Message msg(0,"get","Get",0);
    m_client1->publish(msg);
    qDebug() << "发送主题get成功！";
    }
}



//获取状态值
void devices::GetState(const QMQTT::Message &msg1)
{
    if(m_client1->isConnectedToHost())
    {
        //接收数据并解析数据（获取第一个数据）
        QString jsonData10 = msg1.payload();
        QJsonDocument JsonDocument10 = QJsonDocument::fromJson(jsonData10.toUtf8());

        if(JsonDocument10.isArray()){
            QJsonArray jsonArray = JsonDocument10.array();
            if(!jsonArray.isEmpty()){
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if (jsonObject.contains("subs")) {
                    QJsonArray subsArray = jsonObject.value("subs").toArray();
                    if (!subsArray.isEmpty()){
                        QJsonObject stateObject = subsArray.at(0).toObject();
                        if (stateObject.contains("state")) {
                            m_stateValue10 = stateObject.value("state").toInt();
                            qDebug() << "state：" << m_stateValue10;
                        }
                    }
                }
            }
        }

        //接收数据并解析数据（获取第二个数据）
        QString jsonData11 = msg1.payload();
        QJsonDocument JsonDocument11 = QJsonDocument::fromJson(jsonData11.toUtf8());

        if(JsonDocument11.isArray()){
            QJsonArray jsonArray = JsonDocument11.array();
            if(!jsonArray.isEmpty()){
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if (jsonObject.contains("subs")) {
                    QJsonArray subsArray = jsonObject.value("subs").toArray();
                    if (!subsArray.isEmpty()){
                        QJsonObject stateObject = subsArray.at(1).toObject();
                        if (stateObject.contains("state")) {
                            m_stateValue11 = stateObject.value("state").toInt();
                            qDebug() << "state：" << m_stateValue11;
                        }
                    }
                }
            }
        }

        //接收数据并解析数据（获取第三个数据）
        QString jsonData12 = msg1.payload();
        QJsonDocument JsonDocument12 = QJsonDocument::fromJson(jsonData12.toUtf8());

        if(JsonDocument12.isArray()){
            QJsonArray jsonArray = JsonDocument12.array();
            if(!jsonArray.isEmpty()){
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if (jsonObject.contains("subs")) {
                    QJsonArray subsArray = jsonObject.value("subs").toArray();
                    if (!subsArray.isEmpty()){
                        QJsonObject stateObject = subsArray.at(2).toObject();
                        if (stateObject.contains("state")) {
                            m_stateValue12 = stateObject.value("state").toInt();
                            qDebug() << "state：" << m_stateValue12;
                        }
                    }
                }
            }
        }

        //接收数据并解析数据（获取第四个数据）
        QString jsonData13 = msg1.payload();
        QJsonDocument JsonDocument13 = QJsonDocument::fromJson(jsonData13.toUtf8());

        if(JsonDocument13.isArray()){
            QJsonArray jsonArray = JsonDocument13.array();
            if(!jsonArray.isEmpty()){
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if (jsonObject.contains("subs")) {
                    QJsonArray subsArray = jsonObject.value("subs").toArray();
                    if (!subsArray.isEmpty()){
                        QJsonObject stateObject = subsArray.at(3).toObject();
                        if (stateObject.contains("state")) {
                            m_stateValue13 = stateObject.value("state").toInt();
                            qDebug() << "state：" << m_stateValue13;
                        }
                    }
                }
            }
        }

        //接收数据并解析数据（获取第五个数据）
        QString jsonData14 = msg1.payload();
        QJsonDocument JsonDocument14 = QJsonDocument::fromJson(jsonData14.toUtf8());

        if(JsonDocument14.isArray()){
            QJsonArray jsonArray = JsonDocument14.array();
            if(!jsonArray.isEmpty()){
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if (jsonObject.contains("subs")) {
                    QJsonArray subsArray = jsonObject.value("subs").toArray();
                    if (!subsArray.isEmpty()){
                        QJsonObject stateObject = subsArray.at(4).toObject();
                        if (stateObject.contains("state")) {
                            m_stateValue14 = stateObject.value("state").toInt();
                            qDebug() << "state：" << m_stateValue14;
                        }
                    }
                }
            }
        }

        //接收数据并解析数据（获取第六个数据）
        QString jsonData15 = msg1.payload();
        QJsonDocument JsonDocument15 = QJsonDocument::fromJson(jsonData15.toUtf8());

        if(JsonDocument15.isArray()){
            QJsonArray jsonArray = JsonDocument15.array();
            if(!jsonArray.isEmpty()){
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if (jsonObject.contains("subs")) {
                    QJsonArray subsArray = jsonObject.value("subs").toArray();
                    if (!subsArray.isEmpty()){
                        QJsonObject stateObject = subsArray.at(5).toObject();
                        if (stateObject.contains("state")) {
                            m_stateValue15 = stateObject.value("state").toInt();
                            qDebug() << "state：" << m_stateValue15;
                        }
                    }
                }
            }
        }

        //接收数据并解析数据（获取第七个数据）
        QString jsonData16 = msg1.payload();
        QJsonDocument JsonDocument16 = QJsonDocument::fromJson(jsonData16.toUtf8());

        if(JsonDocument16.isArray()){
            QJsonArray jsonArray = JsonDocument16.array();
            if(!jsonArray.isEmpty()){
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if (jsonObject.contains("subs")) {
                    QJsonArray subsArray = jsonObject.value("subs").toArray();
                    if (!subsArray.isEmpty()){
                        QJsonObject stateObject = subsArray.at(6).toObject();
                        if (stateObject.contains("state")) {
                            m_stateValue16 = stateObject.value("state").toInt();
                            qDebug() << "state：" << m_stateValue16;
                        }
                    }
                }
            }
        }

        //接收数据并解析数据（获取第八个数据）
        QString jsonData17 = msg1.payload();
        QJsonDocument JsonDocument17 = QJsonDocument::fromJson(jsonData17.toUtf8());

        if(JsonDocument17.isArray()){
            QJsonArray jsonArray = JsonDocument17.array();
            if(!jsonArray.isEmpty()){
                QJsonObject jsonObject = jsonArray.at(0).toObject();
                if (jsonObject.contains("subs")) {
                    QJsonArray subsArray = jsonObject.value("subs").toArray();
                    if (!subsArray.isEmpty()){
                        QJsonObject stateObject = subsArray.at(7).toObject();
                        if (stateObject.contains("state")) {
                            m_stateValue17 = stateObject.value("state").toInt();
                            qDebug() << "state：" << m_stateValue17;
                        }
                    }
                }
            }
        }

        QPixmap mypix_1(":/device_open.png");
        QPixmap mypix_2(":/device_close.png");

        //开关总闸1
        if(m_stateValue10 == 0){
            ui->pushButton->setText("开启");
            ui->status_10->setPixmap(mypix_2);
            ui->status_10->setScaledContents(true);
            ui->status_10->show();

//            ui->pushButton_2->setText("开启");
//            ui->status_11->setPixmap(mypix_2);
//            ui->status_11->setScaledContents(true);
//            ui->status_11->show();

//            ui->pushButton_3->setText("开启");
//            ui->status_12->setPixmap(mypix_2);
//            ui->status_12->setScaledContents(true);
//            ui->status_12->show();

//            ui->pushButton_4->setText("开启");
//            ui->status_13->setPixmap(mypix_2);
//            ui->status_13->setScaledContents(true);
//            ui->status_13->show();

//            ui->pushButton_5->setText("开启");
//            ui->status_14->setPixmap(mypix_2);
//            ui->status_14->setScaledContents(true);
//            ui->status_14->show();

//            ui->pushButton_6->setText("开启");
//            ui->status_15->setPixmap(mypix_2);
//            ui->status_15->setScaledContents(true);
//            ui->status_15->show();

//            ui->pushButton_7->setText("开启");
//            ui->status_16->setPixmap(mypix_2);
//            ui->status_16->setScaledContents(true);
//            ui->status_16->show();

//            ui->pushButton_8->setText("开启");
//            ui->status_17->setPixmap(mypix_2);
//            ui->status_17->setScaledContents(true);
//            ui->status_17->show();

        }else{
            ui->pushButton->setText("关闭");
            ui->status_10->setPixmap(mypix_1);
            ui->status_10->setScaledContents(true);
            ui->status_10->show();

//            ui->pushButton_2->setText("关闭");
//            ui->status_11->setPixmap(mypix_1);
//            ui->status_11->setScaledContents(true);
//            ui->status_11->show();

//            ui->pushButton_3->setText("关闭");
//            ui->status_12->setPixmap(mypix_1);
//            ui->status_12->setScaledContents(true);
//            ui->status_12->show();

//            ui->pushButton_4->setText("关闭");
//            ui->status_13->setPixmap(mypix_1);
//            ui->status_13->setScaledContents(true);
//            ui->status_13->show();

//            ui->pushButton_5->setText("关闭");
//            ui->status_14->setPixmap(mypix_1);
//            ui->status_14->setScaledContents(true);
//            ui->status_14->show();

//            ui->pushButton_6->setText("关闭");
//            ui->status_15->setPixmap(mypix_1);
//            ui->status_15->setScaledContents(true);
//            ui->status_15->show();

//            ui->pushButton_7->setText("关闭");
//            ui->status_16->setPixmap(mypix_1);
//            ui->status_16->setScaledContents(true);
//            ui->status_16->show();

//            ui->pushButton_8->setText("关闭");
//            ui->status_17->setPixmap(mypix_1);
//            ui->status_17->setScaledContents(true);
//            ui->status_17->show();

        }

        //喷泉1
        if(m_stateValue11 == 0){
            ui->pushButton_2->setText("开启");
            ui->status_11->setPixmap(mypix_2);
            ui->status_11->setScaledContents(true);
            ui->status_11->show();
        }else{
            ui->pushButton_2->setText("关闭");
            ui->status_11->setPixmap(mypix_1);
            ui->status_11->setScaledContents(true);
            ui->status_11->show();
        }

        //喷泉2
        if(m_stateValue12 == 0){
            ui->pushButton_3->setText("开启");
            ui->status_12->setPixmap(mypix_2);
            ui->status_12->setScaledContents(true);
            ui->status_12->show();
        }else{
            ui->pushButton_3->setText("关闭");
            ui->status_12->setPixmap(mypix_1);
            ui->status_12->setScaledContents(true);
            ui->status_12->show();
        }

        //喷泉3
        if(m_stateValue13 == 0){
            ui->pushButton_4->setText("开启");
            ui->status_13->setPixmap(mypix_2);
            ui->status_13->setScaledContents(true);
            ui->status_13->show();
        }else{
            ui->pushButton_4->setText("关闭");
            ui->status_13->setPixmap(mypix_1);
            ui->status_13->setScaledContents(true);
            ui->status_13->show();
        }

        //喷泉4
        if(m_stateValue14 == 0){
            ui->pushButton_5->setText("开启");
            ui->status_14->setPixmap(mypix_2);
            ui->status_14->setScaledContents(true);
            ui->status_14->show();
        }else{
            ui->pushButton_5->setText("关闭");
            ui->status_14->setPixmap(mypix_1);
            ui->status_14->setScaledContents(true);
            ui->status_14->show();
        }

        //喷泉5
        if(m_stateValue15 == 0){
            ui->pushButton_6->setText("开启");
            ui->status_15->setPixmap(mypix_2);
            ui->status_15->setScaledContents(true);
            ui->status_15->show();
        }else{
            ui->pushButton_6->setText("关闭");
            ui->status_15->setPixmap(mypix_1);
            ui->status_15->setScaledContents(true);
            ui->status_15->show();
        }

        //搅拌机1
        if(m_stateValue16 == 0){
            ui->pushButton_7->setText("开启");
            ui->status_16->setPixmap(mypix_2);
            ui->status_16->setScaledContents(true);
            ui->status_16->show();
        }else{
            ui->pushButton_7->setText("关闭");
            ui->status_16->setPixmap(mypix_1);
            ui->status_16->setScaledContents(true);
            ui->status_16->show();
        }

        //水泵1
        if(m_stateValue17 == 0){
            ui->pushButton_8->setText("开启");
            ui->status_17->setPixmap(mypix_2);
            ui->status_17->setScaledContents(true);
            ui->status_17->show();
        }else{
            ui->pushButton_8->setText("关闭");
            ui->status_17->setPixmap(mypix_1);
            ui->status_17->setScaledContents(true);
            ui->status_17->show();
        }

    }
}



//开关总阀1
void devices::on_pushButton_clicked()
{
    QPixmap mypix_1(":/device_open.png");
    QPixmap mypix_2(":/device_close.png");

    if(ui->pushButton->text() == "开启"){

        ui->pushButton->setText("关闭");
        ui->status_10->setPixmap(mypix_1);
        ui->status_10->setScaledContents(true);
        ui->status_10->show();
        QMQTT::Message msg(0,"sw_on","on",0);
        m_client1->publish(msg);
        qDebug() << "发送主题sw_on成功！";

        ui->pushButton_2->setText("关闭");
        ui->status_11->setPixmap(mypix_1);
        ui->status_11->setScaledContents(true);
        ui->status_11->show();
        QMQTT::Message msg1(0,"fo1_open","on",0);
        m_client1->publish(msg1);
        qDebug() << "发送主题fo1_open成功！";

        ui->pushButton_3->setText("关闭");
        ui->status_12->setPixmap(mypix_1);
        ui->status_12->setScaledContents(true);
        ui->status_12->show();
        QMQTT::Message msg2(0,"fo2_open","open",0);
        m_client1->publish(msg2);
        qDebug() << "发送主题fo2_open成功！";

        ui->pushButton_4->setText("关闭");
        ui->status_13->setPixmap(mypix_1);
        ui->status_13->setScaledContents(true);
        ui->status_13->show();
        QMQTT::Message msg3(0,"fo3_open","open",0);
        m_client1->publish(msg3);
        qDebug() << "发送主题fo3_open成功！";

        ui->pushButton_5->setText("关闭");
        ui->status_14->setPixmap(mypix_1);
        ui->status_14->setScaledContents(true);
        ui->status_14->show();
        QMQTT::Message msg4(0,"fo4_open","open",0);
        m_client1->publish(msg4);
        qDebug() << "发送主题fo4_open成功！";

        ui->pushButton_6->setText("关闭");
        ui->status_15->setPixmap(mypix_1);
        ui->status_15->setScaledContents(true);
        ui->status_15->show();
        QMQTT::Message msg5(0,"fo5_open","open",0);
        m_client1->publish(msg5);
        qDebug() << "发送主题fo5_open成功！";

        ui->pushButton_7->setText("关闭");
        ui->status_16->setPixmap(mypix_1);
        ui->status_16->setScaledContents(true);
        ui->status_16->show();
        QMQTT::Message msg6(0,"mixer_open","open",0);
        m_client1->publish(msg6);
        qDebug() << "发送主题mixer_open成功！";

        ui->pushButton_8->setText("关闭");
        ui->status_17->setPixmap(mypix_1);
        ui->status_17->setScaledContents(true);
        ui->status_17->show();
        QMQTT::Message msg7(0,"pump_open","open",0);
        m_client1->publish(msg7);
        qDebug() << "发送主题pump_open成功！";

    }
    else
    if(ui->pushButton->text() == "关闭"){

        ui->pushButton->setText("开启");
        ui->status_10->setPixmap(mypix_2);
        ui->status_10->setScaledContents(true);
        ui->status_10->show();
        QMQTT::Message msg(0,"sw_off","off",0);
        m_client1->publish(msg);
        qDebug() << "发送主题sw_off成功！";

        ui->pushButton_2->setText("开启");
        ui->status_11->setPixmap(mypix_2);
        ui->status_11->setScaledContents(true);
        ui->status_11->show();
        QMQTT::Message msg1(0,"fo1_close","close",0);
        m_client1->publish(msg1);
        qDebug() << "发送主题fo1_close成功！";

        ui->pushButton_3->setText("开启");
        ui->status_12->setPixmap(mypix_2);
        ui->status_12->setScaledContents(true);
        ui->status_12->show();
        QMQTT::Message msg2(0,"fo2_close","close",0);
        m_client1->publish(msg2);
        qDebug() << "发送主题fo2_close成功！";

        ui->pushButton_4->setText("开启");
        ui->status_13->setPixmap(mypix_2);
        ui->status_13->setScaledContents(true);
        ui->status_13->show();
        QMQTT::Message msg3(0,"fo3_close","close",0);
        m_client1->publish(msg3);
        qDebug() << "发送主题fo3_close成功！";

        ui->pushButton_5->setText("开启");
        ui->status_14->setPixmap(mypix_2);
        ui->status_14->setScaledContents(true);
        ui->status_14->show();
        QMQTT::Message msg4(0,"fo4_close","close",0);
        m_client1->publish(msg4);
        qDebug() << "发送主题fo4_close成功！";

        ui->pushButton_6->setText("开启");
        ui->status_15->setPixmap(mypix_2);
        ui->status_15->setScaledContents(true);
        ui->status_15->show();
        QMQTT::Message msg5(0,"fo5_close","close",0);
        m_client1->publish(msg5);
        qDebug() << "发送主题fo5_close成功！";

        ui->pushButton_7->setText("开启");
        ui->status_16->setPixmap(mypix_2);
        ui->status_16->setScaledContents(true);
        ui->status_16->show();
        QMQTT::Message msg6(0,"mixer_close","close",0);
        m_client1->publish(msg6);
        qDebug() << "发送主题mixer_close成功！";

        ui->pushButton_8->setText("开启");
        ui->status_17->setPixmap(mypix_2);
        ui->status_17->setScaledContents(true);
        ui->status_17->show();
        QMQTT::Message msg7(0,"pump_close","close",0);
        m_client1->publish(msg7);
        qDebug() << "发送主题pump_close成功！";

    }
}



//喷泉1
void devices::on_pushButton_2_clicked()
{
    QPixmap mypix_1(":/device_open.png");
    QPixmap mypix_2(":/device_close.png");

    if(ui->pushButton_2->text() == "开启"){
        ui->pushButton_2->setText("关闭");
        ui->status_11->setPixmap(mypix_1);
        ui->status_11->setScaledContents(true);
        ui->status_11->show();
        QMQTT::Message msg1(0,"fo1_open","on",0);
        m_client1->publish(msg1);
        qDebug() << "发送主题fo1_open成功！";
    }
    else
    if(ui->pushButton_2->text() == "关闭"){
        ui->pushButton_2->setText("开启");
        ui->status_11->setPixmap(mypix_2);
        ui->status_11->setScaledContents(true);
        ui->status_11->show();
        QMQTT::Message msg1(0,"fo1_close","close",0);
        m_client1->publish(msg1);
        qDebug() << "发送主题fo1_close成功！";
    }
}



//喷泉2
void devices::on_pushButton_3_clicked()
{
    QPixmap mypix_1(":/device_open.png");
    QPixmap mypix_2(":/device_close.png");

    if(ui->pushButton_3->text() == "开启"){
        ui->pushButton_3->setText("关闭");
        ui->status_12->setPixmap(mypix_1);
        ui->status_12->setScaledContents(true);
        ui->status_12->show();
        QMQTT::Message msg2(0,"fo2_open","open",0);
        m_client1->publish(msg2);
        qDebug() << "发送主题fo2_open成功！";
    }
    else
    if(ui->pushButton_3->text() == "关闭"){
        ui->pushButton_3->setText("开启");
        ui->status_12->setPixmap(mypix_2);
        ui->status_12->setScaledContents(true);
        ui->status_12->show();
        QMQTT::Message msg2(0,"fo2_close","close",0);
        m_client1->publish(msg2);
        qDebug() << "发送主题fo2_close成功！";
    }
}



//喷泉3
void devices::on_pushButton_4_clicked()
{
    QPixmap mypix_1(":/device_open.png");
    QPixmap mypix_2(":/device_close.png");

    if(ui->pushButton_4->text() == "开启"){
        ui->pushButton_4->setText("关闭");
        ui->status_13->setPixmap(mypix_1);
        ui->status_13->setScaledContents(true);
        ui->status_13->show();
        QMQTT::Message msg3(0,"fo3_open","open",0);
        m_client1->publish(msg3);
        qDebug() << "发送主题fo3_open成功！";
    }
    else
    if(ui->pushButton_4->text() == "关闭"){
        ui->pushButton_4->setText("开启");
        ui->status_13->setPixmap(mypix_2);
        ui->status_13->setScaledContents(true);
        ui->status_13->show();
        QMQTT::Message msg3(0,"fo3_close","close",0);
        m_client1->publish(msg3);
        qDebug() << "发送主题fo3_close成功！";
    }
}



//喷泉4
void devices::on_pushButton_5_clicked()
{
    QPixmap mypix_1(":/device_open.png");
    QPixmap mypix_2(":/device_close.png");

    if(ui->pushButton_5->text() == "开启"){
        ui->pushButton_5->setText("关闭");
        ui->status_14->setPixmap(mypix_1);
        ui->status_14->setScaledContents(true);
        ui->status_14->show();
        QMQTT::Message msg4(0,"fo4_open","open",0);
        m_client1->publish(msg4);
        qDebug() << "发送主题fo4_open成功！";
    }
    else
    if(ui->pushButton_5->text() == "关闭"){
        ui->pushButton_5->setText("开启");
        ui->status_14->setPixmap(mypix_2);
        ui->status_14->setScaledContents(true);
        ui->status_14->show();
        QMQTT::Message msg4(0,"fo4_close","close",0);
        m_client1->publish(msg4);
        qDebug() << "发送主题fo4_close成功！";
    }
}



//喷泉5
void devices::on_pushButton_6_clicked()
{
    QPixmap mypix_1(":/device_open.png");
    QPixmap mypix_2(":/device_close.png");

    if(ui->pushButton_6->text() == "开启"){
        ui->pushButton_6->setText("关闭");
        ui->status_15->setPixmap(mypix_1);
        ui->status_15->setScaledContents(true);
        ui->status_15->show();
        QMQTT::Message msg5(0,"fo5_open","open",0);
        m_client1->publish(msg5);
        qDebug() << "发送主题fo5_open成功！";
    }
    else
    if(ui->pushButton_6->text() == "关闭"){
        ui->pushButton_6->setText("开启");
        ui->status_15->setPixmap(mypix_2);
        ui->status_15->setScaledContents(true);
        ui->status_15->show();
        QMQTT::Message msg5(0,"fo5_close","close",0);
        m_client1->publish(msg5);
        qDebug() << "发送主题fo5_close成功！";
    }
}



//搅拌机1
void devices::on_pushButton_7_clicked()
{
    QPixmap mypix_1(":/device_open.png");
    QPixmap mypix_2(":/device_close.png");

    if(ui->pushButton_7->text() == "开启"){
        ui->pushButton_7->setText("关闭");
        ui->status_16->setPixmap(mypix_1);
        ui->status_16->setScaledContents(true);
        ui->status_16->show();
        QMQTT::Message msg6(0,"mixer_open","open",0);
        m_client1->publish(msg6);
        qDebug() << "发送主题mixer_open成功！";
    }
    else
    if(ui->pushButton_7->text() == "关闭"){
        ui->pushButton_7->setText("开启");
        ui->status_16->setPixmap(mypix_2);
        ui->status_16->setScaledContents(true);
        ui->status_16->show();
        QMQTT::Message msg6(0,"mixer_close","close",0);
        m_client1->publish(msg6);
        qDebug() << "发送主题mixer_close成功！";
    }
}



//水泵1
void devices::on_pushButton_8_clicked()
{
    QPixmap mypix_1(":/device_open.png");
    QPixmap mypix_2(":/device_close.png");

    if(ui->pushButton_8->text() == "开启"){
        ui->pushButton_8->setText("关闭");
        ui->status_17->setPixmap(mypix_1);
        ui->status_17->setScaledContents(true);
        ui->status_17->show();
        QMQTT::Message msg7(0,"pump_open","open",0);
        m_client1->publish(msg7);
        qDebug() << "发送主题pump_open成功！";
    }
    else
    if(ui->pushButton_8->text() == "关闭"){
        ui->pushButton_8->setText("开启");
        ui->status_17->setPixmap(mypix_2);
        ui->status_17->setScaledContents(true);
        ui->status_17->show();
        QMQTT::Message msg7(0,"pump_close","close",0);
        m_client1->publish(msg7);
        qDebug() << "发送主题pump_close成功！";
    }
}





