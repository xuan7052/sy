#include "mainwindow.h"
#include <QChartView>   //图表视图类
#include <QLineSeries>  //线条类
#include <QtMath>
#include <QTimer>
#include <QDateTime>
#include <QDateTimeAxis>    //时间坐标轴类
#include <QValueAxis>       //普通坐标轴类
#include <QTextCodec>       //处理文本编码类
#include <QMessageBox>
#include <string>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

#include "ui_mainwindow.h"


//本机发送标志位
bool flag_send=0;



//数据变量
float Temp_data;
float NH3_data;
float DO_data;
float Dept_data;
float Turb_data;
float TDS_data;
float Cond_data;
float NH3N_data;
float DOL_data;
float SD_data;
float ORP_data;
float SAL_data;



//发送缓冲区
//extern QString strbuf;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //创建一个QMQTT客户端对象并连接到服务器
    m_client = new QMQTT::Client(QHostAddress(QHostAddress::LocalHost),1883,this);
    connect(m_client,SIGNAL(received(QMQTT::Message)),this,SLOT(slot_mqtt_received(QMQTT::Message)));
//    connect(m_client,SIGNAL(received(QMQTT::Message)),this,SLOT(readyRead_Slot()));

    //初始化下拉框
    ui->cbxQosSubscribe->addItem("0");
    ui->cbxQosSubscribe->addItem("1");
    ui->cbxQosSubscribe->addItem("2");

    ui->leHostaddr->setText("broker.emqx.io");
    ui->lePort->setText("1883");
    ui->leKeepAlive->setText("60");
    ui->leSubscribeTopic->setText("data_out");


    //定时器检查mqtt客户端连接状态
    m_timer1s = new QTimer(this);
    m_timer1s->start(1000);
    connect(this->m_timer1s,&QTimer::timeout,[this](void){
       if(this->m_client->isConnectedToHost() == false){
           ui->pbConnectToHost->setText("连接");
           ui->cbxSubscribedTopic->clear();
       }
    });

    //读取本地配置
    QFile f(QCoreApplication::applicationDirPath() + "/config.ini");
    if(f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->leHostaddr->setText(get_cfg_str(f.readLine()));
        ui->lePort->setText(get_cfg_str(f.readLine()));
        ui->leClientId->setText(get_cfg_str(f.readLine()));
        ui->leUsername->setText(get_cfg_str(f.readLine()));
        ui->lePassword->setText(get_cfg_str(f.readLine()));
        ui->leKeepAlive->setText(get_cfg_str(f.readLine()));
        f.close();
    }

    //创建表
    createChart();

    //创建定时器 表格更新
    timer = new QTimer(this);
    timer->start(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(ReData_Slot()));

    //创建定时器 发送更新
//    timer_send = new QTimer(this);
//    timer_send->start(20);
//    connect(timer_send,SIGNAL(timeout()),this,SLOT(timer_send_Slot()));

    //初始化数据库
    dbui = new database;

    //初始化设备管理
    dev = new devices;

    //工具栏大小设置
//    QSize si;
//    si.setWidth(40);
//    si.setHeight(40);
//    ui->toolBar->setIconSize(si);

    //设置窗口固定大小
    setFixedSize(1920,540);

    //设置窗口名称
    setWindowTitle("\"净源\"河道监测软件（V1.0）");

}



//提取值
QString MainWindow::get_cfg_str(const QString &str)
{
    std::string rawstr = str.trimmed().toStdString();
    int index = rawstr.find("=") + 1;

    if(index > 0){
        std::string cfgstr = rawstr.substr(index,rawstr.size() - 1);
        return QString(cfgstr.c_str());
    }
    return NULL;
}



MainWindow::~MainWindow()
{
    if(m_client->isConnectedToHost())
    {
        for(int i = 0; i != ui->cbxSubscribedTopic->count(); i++)
        {
            m_client->unsubscribe(ui->cbxSubscribedTopic->itemText(i));
        }
        m_client->disconnected();
    }

    delete m_timer1s;
    m_timer1s = nullptr;
    delete timer;
    timer=nullptr;
    delete m_client;
    delete ui;
}



//发送标志位
void MainWindow::ttest()
{
    flag_send = 1;
}



