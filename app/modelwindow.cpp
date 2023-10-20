/*
Copyright (C) 2023  Quectel Wireless Solutions Co.,Ltd.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "modelwindow.h"
#include "ui_modelwindow.h"
#include "kernel/mmessagebox.h"
#include <QJsonArray>
#include "kernel/myObjMessServer.h"


modelWindow::modelWindow(int mode,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::modelWindow)
{
    ui->setupUi(this);
    this->userMode = mode;
    uiAttributeSet();
    this->modelprocess = new modelProcess();
    connect(&atHubInstance,SIGNAL(signalAtAck(QString)),this,SLOT(recvAtAck(QString)));
    ui->label_formatShow->setStyleSheet("background-color:rgb(255,235,181);");
    ui->pushButton_properties->setChecked(true);
    ui->tabWidget_model->setCurrentIndex(0);
    ui->label_formatShow->hide();
    ui->comboBox_sendQos->setView(new QListView());
}

modelWindow::~modelWindow()
{
    delete ui;
}

bool modelWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(obj->metaObject()->className() == QStringLiteral("QComboBox") || obj->metaObject()->className() == QStringLiteral("QSpinBox"))
    {
        if (event->type() == QEvent::Wheel)//鼠标滚轮事件
        {
            return true;//禁用下拉框的滚轮改变项的功能
        }
    }
    return false;
}

/**************************************************************************
** 功能	@brief : 设置父对象
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::setParm(void *father)
{
    if(0 == this->userMode)
    {
        this->father = (MainWindow*)father;
    }
//    ((passWindow*)this->passwindow)->setParm(father);
}
/**************************************************************************
** 功能	@brief : 窗口控件属性初始化
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::uiAttributeSet()
{
    ui->spinBox_modelReadLen->setEnabled(true);
    ui->spinBox_timerMs->setRange(1*1000,600*1000);
    ui->spinBox_sendPkgId->setRange(0,65535);
    ui->tableWidget_properties->setShowGrid(false);
    ui->tableWidget_services->setShowGrid(false);
    ui->tableWidget_events->setShowGrid(false);
    ui->tableWidget_properties->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    ui->tableWidget_services->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
    ui->tableWidget_events->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);

    ui->tableWidget_properties->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                        color: rgb(104, 112, 130); \
                                                        font-size:16px; \
                                                        background: transparent; \
                                                        background: rgb(246, 249, 255); \
                                                        padding: 1px;border: none; \
                                                        border-style:solid; \
                                                        border-right-width:0px; \
                                                        border-left-width:0px; \
                                                        }");
    ui->tableWidget_properties->setStyleSheet("QTableWidget{ \
                                    color: rgb(104, 112, 130); \
                                    font-size:14px; \
                                    border-style:solid; \
                                    border-width:1px; \
                                    border-color:rgb(229, 231, 234); \
                                    border-radius:4px; \
                                    padding:2px; \
                                    } \
                                    QTableWidget::item{ \
                                    border-style:solid; \
                                    border-bottom-width:1px; \
                                    border-color:rgb(229, 231, 234); \
                                    } \
                                    QTableWidget::indicator{width: 20px;height:20px; } \
                                    QTableWidget::indicator:checked {image: url(:/png/checkBox1.png);} \
                                    QTableWidget::indicator:unchecked {image: url(:/png/checkBox0.png);} \
                                    QTableWidget::item::selected { \
                                    background: rgba(47, 105, 235, 0.6); \
                                    }");
    ui->tableWidget_services->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              border-right-width:0px; \
                                                              border-left-width:0px; \
                                                              }");
    ui->tableWidget_services->setStyleSheet("QTableWidget{ \
                                          color: rgb(104, 112, 130); \
                                          font-size:14px; \
                                          border-style:solid; \
                                          border-width:1px; \
                                          border-color:rgb(229, 231, 234); \
                                          border-radius:4px; \
                                          padding:2px; \
                                          } \
                                          QTableWidget::item{ \
                                          border-style:solid; \
                                          border-bottom-width:1px; \
                                          border-color:rgb(229, 231, 234); \
                                          } \
                                          QTableWidget::indicator{width: 20px;height:20px; } \
                                          QTableWidget::indicator:checked {image: url(:/png/checkBox1.png);} \
                                          QTableWidget::indicator:unchecked {image: url(:/png/checkBox0.png);} \
                                          QTableWidget::item::selected { \
                                          background: rgba(47, 105, 235, 0.6); \
                                          }");
    ui->tableWidget_events->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                            color: rgb(104, 112, 130); \
                                                            font-size:16px; \
                                                            background: transparent; \
                                                            background: rgb(246, 249, 255); \
                                                            padding: 1px;border: none; \
                                                            border-style:solid; \
                                                            border-right-width:0px; \
                                                            border-left-width:0px; \
                                                            }");
    ui->tableWidget_events->setStyleSheet("QTableWidget{ \
                                        color: rgb(104, 112, 130); \
                                        font-size:14px; \
                                        border-style:solid; \
                                        border-width:1px; \
                                        border-color:rgb(229, 231, 234); \
                                        border-radius:4px; \
                                        padding:2px; \
                                        } \
                                        QTableWidget::item{ \
                                        border-style:solid; \
                                        border-bottom-width:1px; \
                                        border-color:rgb(229, 231, 234); \
                                        } \
                                        QTableWidget::indicator{width: 20px;height:20px; } \
                                        QTableWidget::indicator:checked {image: url(:/png/checkBox1.png);} \
                                        QTableWidget::indicator:unchecked {image: url(:/png/checkBox0.png);} \
                                        QTableWidget::item::selected { \
                                        background: rgba(47, 105, 235, 0.6); \
                                        }");
//      ui->tableWidget_properties->horizontalHeader()->setDefaultSectionSize(50);  //设置默认宽度
//      ui->tableWidget_properties->verticalHeader()->setDefaultSectionSize(60);   //设置一行默认高度
      ui->tableWidget_properties->setColumnWidth(0,30);
      ui->tableWidget_properties->setColumnWidth(1,100);
      ui->tableWidget_properties->setColumnWidth(2,200);
      ui->tableWidget_properties->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
      ui->tableWidget_properties->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
      ui->tableWidget_properties->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);

//      ui->tableWidget_services->horizontalHeader()->setDefaultSectionSize(50);  //设置默认宽度
//      ui->tableWidget_services->verticalHeader()->setDefaultSectionSize(60);   //设置一行默认高度
      ui->tableWidget_services->setColumnWidth(0,30);
      ui->tableWidget_services->setColumnWidth(1,100);
      ui->tableWidget_services->setColumnWidth(2,200);
      ui->tableWidget_services->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
      ui->tableWidget_services->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
      ui->tableWidget_services->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);

//      ui->tableWidget_events->horizontalHeader()->setDefaultSectionSize(50);  //设置默认宽度
//      ui->tableWidget_events->verticalHeader()->setDefaultSectionSize(60);   //设置一行默认高度
      ui->tableWidget_events->setColumnWidth(0,30);
      ui->tableWidget_events->setColumnWidth(1,100);
      ui->tableWidget_events->setColumnWidth(2,200);
      ui->tableWidget_events->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
      ui->tableWidget_events->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
      ui->tableWidget_events->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);

      //QComboBox，QSpinBox注册鼠标滑轮事件
      ui->spinBox_timerMs->installEventFilter(this);
      ui->comboBox_sendQos->installEventFilter(this);
      ui->spinBox_sendPkgId->installEventFilter(this);
      ui->spinBox_modelReadLen->installEventFilter(this);
}

/**************************************************************************
** 功能	@brief : 重置窗口状态
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::resetUIValue()
{
    ui->checkBox_timer->setChecked(false);
    ui->spinBox_timerMs->setValue(1000);
    ui->checkBox_sendAckAuto->setChecked(false);
    ui->comboBox_sendQos->setCurrentIndex(0);
    ui->spinBox_sendPkgId->setValue(0);
    ui->label_modelReadByte->setText(QString::number(0));
    ui->label_modelReadCount->setText(QString::number(0));
}

int modelWindow::getReadCurrentCount()
{
    return ui->spinBox_modelReadLen->value();
}

/**************************************************************************
** 功能	@brief : 定时上报选定的物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::reportModelTimeout()
{
    qInfo()<<__FUNCTION__;
    QByteArray sendData;
    bool mode ;
    bool sendMode;
    if(0 == this->userMode)
    {
        mode = this->father->userconfigStatus(GETUSERINFO,SENDMMODELMODE);
        sendMode =this->father->userconfigStatus(GETUSERINFO,SOCKETSTATUS);
    }
    else if(1 == this->userMode)
    {
        myObjMessServer::getObjMessServer()->notify("mainwindow_userconfigStatus",Q_RETURN_ARG(bool,mode),Q_ARG(int,GETUSERINFO),Q_ARG(int,SENDMMODELMODE),Q_ARG(bool,false));
        myObjMessServer::getObjMessServer()->notify("mainwindow_userconfigStatus",Q_RETURN_ARG(bool,sendMode),Q_ARG(int,GETUSERINFO),Q_ARG(int,SOCKETSTATUS),Q_ARG(bool,false));
    }
    if(sendMode != true)
    {
        on_checkBox_timer_toggled(false);
    }
    switch (ui->tabWidget_model->currentIndex())
    {
    case 0:
        sendData = this->modelprocess->sendModelData(ui->tableWidget_properties,mode);
        break;
    case 1:
        sendData = this->modelprocess->sendModelData(ui->tableWidget_services,mode);
        break;
    case 2:
        sendData = this->modelprocess->sendModelData(ui->tableWidget_events,mode);
        break;
    default:
        break;
    }

//    if(0 == ui->pushButton_sendTtlv->text().compare(tr("直接发送")))
//    {
//        sendDataToQuec(sendData,false);
//    }
//    else
    if(0 == ui->pushButton_sendTtlv->text().compare(tr("发送数据")))
    {
        sendDataToQuec(sendData,false);
    }
}

/**************************************************************************
** 功能	@brief : 物模型列表更新
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::modelTableUpdate(QString filePath)
{
    qInfo()<<__FUNCTION__;
    ui->tableWidget_properties->setRowCount(0);
    ui->tableWidget_services->setRowCount(0);
    ui->tableWidget_events->setRowCount(0);
    if(filePath.isEmpty())
       return;
    jsonFile *jsonfile = new jsonFile(filePath);
    jsonfile->readFile();
    QJsonValue profile;
    QJsonValue properties;
    if(jsonfile->readJsonKeyValue("properties",&properties))
    {
        this->modelprocess->updateModelPointList(properties.toArray(),ui->tableWidget_properties);
    }
    QJsonValue services;
    if(jsonfile->readJsonKeyValue("services",&services))
    {
        this->modelprocess->updateModelPointList(services.toArray(),ui->tableWidget_services);
    }
    QJsonValue events;
    if(jsonfile->readJsonKeyValue("events",&events))
    {
        this->modelprocess->updateModelPointList(events.toArray(),ui->tableWidget_events);
    }
    //delete(jsonfile);
}
/**************************************************************************
** 功能	@brief : 数据发送中转
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::sendDataToQuec(QByteArray data,bool externFlag)
{
    if(!data.isEmpty())
    {
        // this->father->ATCMDSend("AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(sendData.length()),sendData);
        if(0 == this->userMode)
        {
            if(externFlag)
            {
                myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QByteArray,QString("AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(data.length())+",\"").toUtf8()+data+"\""),Q_ARG(bool,true));
            }
            else
            {
                myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QString,"AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(data.length())),Q_ARG(QByteArray,data));
            }
            //this->father->ATCMDSend("AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(data.length()),data);
        }
        else if(1 == this->userMode)
        {
            if(externFlag)
            {
                myObjMessServer::getObjMessServer()->notify("subdevwindow_ATSendModuleData",Q_ARG(QByteArray,data),Q_ARG(bool,externFlag));
            }
            else
            {
                myObjMessServer::getObjMessServer()->notify("subdevwindow_ATSendModuleData",Q_ARG(QByteArray,data));
            }
        }
    }
}
/**************************************************************************
** 功能	@brief : 数据发送中转+PKGID
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::sendDataToQuec_Pkgid(QByteArray data,bool externFlag)
{
    qDebug()<<__FUNCTION__<<"data:"<<data<<",userMode:"<<userMode<<",externFlag:"<<externFlag;
    if(!data.isEmpty())
    {
        // this->father->ATCMDSend("AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(sendData.length()),sendData);
        if(0 == this->userMode)
        {
            if(externFlag)
            {
                myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QByteArray,QString("AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(data.length())+","+ui->spinBox_sendPkgId->text()+",\"").toUtf8()+data+"\""),Q_ARG(bool,true));
            }
            else
            {
                qDebug()<<"AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(data.length())+","+ui->spinBox_sendPkgId->text();
                myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QString,"AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(data.length())+","+ui->spinBox_sendPkgId->text()),Q_ARG(QByteArray,data));
            }
            //this->father->ATCMDSend("AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(data.length()),data);
        }
        else if(1 == this->userMode)
        {
            if(externFlag)
            {
                myObjMessServer::getObjMessServer()->notify("subdevwindow_ATSendModuleData",Q_ARG(QByteArray,data),Q_ARG(bool,externFlag),Q_ARG(int,ui->spinBox_sendPkgId->value()));
            }
            else
            {
                myObjMessServer::getObjMessServer()->notify("subdevwindow_ATSendModuleData",Q_ARG(QByteArray,data),Q_ARG(int,ui->spinBox_sendPkgId->value()));
            }
        }
    }
}
/**************************************************************************
* @brief        at指令返回值处理
* @param [in]   cmdData 发送的at指令内容
* @param [out]
* @retval
***************************************************************************/
void modelWindow::recvAtAck(QString cmdData)
{
    qDebug()<<__FUNCTION__<<__LINE__<<cmdData;
    if(cmdData.contains("QIOTCFG=\"tsl\""))
    {
        if(cmdData.contains("1"))
        {
            ui->label_formatShow->setText(tr("APP当前为%1类型数据收发模式").arg("JSON"));
        }
        else
        {
            ui->label_formatShow->setText(tr("APP当前为%1类型数据收发模式").arg("TTLV"));
        }
    }
}

