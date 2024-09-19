#include "database.h"
#include "ui_database.h"

database::database(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::database)
{
    ui->setupUi(this);

    //定时器
    timer = new QTimer(this);
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(RD()));

    //设置窗口固定大小
    setFixedSize(770,455);

    //设置窗口名称
    setWindowTitle("———数据展示———");

    //创建一个QMQTT客户端对象并连接到本地服务器
    m_client = new QMQTT::Client(QHostAddress::LocalHost,1883,this);
    connect(m_client,SIGNAL(received(QMQTT::Message)),this,SLOT(GeDa(QMQTT::Message)));

    //定时器检查mqtt客户端连接状态
    m_timer1s = new QTimer(this);
    m_timer1s->start(1000);
    connect(this->m_timer1s,&QTimer::timeout,[this](void){
       if(this->m_client->isConnectedToHost() == false){
           ConToHost();
       }
    });
}



database::~database()
{
    delete m_timer1s;
    m_timer1s = nullptr;
    delete m_client;
    delete ui;
}



//连接MQTT服务器
void database::ConToHost()
{
    if(m_client->isConnectedToHost() == false)
    {
//        connect(m_client1,&QMQTT::Client::connected,[](void){});
        m_client->setHostName("broker.emqx.io");
        m_client->setPort(1883);
        m_client->setVersion(QMQTT::V3_1_1);
        m_client->setKeepAlive(60);
        m_client->cleanSession();
        m_client->connectToHost();

    }
    else{
        m_client->disconnectFromHost();
    }
}



//时间刷新
void database::RD()
{
    ui->time->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}



//订阅发送主题
void database::on_GetData_clicked()
{
    if(m_client->isConnectedToHost())
    {
    //订阅主题data_out1
    m_client->subscribe("data_out1",0);
    qDebug() << "订阅主题data_out1成功！";
    }
}



//获取数据
void database::GeDa(const QMQTT::Message &msg)
{
    //获取温度值
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("Temp"))
            {
                QString tempValue = jsonObject.value("Temp").toString();
                ui->Temp->setText(tempValue);
                qDebug() << "温度值：" << tempValue;
            }
        }
    }

    //获取氨气含量
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("NH3"))
            {
                QString NH3Value = jsonObject.value("NH3").toString();
                ui->NH3->setText(NH3Value);
                qDebug() << "氨气含量：" << NH3Value;
            }
        }
    }

    //获取溶解氧含量
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("DO"))
            {
                QString DOValue = jsonObject.value("DO").toString();
                ui->DO->setText(DOValue);
                qDebug() << "溶解氧含量：" << DOValue;
            }
        }
    }

    //浑浊度
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("Turb"))
            {
                QString TurbValue = jsonObject.value("Turb").toString();
                ui->Turb->setText(TurbValue);
                qDebug() << "浑浊度：" << TurbValue;
            }
        }
    }

    //溶解性固体总量
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("TDS"))
            {
                QString TDSValue = jsonObject.value("TDS").toString();
                ui->TDS->setText(TDSValue);
                qDebug() << "溶解性固体总量：" << TDSValue;
            }
        }
    }

    //水质电导率
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("Cond"))
            {
                QString CondValue = jsonObject.value("Cond").toString();
                ui->Cond->setText(CondValue);
                qDebug() << "水质电导率：" << CondValue;
            }
        }
    }

    //深度
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("Dept"))
            {
                QString DeptValue = jsonObject.value("Dept").toString();
                ui->Dept->setText(DeptValue);
                qDebug() << "深度：" << DeptValue;
            }
        }
    }

    //氨氮含量
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("NH3-N"))
            {
                QString NH3NValue = jsonObject.value("NH3-N").toString();
                ui->NH3N->setText(NH3NValue);
                qDebug() << "氨氮含量：" << NH3NValue;
            }
        }
    }

    //溶解氧含量
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("DO2"))
            {
                QString DO2Value = jsonObject.value("DO2").toString();
                ui->DOL->setText(DO2Value);
                qDebug() << "氨气含量：" << DO2Value;
            }
        }
    }

    //透明度
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("SD"))
            {
                QString SDValue = jsonObject.value("SD").toString();
                ui->SD->setText(SDValue);
                qDebug() << "透明度：" << SDValue;
            }
        }
    }

    //PH值
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("ORP"))
            {
                QString ORPValue = jsonObject.value("ORP").toString();
                ui->ORP->setText(ORPValue);
                qDebug() << "PH值：" << ORPValue;
            }
        }
    }

    //无菌保证水平
    if (m_client->isConnectedToHost())
    {
        // 接收数据并解析数据
        QString jsonData = msg.payload();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData.toUtf8());

        if (jsonDocument.isObject())
        {
            QJsonObject jsonObject = jsonDocument.object();
            if (jsonObject.contains("SAL"))
            {
                QString SALValue = jsonObject.value("SAL").toString();
                ui->SAL->setText(SALValue);
                qDebug() << "无菌保证水平：" << SALValue;
            }
        }
    }
}