//数据解析
void MainWindow::BackDataParsing(QString strBuf)
{
        //表中数据处理
        QString str = strBuf.mid(strBuf.indexOf("Temp:")+((QString)"Temp:").length(),strBuf.indexOf("NH3:")-strBuf.indexOf("Temp:")-((QString)"Temp:").length()-1);
        QString st1 = strBuf.mid(strBuf.indexOf("NH3:")+((QString)"NH3:").length(),strBuf.indexOf("DO:")-strBuf.indexOf("NH3:")-((QString)"NH3:").length()-1);
        QString st2 = strBuf.mid(strBuf.indexOf("DO:")+((QString)"DO:").length(),strBuf.indexOf("Dept:")-strBuf.indexOf("DO:")-((QString)"DO:").length()-1);
        QString st3 = strBuf.mid(strBuf.indexOf("Dept:")+((QString)"Dept:").length(),strBuf.indexOf("Turb:")-strBuf.indexOf("Dept:")-((QString)"Dept:").length()-1);
        QString st4 = strBuf.mid(strBuf.indexOf("Turb:")+((QString)"Turb:").length(),strBuf.indexOf("TDS:")-strBuf.indexOf("Turb:")-((QString)"Turb:").length()-1);
        QString st5 = strBuf.mid(strBuf.indexOf("TDS:")+((QString)"TDS:").length(),strBuf.indexOf("Cond:")-strBuf.indexOf("TDS:")-((QString)"TDS:").length()-1);
        QString st6 = strBuf.mid(strBuf.indexOf("Cond:")+((QString)"Cond:").length(),strBuf.indexOf("NH3-N:")-strBuf.indexOf("Cond:")-((QString)"Cond:").length()-1);
        QString st7 = strBuf.mid(strBuf.indexOf("NH3-N:")+((QString)"NH3-N:").length(),strBuf.indexOf("DO2:")-strBuf.indexOf("NH3-N:")-((QString)"NH3-N:").length()-1);
        QString st8 = strBuf.mid(strBuf.indexOf("DO2:")+((QString)"DO2:").length(),strBuf.indexOf("SD:")-strBuf.indexOf("DO2:")-((QString)"DO2:").length()-1);
        QString st9 = strBuf.mid(strBuf.indexOf("SD:")+((QString)"SD:").length(),strBuf.indexOf("ORP:")-strBuf.indexOf("SD:")-((QString)"SD:").length()-1);
        QString st10 = strBuf.mid(strBuf.indexOf("ORP:")+((QString)"ORP:").length(),strBuf.indexOf("SAL:")-strBuf.indexOf("ORP:")-((QString)"ORP:").length()-1);
        QString st11 = strBuf.mid(strBuf.indexOf("SAL:")+((QString)"SAL:").length(),strBuf.indexOf("}")-strBuf.indexOf("SAL:")-((QString)"SAL:").length());

        //更新至表格
        Temp_data = str.toFloat();
        NH3_data = st1.toFloat();
        DO_data = st2.toFloat();
        Dept_data = st3.toFloat();
        Turb_data = st4.toFloat();
        TDS_data = st5.toFloat();
        Cond_data = st6.toFloat();
        NH3N_data = st7.toFloat();
        DOL_data = st8.toFloat();
        SD_data = st9.toFloat();
        ORP_data = st10.toFloat();
        SAL_data = st11.toFloat();

//        //送入数据库
//        dbui->UpdataToDataBase(str,st1,st2,st3,st4,st5,st6,st7,st8,st9,st10,st11);
//        qDebug() << "数据解析成功";
}



//收到的数据放入接收框    解析
//void MainWindow::readyRead_Slot()
//{
//    QByteArray receiveData;
//    QTextCodec *tc = QTextCodec::codecForName("GBK");

//    QMQTT::Message msg;

//    receiveData = msg.payload();

//    qDebug() << "获取到数据";

//    if(!receiveData.isEmpty())
//    {
//        QString strBuf = tc->toUnicode(receiveData);

//        //解析到可视化曲线图
//        BackDataParsing(strBuf);
//    }
//    receiveData.clear();

//}