/**************************************************************************
** 功能	@brief : 发送物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::on_pushButton_sendTtlv_clicked()
{
    qInfo()<<__FUNCTION__;
    QByteArray sendData;
//    int mode = this->father->userconfigStatus(GETUSERINFO,SENDMMODELMODE);
    bool mode = false;
    myObjMessServer::getObjMessServer()->notify("mainwindow_userconfigStatus",Q_RETURN_ARG(bool,mode),Q_ARG(int,GETUSERINFO),Q_ARG(int,SENDMMODELMODE),Q_ARG(bool,false));
    switch (ui->tabWidget_model->currentIndex())
    {
    case 0:
        if(ui->tableWidget_properties->currentRow() == -1)
        {
            MMessageBox::warning(this,tr("警告"),tr("请先选择上方表格栏中需要发送的物模型"),tr("确认"));
            return;
        }
        sendData = this->modelprocess->sendModelData(ui->tableWidget_properties,mode);
        break;
    case 1:
        if(ui->tableWidget_services->currentRow() == -1)
        {
            MMessageBox::warning(this,tr("警告"),tr("请先选择上方表格栏中需要发送的物模型"),tr("确认"));
            return;
        }
        sendData = this->modelprocess->sendModelData(ui->tableWidget_services,mode);
        break;
    case 2:
        if(ui->tableWidget_events->currentRow() == -1)
        {
            MMessageBox::warning(this,tr("警告"),tr("请先选择上方表格栏中需要发送的物模型"),tr("确认"));
            return;
        }
        sendData = this->modelprocess->sendModelData(ui->tableWidget_events,mode);
        break;
    default:
        break;
    }
//    if(0 == ui->pushButton_sendTtlv->text().compare(QString("直接发送")))
//    {
//        sendDataToQuec(sendData,false);
//    }
//    else
    if(0 == ui->pushButton_sendTtlv->text().compare(tr("发送数据")))
    {
        sendDataToQuec(sendData,false);
    }
}
/**************************************************************************
** 功能	@brief : 发送物模型数据应答
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::on_pushButton_sendAck_clicked()
{
    qInfo()<<__FUNCTION__;
    QByteArray sendData;
//    bool mode = this->father->userconfigStatus(GETUSERINFO,SENDMMODELMODE);
    bool mode = false;
    myObjMessServer::getObjMessServer()->notify("mainwindow_userconfigStatus",Q_RETURN_ARG(bool,mode),Q_ARG(int,GETUSERINFO),Q_ARG(int,SENDMMODELMODE),Q_ARG(bool,false));
    switch (ui->tabWidget_model->currentIndex())
    {
    case 0:
        if(ui->tableWidget_properties->currentRow() == -1)
        {
            MMessageBox::information(this,tr("提示"),tr("请先选择需要发送的物模型"),tr("确认"));
            return;
        }
        sendData = this->modelprocess->readModelData(QList<QByteArray>(),ui->tableWidget_properties,mode);
        break;
    case 1:
        if(ui->tableWidget_services->currentRow() == -1)
        {
            MMessageBox::information(this,tr("提示"),tr("请先选择需要发送的物模型"),tr("确认"));
            return;
        }
        sendData = this->modelprocess->readModelData(QList<QByteArray>(),ui->tableWidget_services,mode);
        break;
    case 2:
        if(ui->tableWidget_events->currentRow() == -1)
        {
            MMessageBox::information(this,tr("提示"),tr("请先选择需要发送的物模型"),tr("确认"));
            return;
        }
        sendData = this->modelprocess->readModelData(QList<QByteArray>(),ui->tableWidget_events,mode);
        break;
    default:
        break;
    }
//    if(!sendData.isEmpty())
//        this->father->ATCMDSend("AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(sendData.length())+","+ui->spinBox_sendPkgId->text(),sendData);
//    if(0 == ui->pushButton_sendTtlv->text().compare(QString("直接发送")))
//    {
//        sendDataToQuec_Pkgid(sendData,false);
//    }
//    else
    if(0 == ui->pushButton_sendTtlv->text().compare(tr("发送数据")))
    {
        sendDataToQuec_Pkgid(sendData,false);
    }
}
/**************************************************************************
** 功能	@brief : 读取缓存的物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::on_pushButton_readTtlv_clicked()
{
    qInfo()<<__FUNCTION__;
    if(0 == userMode)
    {
        QString sendData;
        int value = ui->spinBox_modelReadLen->value();
        if(value == 0)
        {
            value = 512;
        }
        sendData.append("AT+QIOTMODELRD="+QString::number(value));
        this->father->ATCMDSend(sendData);
    }
    else if(1 == userMode)
    {
        myObjMessServer::getObjMessServer()->notify("subdevwindow_ATRecvModuleData");
    }
}
/**************************************************************************
** 功能	@brief : 读取缓存的物模型数据最大值
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::on_pushButton_readMaxTtlv_clicked()
{
    qInfo()<<__FUNCTION__;
    if(0 == userMode)
    {
        QString sendData;
        sendData.append("AT+QIOTMODELRD=?");
        this->father->ATCMDSend(sendData);
    }
    else if(1 == userMode)
    {
        myObjMessServer::getObjMessServer()->notify("subdevwindow_ATRecvModuleData",Q_ARG(int,0));
    }
}

/**************************************************************************
** 功能	@brief : 读取缓存的物模型数据后的返回结果显示
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::csdkModelIssue(QByteArray data)
{
    qInfo()<<__FUNCTION__;
    if(data.isNull())
    {
        ui->label_modelReadCount->setText(QString::number(ui->label_modelReadCount->text().toInt()+1));
    }
    else
    {
//        bool mode = this->father->userconfigStatus(GETUSERINFO,SENDMMODELMODE);
        bool mode = false;
        myObjMessServer::getObjMessServer()->notify("mainwindow_userconfigStatus",Q_RETURN_ARG(bool,mode),Q_ARG(int,GETUSERINFO),Q_ARG(int,SENDMMODELMODE),Q_ARG(bool,false));
        if(-1 == this->modelprocess->recvModelData(data,ui->tableWidget_properties,mode))
        {
            if(-1 != this->modelprocess->recvModelData(data,ui->tableWidget_services,mode))
            {
                this->modelprocess->setServerFlag(true);
                QByteArray sendData = this->modelprocess->readModelData(QList<QByteArray>()<<this->modelprocess->extractServerId(data,mode),ui->tableWidget_services,mode);
    //            if(0 == ui->pushButton_sendTtlv->text().compare(QString("直接发送")))
    //            {
    //                sendDataToQuec(sendData,false);
    //            }
    //            else
                if(0 == ui->pushButton_sendTtlv->text().compare(tr("发送数据")) && ui->checkBox_sendAckAuto->isChecked())
                {
                    sendDataToQuec(sendData,false);
                }
            }
            else
            {
                MMessageBox::critical(this,tr("错误"),tr("无法匹配数据"),tr("确认"));
            }
        }
    }
}
/**************************************************************************
** 功能	@brief : 读取缓存的物模型数据后的返回结果处理
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::csdkModelRead(QString readByte,QString readCount,QByteArray data)
{
    qInfo()<<__FUNCTION__;
    ui->label_modelReadByte->setText(readByte);
    ui->label_modelReadCount->setText(readCount);
    if(!data.isNull())
    {
        dataCache.append(data);
        if(readByte.toInt() !=0) //|| readCount.toInt() != 0
        {
            if(0 == userMode)
            {
                int value = ui->spinBox_modelReadLen->value();
                this->father->ATCMDSend("AT+QIOTMODELRD="+QString::number(value));
            }
            else if(1 == userMode)
            {
                myObjMessServer::getObjMessServer()->notify("subdevwindow_ATRecvModuleData");
            }
            return ;
        }
        else
        {
//    //        this->modelprocess->recvModelData(data,ui->tableWidget_properties);
////            bool mode = this->father->userconfigStatus(GETUSERINFO,SENDMMODELMODE);
           bool mode = false;
           myObjMessServer::getObjMessServer()->notify("mainwindow_userconfigStatus",Q_RETURN_ARG(bool,mode),Q_ARG(int,GETUSERINFO),Q_ARG(int,SENDMMODELMODE),Q_ARG(bool,false));
           if(-1 == this->modelprocess->recvModelData(dataCache,ui->tableWidget_properties,mode))
           {
               if(-1 != this->modelprocess->recvModelData(data,ui->tableWidget_services,mode))
               {
                   QByteArray sendData = this->modelprocess->readModelData(QList<QByteArray>()<<this->modelprocess->extractServerId(dataCache,mode),ui->tableWidget_services,mode);
   //                if(0 == ui->pushButton_sendTtlv->text().compare(QString("直接发送")))
   //                {
   //                    sendDataToQuec(sendData,false);
   //                }
   //                else
                   if(0 == ui->pushButton_sendTtlv->text().compare(tr("发送数据")) && ui->checkBox_sendAckAuto->isChecked())
                   {
                       sendDataToQuec(sendData,false);
                   }
               }
               else
               {
                   MMessageBox::critical(this,tr("错误"),tr("无法匹配数据"),tr("确认"));
               }
           }
            dataCache.clear();
        }
    }
}
/**************************************************************************
** 功能	@brief : 获取并发送物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::csdkModelGet(int pkgId,QList<QByteArray> idList)
{
    qInfo()<<__FUNCTION__;
    ui->spinBox_sendPkgId->setValue(pkgId);
    if(ui->checkBox_sendAckAuto->isChecked())
    {
        QByteArray sendData;
//        bool mode = this->father->userconfigStatus(GETUSERINFO,SENDMMODELMODE);
        bool mode = false;
        myObjMessServer::getObjMessServer()->notify("mainwindow_userconfigStatus",Q_RETURN_ARG(bool,mode),Q_ARG(int,GETUSERINFO),Q_ARG(int,SENDMMODELMODE),Q_ARG(bool,false));
        sendData.append(this->modelprocess->readModelData(idList,ui->tableWidget_properties,mode));
        sendData.append(this->modelprocess->readModelData(idList,ui->tableWidget_services,mode));
        sendData.append(this->modelprocess->readModelData(idList,ui->tableWidget_events,mode));
//        if(!sendData.isEmpty())
//            this->father->ATCMDSend("AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(sendData.length())+","+QString::number(pkgId),sendData);
        if(!sendData.isEmpty())
        {
            if(0 == this->userMode)
            {
                myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QString,"AT+QIOTMODELTD="+ui->comboBox_sendQos->currentText()+","+QString::number(sendData.length())+","+QString::number(pkgId)),Q_ARG(QByteArray,sendData));
            }
            else if(1 == this->userMode)
            {
                myObjMessServer::getObjMessServer()->notify("subdevwindow_ATSendModuleData",Q_ARG(QByteArray,sendData),Q_ARG(int,pkgId));
            }
        }
    }
//    else
//    {
//        this->modelprocess->readModelId(idList,ui->tableWidget_properties);
    //    }
}

/**************************************************************************
** 功能	@brief : 开启定时器：自动发送物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::on_checkBox_timer_toggled(bool checked)
{
    qInfo()<<__FUNCTION__;
    if(checked == false)
    {
        this->reportTimer.stop();
        disconnect(&this->reportTimer, SIGNAL(timeout()), this, SLOT(reportModelTimeout()));
        ui->checkBox_timer->setText(tr("关闭"));
        ui->spinBox_timerMs->setEnabled(true);
    }
    else
    {
        ui->checkBox_timer->setText(tr("关闭"));
        bool mode = false;
        myObjMessServer::getObjMessServer()->notify("mainwindow_userconfigStatus",Q_RETURN_ARG(bool,mode),Q_ARG(int,GETUSERINFO),Q_ARG(int,SOCKETSTATUS),Q_ARG(bool,false));
        if((!(mode)) && checked)
        {
            MMessageBox::information(this,tr("警告"),tr("请先打开tcp或串口"),tr("确认"));
            ui->checkBox_timer->setChecked(false);
            ui->checkBox_timer->setText(QString("开启"));
            return;
        }
        int currentRow = -1;
        switch (ui->tabWidget_model->currentIndex())
        {
        case 0:
            currentRow = ui->tableWidget_properties->currentRow();
            break;
        case 1:
            currentRow = ui->tableWidget_services->currentRow();
            break;
        case 2:
            currentRow = ui->tableWidget_events->currentRow();
            break;
        default:
            break;
        }
        if(currentRow < 0)
        {
            MMessageBox::information(this,tr("警告"),tr("请先选择需要发送的物模型"),tr("确认"));
            ui->checkBox_timer->setChecked(false);
            ui->checkBox_timer->setText(tr("开启"));
            return;
        }
        this->reportTimer.start(ui->spinBox_timerMs->value());
        connect(&this->reportTimer, SIGNAL(timeout()), this, SLOT(reportModelTimeout()));
        ui->checkBox_timer->setText(tr("开启"));
        ui->spinBox_timerMs->setEnabled(false);
    }
}

/**************************************************************************
** 功能	@brief : 自动发送的时间修改
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::on_spinBox_timerMs_valueChanged(int arg1)
{
    if(ui->checkBox_timer->isChecked())
    {
        this->reportTimer.start(arg1);
    }
}
/**************************************************************************
** 功能	@brief : 右键菜单显示
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::listWidgetShow(QStringList data, QRect showPoint)
{
    int listWidget_widget = data.size()*showPoint.height();
    if(NULL == listWidget)
    {
        listWidget = new newListWiget();
    }
    else if(NULL != listWidget)
    {
        delete listWidget;
        listWidget = NULL;
        listWidget = new newListWiget();
    }
    for(int i = 0;i<data.size();i++)
    {

        listWidget->addItem(data[i]);
    }
    listWidget->setWindowFlags(Qt::FramelessWindowHint );
    listWidget->setAttribute(Qt::WA_TranslucentBackground);
    listWidget->setAttribute(Qt::WA_QuitOnClose,false);
    listWidget->setGeometry(showPoint.x(),showPoint.y(),showPoint.width()+10,listWidget_widget);
    connect(listWidget,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(itemClickedHandle(QListWidgetItem*)));
    listWidget->setWindowModality(Qt::ApplicationModal);
    listWidget->show();
}
/**************************************************************************
** 功能	@brief : 右键菜单关闭
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::listWidgetCheckClose()
{
    if(listWidget != NULL)
    {
        listWidget->close();
        delete listWidget;
        listWidget = NULL;
    }
}
/**************************************************************************
** 功能	@brief : 设置物模型读取的最大长度
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::setModelReadMaxLen(int len)
{
    ui->spinBox_modelReadLen->setValue(len);
}
/**************************************************************************
** 功能	@brief : 通过label显示当前数据收发模式
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::setLabelShowTls(bool mode)
{
    if(mode)
    {
        ui->label_formatShow->setText(tr("提示：APP当前为%1类型数据收发模式").arg("TTLV"));
    }
    else
    {
        ui->label_formatShow->setText(tr("提示：APP当前为%1类型数据收发模式").arg("JSON"));
    }
}
/**************************************************************************
** 功能	@brief : 获取当前数据收发模式
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool modelWindow::getTlsStatus()
{
    if(ui->label_formatShow->text().contains("TTLV"))
    {
        return false;
    }
    return true;
}
/**************************************************************************
** 功能	@brief : 翻译转换
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool modelWindow::retranslationUi()
{
    ui->retranslateUi(this);
    return true;
}
/**************************************************************************
** 功能	@brief : 发送 接收物模型的AT指令
** 输入	@param :
** 输出	@retval:
***************************************************************************/
//void modelWindow::on_pushButton_recvModeSelect_clicked()
//{
//    QStringList list;
//    list<<tr("读取物模型数据")<<tr("获取读取的最大值");
//    qDebug()<<ui->pushButton_readTtlv->size();
//    QSize size = ui->pushButton_readTtlv->size();
//    if(size.height() < 15)
//    {
//        size.setHeight(15);
//    }
//    int oneFontWidth = size.width()/ui->pushButton_readTtlv->text().size();
//    int maxWidth = 0;
//    for(int i =0;i<list.size();i++)
//    {
//        int fontWidth = oneFontWidth * QString(list[i]).size();
//        if(maxWidth < fontWidth)
//        {
//            maxWidth = fontWidth;
//        }
//    }
//    size.setWidth(maxWidth);
//    QPoint point;
//    point.setX(ui->pushButton_recvModeSelect->mapToGlobal(QPoint(0, 0)).x());
//    point.setY(ui->pushButton_recvModeSelect->mapToGlobal(QPoint(0, 0)).y()+ui->pushButton_recvModeSelect->height()-1);
//    QRect rect(point,size);
//    listWidgetShow(list,rect);
//}
/**************************************************************************
** 功能	@brief : 读取物模型数据 或 读取物模型数据的最大值
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::itemClickedHandle(QListWidgetItem *item)
{
//    if(0 == item->text().compare(tr("直接发送")) || 0 == item->text().compare(tr("透传发送")))
/*    if(0 == item->text().compare(tr("发送数据")))
    {
        ui->pushButton_sendTtlv->setText(item->text());
    }
    else */if(0 == item->text().compare(tr("读取物模型数据")) || 0 == item->text().compare(tr("获取读取的最大值")))
    {
        ui->pushButton_readTtlv->setText(item->text());
    }
    listWidget->close();
    listWidget = NULL;
}

