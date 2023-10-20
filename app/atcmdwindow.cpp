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
#include "atcmdwindow.h"
#include "ui_atcmdwindow.h"
#include <QStandardPaths>
#include <QInputDialog>
#include "baseQTConfigure.h"
#include "userconfigure.h"
#include "modelwindow.h"
#include "kernel/mmessagebox.h"

bool isFirstTime = true;

ATcmdWindow::ATcmdWindow(int mode,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ATcmdWindow)
{
    ui->setupUi(this);
    this->userMode = mode;
    ui->comboBox_buffer->setView(new QListView());
    ui->comboBox_format->setView(new QListView());
    ui->comboBox_gwMode->setView(new QListView());
    ui->comboBox_server->setView(new QListView());
    ui->comboBox_protocol->setView(new QListView());
    ui->comboBox_txidMode->setView(new QListView());
    ui->comboBox_sessionMode->setView(new QListView());
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->lineEdit_dk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
    ui->lineEdit_ds->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
    ui->lineEdit_psk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
    connect(&atHubInstance,SIGNAL(signalAtAck(QString)),this,SLOT(handleAtAck(QString)));
    //QComboBox，QSpinBox注册鼠标滑轮事件
    QList<QComboBox *> comboBoxList = this->findChildren<QComboBox *>();
    for (int i = 0; i < comboBoxList.count(); i ++)
    {
        comboBoxList.at(i)->installEventFilter(this);
    }
    QList<QSpinBox *> spinBoxList = this->findChildren<QSpinBox *>();
    for (int i = 0; i < spinBoxList.count(); i ++)
    {
        spinBoxList.at(i)->installEventFilter(this);
    }
    //V3.0.0隐藏鉴权快捷AT
    ui->label_gwMode->hide();
    ui->label_18->hide();
    ui->comboBox_gwMode->hide();
    ui->lineEdit_gwMode->hide();
    ui->pushButton_gwMode->hide();
    ui->pushButton_gwMode_get->hide();
}