//创建chart
void MainWindow::createChart()
{
    QChart *qchart = new QChart();
    //把chart放到容器里
    ui->graphicsView->setChart(qchart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);//设置抗锯齿

    //创建十条线
    QLineSeries *series0 = new QLineSeries;
    QLineSeries *series1 = new QLineSeries;
    QLineSeries *series2 = new QLineSeries;
    QLineSeries *series3 = new QLineSeries;
    QLineSeries *series4 = new QLineSeries;
    QLineSeries *series5 = new QLineSeries;
    QLineSeries *series6 = new QLineSeries;
    QLineSeries *series7 = new QLineSeries;
    QLineSeries *series8 = new QLineSeries;
    QLineSeries *series9 = new QLineSeries;
    QLineSeries *series10 = new QLineSeries;
    QLineSeries *series11 = new QLineSeries;

    //设置名称
    series0->setName("温度");
    series1->setName("氨气");
    series2->setName("溶解氧");
    series3->setName("深度");
    series4->setName("浑浊度");
    series5->setName("溶解性固体总量");
    series6->setName("水质电导率");
    series7->setName("氨氮含量");
    series8->setName("溶解氧含量");
    series9->setName("透明度");
    series10->setName("PH值");
    series11->setName("无菌保证水平");

    //把线条放到chart里
    qchart->addSeries(series0);
    qchart->addSeries(series1);
    qchart->addSeries(series2);
    qchart->addSeries(series3);
    qchart->addSeries(series4);
    qchart->addSeries(series5);
    qchart->addSeries(series6);
    qchart->addSeries(series7);
    qchart->addSeries(series8);
    qchart->addSeries(series9);
    qchart->addSeries(series10);
    qchart->addSeries(series11);

    //创建x轴坐标
    QDateTimeAxis *axisX = new QDateTimeAxis;

    //格式
    axisX->setFormat("hh:mm:ss");

    //设置竖条数量
    axisX->setTickCount(8);

    //设置坐标名称
    axisX->setTitleText("time(sec)");

    qchart->setAxisX(axisX,series0);
    qchart->setAxisX(axisX,series1);
    qchart->setAxisX(axisX,series2);
    qchart->setAxisX(axisX,series3);
    qchart->setAxisX(axisX,series4);
    qchart->setAxisX(axisX,series5);
    qchart->setAxisX(axisX,series6);
    qchart->setAxisX(axisX,series7);
    qchart->setAxisX(axisX,series8);
    qchart->setAxisX(axisX,series9);
    qchart->setAxisX(axisX,series10);
    qchart->setAxisX(axisX,series11);

    //创建y轴坐标
    QValueAxis *axisY = new QValueAxis;

    //设置取值范围
    axisY->setRange(0,300);

    //设置横条数量
    axisY->setTickCount(11);

    //设置坐标名称
    axisY->setTitleText("content");

    qchart->setAxisY(axisY,series0);
    qchart->setAxisY(axisY,series1);
    qchart->setAxisY(axisY,series2);
    qchart->setAxisY(axisY,series3);
    qchart->setAxisY(axisY,series4);
    qchart->setAxisY(axisY,series5);
    qchart->setAxisY(axisY,series6);
    qchart->setAxisY(axisY,series7);
    qchart->setAxisY(axisY,series8);
    qchart->setAxisY(axisY,series9);
    qchart->setAxisY(axisY,series10);
    qchart->setAxisY(axisY,series11);

    qchart->setDropShadowEnabled(true);

    //初始化坐标
        //设置最小值坐标值 系统时间当前时间
    qchart->axisX()->setMin(QDateTime::currentDateTime().addSecs(0));
        //设置最大值坐标值 系统时间后8*10
    qchart->axisX()->setMax(QDateTime::currentDateTime().addSecs(8*10));
}



//表刷新
void MainWindow::DisplayChart()
{
    //获取当前时间
    QDateTime currentTime = QDateTime::currentDateTime();

    //获取初始化的qchart
    QChart *qchart = (QChart *)ui->graphicsView->chart();

    //获取初始化的series
    QLineSeries *series0 = (QLineSeries *)ui->graphicsView->chart()->series().at(0);
    QLineSeries *series1 = (QLineSeries *)ui->graphicsView->chart()->series().at(1);
    QLineSeries *series2 = (QLineSeries *)ui->graphicsView->chart()->series().at(2);
    QLineSeries *series3 = (QLineSeries *)ui->graphicsView->chart()->series().at(3);
    QLineSeries *series4 = (QLineSeries *)ui->graphicsView->chart()->series().at(4);
    QLineSeries *series5 = (QLineSeries *)ui->graphicsView->chart()->series().at(5);
    QLineSeries *series6 = (QLineSeries *)ui->graphicsView->chart()->series().at(6);
    QLineSeries *series7 = (QLineSeries *)ui->graphicsView->chart()->series().at(7);
    QLineSeries *series8 = (QLineSeries *)ui->graphicsView->chart()->series().at(8);
    QLineSeries *series9 = (QLineSeries *)ui->graphicsView->chart()->series().at(9);
    QLineSeries *series10 = (QLineSeries *)ui->graphicsView->chart()->series().at(10);
    QLineSeries *series11 = (QLineSeries *)ui->graphicsView->chart()->series().at(11);

    series0->append(currentTime.toMSecsSinceEpoch(),Temp_data);
    series1->append(currentTime.toMSecsSinceEpoch(),NH3_data);
    series2->append(currentTime.toMSecsSinceEpoch(),DO_data);
    series3->append(currentTime.toMSecsSinceEpoch(),Dept_data);
    series4->append(currentTime.toMSecsSinceEpoch(),Turb_data);
    series5->append(currentTime.toMSecsSinceEpoch(),TDS_data);
    series6->append(currentTime.toMSecsSinceEpoch(),Cond_data);
    series7->append(currentTime.toMSecsSinceEpoch(),NH3N_data);
    series8->append(currentTime.toMSecsSinceEpoch(),DOL_data);
    series9->append(currentTime.toMSecsSinceEpoch(),SD_data);
    series10->append(currentTime.toMSecsSinceEpoch(),ORP_data);
    series11->append(currentTime.toMSecsSinceEpoch(),SAL_data);

    qchart->axisX()->setMin(QDateTime::currentDateTime().addSecs(-8*10));
    qchart->axisX()->setMax(QDateTime::currentDateTime().addSecs(0));

}