//void modelWindow::on_pushButton_sendModeSelect_clicked()
//{
//    QStringList list;
//    list<<QString("直接发送")<<QString("透传发送");
//    QPoint point;
//    point.setX(QWidget::mapToGlobal(ui->pushButton_sendModeSelect->pos()).x()+10);
//    point.setY(QWidget::mapToGlobal(ui->pushButton_sendModeSelect->pos()).y()+ui->pushButton_sendModeSelect->height()+ui->groupBox_send->pos().y());
//    qDebug()<<"point:"<<point;
//    qDebug()<<ui->pushButton_sendTtlv->size();
//    QRect rect(point,ui->pushButton_sendTtlv->size());
//    listWidgetShow(list,rect);
//}
/**************************************************************************
** 功能	@brief : 鼠标事件：根据数据位置决定菜单是否需要关闭
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if(this->listWidget != NULL && this->listWidget->isActiveWindow())
    {
        if (listWidget->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        {
            listWidget->close();
            listWidget = NULL;
        }
    }
}
/**************************************************************************
** 功能	@brief : 窗口事件：如果鼠标离开菜单窗口，则关闭
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelWindow::leaveEvent(QEvent *e)
{
    Q_UNUSED(e);
    if(listWidget != NULL && !(this->geometry().contains(this->mapFromGlobal(QCursor::pos()))))
    {
        if(!listWidget->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        {
            listWidget->close();
            delete listWidget;
            listWidget = NULL;
        }
    }
}

void modelWindow::on_pushButton_properties_clicked()
{
    ui->tabWidget_model->setCurrentIndex(0);
}

void modelWindow::on_pushButton_services_clicked()
{
    ui->tabWidget_model->setCurrentIndex(1);
}

void modelWindow::on_pushButton_3_clicked()
{
    ui->tabWidget_model->setCurrentIndex(2);
}