ATcmdWindow::~ATcmdWindow()
{
    delete ui;
}
/**************************************************************************
** 功能	@brief :  过滤QComboBox、QSpinBox控件的滚轮信号
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool ATcmdWindow::eventFilter(QObject *obj, QEvent *event)
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

bool ATcmdWindow::retranslationUi()
{
    ui->retranslateUi(this);
    return true;
}
/**************************************************************************
** 功能	@brief :  获取tsl值（对外接口）
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::getTslValue()
{
    on_pushButton_format_get_clicked();
}
/**************************************************************************
** 功能	@brief : 设置父对象
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::setParm(void *father)
{
    this->father = (MainWindow*)father;
    this->father->userconfigStatus(SETUSERINFO,SENDMMODELMODE,(ui->comboBox_format->currentIndex() == 0)? TTLVMODESNED:JSONMODESEND);
    isFirstTime = true;
    QStringList readSeverList = this->father->configprocess->historyRead("serverList");
    ui->comboBox_server->clear();
    this->currentConfigInfo.serverList.clear();
    currentConfigInfo.serverList.append(tr("生产环境(http)"));
    currentConfigInfo.serverList.append(tr("生产环境(mqtt)"));
    currentConfigInfo.serverList.append(tr("生产环境(coap)"));
    for(int count = 0; count < readSeverList.count(); count ++)
    {
        this->currentConfigInfo.serverList.append(readSeverList[count]);
    }
    currentConfigInfo.serverList.append(tr("自定义"));
    ui->comboBox_server->addItems(currentConfigInfo.serverList);
    ui->comboBox_protocol->setCurrentIndex(1);
    on_comboBox_protocol_activated("MQTT");
}
/**************************************************************************
** 功能	@brief :  产品server设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_server_clicked()
{
    qInfo()<<__FUNCTION__<<ui->comboBox_server->currentText();
    bool flag = ui->comboBox_protocol->currentIndex() == 1 ? true : false;

    QString server;
    if("开发环境" == ui->comboBox_server->currentText())
    {
        server = (flag == true ? "192.168.25.122:1884" :NULL);
    }
    else if("fat2环境" == ui->comboBox_server->currentText())
    {
        server = (flag == true ?"220.180.239.212:8382":"220.180.239.212:8416");
    }
    else if("fatb环境" == ui->comboBox_server->currentText())
    {
        server = (flag == true ?"220.180.239.212:8462":"220.180.239.212:8021");
    }
    else if(tr("生产环境(http)") == ui->comboBox_server->currentText())
    {
        server = "iot-south.quectel.com:2883";
    }
    else if(tr("生产环境(mqtt)") == ui->comboBox_server->currentText())
    {
        server = "iot-south.quectelcn.com:1883";
    }
    else if(tr("生产环境(coap)") == ui->comboBox_server->currentText())
    {
        server = "iot-south.quectelcn.com:5683";
    }
    else
    {
        server = ui->comboBox_server->currentText();
    }
    this->father->ATCMDSend("AT+QIOTCFG=\"server\","+QString::number(ui->comboBox_protocol->currentIndex())+",\""+server+"\"");
}
/**************************************************************************
** 功能	@brief :  产品缓存类型设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_buffer_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"buffer\","+QString::number(ui->comboBox_buffer->currentIndex()));
}
/**************************************************************************
** 功能	@brief :  产品连接心跳设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_lifetime_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"lifetime\","+QString::number(ui->spinBox_lifetime->value()));
}
/**************************************************************************
** 功能	@brief :  产品连接id设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_contextId_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"act\","+QString::number(ui->spinBox_contextId->value()));
}
/**************************************************************************
** 功能	@brief :  产品数据传输格式设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_format_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"tsl\","+QString::number(ui->comboBox_format->currentIndex()));
    this->father->userconfigStatus(SETUSERINFO,SENDMMODELMODE,ui->comboBox_format->currentIndex());
}
/**************************************************************************
** 功能	@brief :  产品传输加密设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_sessionMode_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"session_mode\","+QString::number(ui->comboBox_sessionMode->currentIndex()));
}

/**************************************************************************
** 功能	@brief :  产品server选择
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_comboBox_server_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    qInfo()<<__FUNCTION__;
    if(ui->comboBox_server->currentText() == tr("自定义"))
    {
        ui->comboBox_server->setEditable(true);
        ui->comboBox_server->clearEditText();
    }
    else
    {
        ui->comboBox_server->setEditable(false);
    }
    if((ui->comboBox_server->itemText((ui->comboBox_server->count()-1) > 0 ? (ui->comboBox_server->count()-1):0) != tr("自定义")) && !isFirstTime)
    {
        isFirstTime = false;
        disconnect(ui->comboBox_server,SIGNAL(currentIndexChanged(int)),this,SLOT(on_comboBox_server_currentIndexChanged(int)));
        // 保存选项，重刷
        currentConfigInfo.serverList.clear();
        QStringList saveList;
        for(int count = 0; count < ui->comboBox_server->count(); count ++)
        {
            if(0 != ui->comboBox_server->itemText(count).compare(tr("自定义")))
            {
                this->currentConfigInfo.serverList.append(ui->comboBox_server->itemText(count));
            }
            if((0 != ui->comboBox_server->itemText(count).compare(tr("自定义"))) ||
                    0 != ui->comboBox_server->itemText(count).compare(tr("生产环境(http)")) ||
                    0 != ui->comboBox_server->itemText(count).compare(tr("生产环境(mqtt)")) ||
                    0 != ui->comboBox_server->itemText(count).compare(tr("生产环境(coap)")))
            {
                saveList.append(ui->comboBox_server->itemText(count)); //只保存用户自定义服务器
            }
        }
        this->currentConfigInfo.serverList.append(tr("自定义"));
        qDebug()<<"currentConfigInfo.serverList:"<<currentConfigInfo.serverList;
        ui->comboBox_server->clear();
        ui->comboBox_server->addItems(currentConfigInfo.serverList);
        ui->comboBox_server->setCurrentIndex(currentConfigInfo.serverList.count()-2);
        this->father->configprocess->historySave("serverList",saveList);
        connect(ui->comboBox_server,SIGNAL(currentIndexChanged(int)),this,SLOT(on_comboBox_server_currentIndexChanged(int)));
    }
}
/**************************************************************************
** 功能	@brief :  设置DKDS
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_dkds_clicked()
{
    QString dk_data = ui->lineEdit_dk->text();
    QString ds_data = ui->lineEdit_ds->text();
    QString data = "AT+QIOTCFG=\"dk_ds\"";
    if(!dk_data.isEmpty())
    {
        data.append(",\""+dk_data+"\"");
    }
    else
    {
        data.append(",\"\"");
    }
    if(!ds_data.isEmpty())
    {
        data.append(",\""+ds_data+"\"");
    }
//    else
//    {
//        data.append(",\"\"");
//    }
    this->father->ATCMDSend(data);
//    CurrentConfigInfoChange();
}
/**************************************************************************
** 功能	@brief :  设置PSK
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_psk_clicked()
{
    this->father->ATCMDSend("AT+QIOTCFG=\"psk\",\""+ui->lineEdit_psk->text()+"\"");
}
/**************************************************************************
** 功能	@brief :  设置txidMode
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_txidMode_clicked()
{
    this->father->ATCMDSend("AT+QIOTCFG=\"txid_mode\","+QString::number(ui->comboBox_txidMode->currentIndex()));
}
/**************************************************************************
** 功能	@brief :  设置gwMode
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_gwMode_clicked()
{
    if(0 == ui->comboBox_gwMode->currentIndex())
    {
        this->father->ATCMDSend("AT+QIOTCFG=\"gw_mode\","+QString::number(ui->comboBox_gwMode->currentIndex()));
    }
    else
    {
        if(!ui->lineEdit_gwMode->text().isEmpty())
        {
            this->father->ATCMDSend("AT+QIOTCFG=\"gw_mode\","+QString::number(ui->comboBox_gwMode->currentIndex())+",\""+ui->lineEdit_gwMode->text()+"\"");
        }
        else
        {
            this->father->statusBarShow(tr("鉴权信息不能为空"));
        }
    }
}
/**************************************************************************
** 功能	@brief :  产品server获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_server_get_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"server\"");
}
/**************************************************************************
** 功能	@brief :  产品当前缓存方式获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_buffer_get_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"buffer\"");
}
/**************************************************************************
** 功能	@brief :  产品当前连接心跳值获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_lifetime_get_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"lifetime\"");
}
/**************************************************************************
** 功能	@brief :  产品当前连接id获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_contextId_get_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"act\"");
}
/**************************************************************************
** 功能	@brief :  产品当前传输格式获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_format_get_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"tsl\"");
}
/**************************************************************************
** 功能	@brief :  产品当前传输加密形式获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_sessionMode_get_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"session_mode\"");
}
/**************************************************************************
** 功能	@brief :  获取DKDS
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_dkds_get_clicked()
{
    this->father->ATCMDSend("AT+QIOTCFG=\"dk_ds\"");
}
/**************************************************************************
** 功能	@brief :  获取PSK
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_psk_get_clicked()
{
    this->father->ATCMDSend("AT+QIOTCFG=\"psk\"");
}
/**************************************************************************
** 功能	@brief :  获取txidMode
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_txidMode_get_clicked()
{
    this->father->ATCMDSend("AT+QIOTCFG=\"txid_mode\"");
}
/**************************************************************************
** 功能	@brief :  gwMode
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::on_pushButton_gwMode_get_clicked()
{
    this->father->ATCMDSend("AT+QIOTCFG=\"gw_mode\"");
}
/**************************************************************************
** 功能	@brief :  处理AT指令反馈
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATcmdWindow::handleAtAck(QString cmdData)
{
    qDebug()<<"handleAtAck:"<<cmdData;
    if(cmdData.contains("QIOTCFG=") && !cmdData.contains("?"))
    {
        cmdData.remove("\"");
        // 去除\r\n
        if(cmdData.contains("\r\n"))
        {
            cmdData.chop(2);
        }
        QString payload = cmdData.mid(cmdData.indexOf("=")+1);
        if(payload.size() > 0)
        {
            QStringList list =  payload.split(",");
            QList<QByteArray> paraList;
            for(int i =0;i<list.count();i++)
            {
                paraList.append(QString(list[i]).toLatin1());
            }
            if(list.count() > 1)
            {
                handleConfigGetEvent(paraList);
            }
        }
    }
}

void ATcmdWindow::handleConfigGetEvent(QList<QByteArray> paraList)
{
    qDebug()<<__FUNCTION__<<"paraList:"<<paraList;
    if(paraList.size() >0)
    {
        if(QString(paraList[0]) == "server")
        {
            if(paraList.size() == 3)
            {
                ui->comboBox_protocol->setCurrentIndex(QString(paraList[1]).toInt());
                // 增加对server的QMAP保存
            }
        }
        else if(QString(paraList[0]) == "lifetime")
        {
            if(paraList.size() == 2)
            {
                ui->spinBox_lifetime->setValue(QString(paraList[1]).toInt());
            }
        }
        else if(QString(paraList[0]) == "buffer")
        {
            if(paraList.size() == 2)
            {
                if(QString(paraList[1]).toInt() < ui->comboBox_buffer->count())
                {
                    ui->comboBox_buffer->setCurrentIndex(QString(paraList[1]).toInt());
                }
            }
        }
        else if(QString(paraList[0]) == "act")
        {
            if(paraList.size() == 2)
            {
                ui->spinBox_contextId->setValue(QString(paraList[1]).toInt());
            }
        }
        else if(QString(paraList[0]) == "session_mode")
        {
            if(paraList.size() == 2)
            {
                if(QString(paraList[1]).toInt() < ui->comboBox_sessionMode->count())
                {
                    ui->comboBox_sessionMode->setCurrentIndex(QString(paraList[1]).toInt());
                }
            }
        }
        else if(QString(paraList[0]) == "tsl")
        {
            if(paraList.size() == 2)
            {
                if(QString(paraList[1]).toInt() < ui->comboBox_format->count())
                {
                    ui->comboBox_format->setCurrentIndex(QString(paraList[1]).toInt());
                    ((modelWindow *)(this->father->tabModel))->setLabelShowTls(QString(paraList[1]).toInt()==0?true:false);
                    this->father->userconfigStatus(SETUSERINFO,SENDMMODELMODE,(ui->comboBox_format->currentIndex() == 0)? TTLVMODESNED:JSONMODESEND);
                }
            }
        }
        else if(QString(paraList[0]) == "dk_ds")
        {
            if(paraList.size() == 3)
            {
                ui->lineEdit_dk->setText(paraList[1]);
                ui->lineEdit_ds->setText(paraList[2]);
            }
        }
        else if(QString(paraList[0]) == "psk")
        {
            if(paraList.size() == 2)
            {
                ui->lineEdit_psk->setText(paraList[1]);
            }
        }
        else if(QString(paraList[0]) == "txid_mode")
        {
            if(paraList.size() == 2)
            {
                if(QString(paraList[1]).toInt() < ui->comboBox_txidMode->count())
                {
                    ui->comboBox_txidMode->setCurrentIndex(QString(paraList[1]).toInt());
                }
            }
        }
        else if(QString(paraList[0]) == "gw_mode")
        {
            if(paraList.size() >= 2)
            {
                if(QString(paraList[1]).toInt() < ui->comboBox_gwMode->count())
                {
                    ui->comboBox_gwMode->setCurrentIndex(QString(paraList[1]).toInt());
                }
                if(paraList.size() == 3)
                {
                    ui->lineEdit_gwMode->setText(paraList[2]);
                }
            }
        }
    }
}
void ATcmdWindow::on_comboBox_protocol_activated(const QString &arg1)
{
    if (arg1.indexOf("MQTT") != -1)
    {
        QListView* view = dynamic_cast<QListView*>(ui->comboBox_server->view());
        if (view != nullptr) {
            view->setRowHidden(0, false);
            view->setRowHidden(1, false);
            view->setRowHidden(2, true);
            ui->comboBox_server->setCurrentIndex(1);
        }
    }
    else if (arg1.indexOf("LwM2M") != -1)
    {
        QListView* view = dynamic_cast<QListView*>(ui->comboBox_server->view());
        if (view != nullptr) {
            view->setRowHidden(0, true);
            view->setRowHidden(1, true);
            view->setRowHidden(2, false);
            ui->comboBox_server->setCurrentIndex(2);
        }
    }
}