//时间刷新
void MainWindow::ReData_Slot()
{
    DisplayChart();

    ui->time->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}



//发送刷新
void MainWindow::timer_send_Slot()
{
//    if(flag_send==1)
//    {
//        flag_send=0;
//    }
//    QMQTT::Message msg(0, "test", strbuf.toLocal8Bit().data(), 0);
//    m_client->publish(msg);
}



//开关
void MainWindow::on_close_triggered()
{
    static bool ledSw=false;
    ledSw=!ledSw;
    if(ledSw){
        ui->close->setIcon(QIcon(":/open.png"));
        QMQTT::Message msg(0,"qttest","open", 0);
        m_client->publish(msg);
    }
    else{
         ui->close->setIcon(QIcon(":/close.png"));
         QMQTT::Message msg(0,"qttest","close", 0);
         m_client->publish(msg);
    }
}



//连接MQTT服务器
void MainWindow::on_pbConnectToHost_clicked()
{
    if(m_client->isConnectedToHost() == false)
    {
        connect(m_client,&QMQTT::Client::connected,[this](void){ui->pbConnectToHost->setText("断开连接");});
        m_client->setHostName(ui->leHostaddr->text());
        m_client->setPort(ui->lePort->text().toInt());
        m_client->setClientId(ui->leClientId->text());
        m_client->setUsername(ui->leUsername->text());
        m_client->setPassword(QByteArray(ui->lePassword->text().toStdString().c_str()));
        m_client->setVersion(QMQTT::V3_1_1);
        if(ui->leKeepAlive->text().size() != 0) m_client->setKeepAlive(ui->leKeepAlive->text().toInt());
        m_client->cleanSession();
        m_client->connectToHost();
        qDebug() << "连接MQTT服务器成功";

    }else{
        for(int i = 0; i != ui->cbxSubscribedTopic->count();i++)
        {
            m_client->unsubscribe(ui->cbxSubscribedTopic->itemText(i));
        }
        m_client->disconnectFromHost();
        ui->pbConnectToHost->setText("连接");
    }
}



//订阅主题
void MainWindow::on_pcSubscribeTopic_clicked()
{
    if(m_client->isConnectedToHost())
    {
        m_client->subscribe(ui->leSubscribeTopic->text(),ui->cbxQosSubscribe->currentText().toUInt());

        QString topic = ui->leSubscribeTopic->text();

        if(ui->cbxSubscribedTopic->findText(topic) < 0)
            ui->cbxSubscribedTopic->addItem(topic);

        qDebug() << "订阅主题成功";
        qDebug() << m_client->connectionState();
    }
}



//取消订阅
void MainWindow::on_pbUnsubscribeTopic_clicked()
{
    if(m_client->isConnectedToHost())
    {
        m_client->unsubscribe(ui->cbxSubscribedTopic->currentText());
        ui->cbxSubscribedTopic->removeItem(ui->cbxSubscribedTopic->currentIndex());
    }
}



//收到并解析数据
void MainWindow::slot_mqtt_received(const QMQTT::Message &msg)
{
//    ui->teRecvData->append("【主题】：" + msg.topic());
//    ui->teRecvData->append("【数据】：" + QString(msg.payload()));

    QByteArray receiveData;
    QTextCodec *tc = QTextCodec::codecForName("GBK");

    receiveData = msg.payload();
    qDebug() << msg.payload();

    if(!receiveData.isEmpty())
    {
        QString strBuf = tc->toUnicode(receiveData);

        //解析到可视化曲线图
        BackDataParsing(strBuf);
    }
    receiveData.clear();
}



//数据展示
void MainWindow::on_data_triggered()
{
    dbui->show();
}



//设备管理
void MainWindow::on_devices_triggered()
{
    dev->show();
}



//退出程序
void MainWindow::on_exit_triggered()
{
    this->close();
}
