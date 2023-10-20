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
#include "debugwindow.h"
#include "ui_debugwindow.h"

#include <QFileDialog>
#include <QNetworkInterface>
#include <QHostAddress>
#include "userconfigure.h"
#include "kernel/mmessagebox.h"
#include "app/atcmdwindow.h"

/*
 * 将AT发送窗口独立出来，不做指令处理
 * 待完善任务：统一tcp/串口的状态识别
 */

static QByteArray tcpBuffer;
static QByteArray serialBuffer;
QByteArray *recvBuffer;
QMutex socketMutex;

debugWindow::debugWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::debugWindow)
{
    ui->setupUi(this);
    this->qserial = new qSerial();
    this->qserial->portListPeriodSet(1000);
    connect(this->qserial,SIGNAL(portListNoticSignal(QList<QString>)),this,SLOT(portListNoticSlot(QList<QString>)));
    connect(this->qserial,SIGNAL(dataReadNoticSignal(QByteArray)),this,SLOT(dataReadNoticSlot(QByteArray)));

    connect(&this->uartTimer, SIGNAL(timeout()), this, SLOT(uartRecvTimeout()));
    localIpListUpdate();
//    this->socket = new QTcpSocket();
    this->socket = NULL;
    this->MaxDebugEvenValue = LL_DBG;
    ui->comboBox_debugSelect->setCurrentIndex(LL_DBG);
    reConnectSum = 0;

    connect(this,SIGNAL(TcpRecvData()),this,SLOT(tcpRecvDataHandle()));
    tcpDataHandleFlag = false;

    this->tcpRecvDelaytimer = new QTimer();
    connect(this->tcpRecvDelaytimer,SIGNAL(timeout()),this,SLOT(tcpRecvTimeout()));
    uiAttributeSet();
    ui->groupBox_14->hide();
    ui->widget_tcp->hide();
    ui->comboBox_runMode->setView(new QListView());
    ui->comboBox_uartPort->setView(new QListView());
    ui->comboBox_uartBate->setView(new QListView());
    ui->comboBox_uartData->setView(new QListView());
    ui->comboBox_uartStop->setView(new QListView());
    ui->comboBox_localIp->setView(new QListView());
    this->sendSTL = new QTimer();
    connect(this->sendSTL,SIGNAL(timeout()),this,SLOT(sendSTLAt()));
    sendSTL->setSingleShot(true);
}

void debugWindow::sendSTLAt()
{
    ((ATcmdWindow*)this->father->atcmdwindow)->getTslValue();
}

debugWindow::~debugWindow()
{
    delete ui;
}
/**************************************************************************
** 功能	@brief :  过滤QComboBox、QSpinBox控件的滚轮信号
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool debugWindow::eventFilter(QObject *obj, QEvent *event)
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
** 功能	@brief :  设置父对象
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::setParm(void *father)
{
    this->father = (MainWindow*)father;
    this->father->atcmdprocess = new atcmdProcess();
    this->father->configprocess->historyRead("ATSend",model_atInput);
    QString value;
    value = this->father->configprocess->uartConfigRead("remote_ip");
    if(!value.isEmpty())
    {
        ui->lineEdit_remoteIp->setText(value);
    }
    value = this->father->configprocess->uartConfigRead("remote_port");
    if(!value.isEmpty())
    {
        ui->spinBox_remotePort->setValue(value.toInt());
    }
    value = this->father->configprocess->uartConfigRead("local_port");
    if(!value.isEmpty())
    {
        ui->spinBox_localPort->setValue(value.toInt());
    }
    value = this->father->configprocess->uartConfigRead("logTime");
    if(!value.isEmpty())
    {
        ui->checkBox_logTime->setChecked(value.toInt());
    }
//    connect(this->father->atcmdprocess,SIGNAL(sendATDataEmit(QString)),this,SLOT(recvATDataEmit(QString)));
    QJsonValue tab_num = this->father->userConfigFile->readJsonFile("interface_connect_mode");
    if(!tab_num.isNull())
    {
        if (tab_num.toInt() == 1)
        {
            ui->pushButton_modeTcp->setChecked(true);
            on_pushButton_modeTcp_clicked();
        }
        else
        {
            ui->pushButton_modeCom->setChecked(true);
            on_pushButton_modeCom_clicked();
        }
//        ui->tabWidget->setCurrentIndex(tab_num.toInt());
    }
}
/**************************************************************************
** 功能	@brief :  控件属性初始化设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::uiAttributeSet()
{
    ui->lineEdit_remoteIp->setClearButtonEnabled(true);
    ui->lineEdit_remoteIp->setInputMask("000.000.000.000; ");
    /* AT命令输入框 */
    model_atInput = new QStandardItemModel(0, 1, this);
    completer_atInput = new QCompleter(model_atInput, this);
    textEdit_atInput = new CompleterTextEdit;
    textEdit_atInput->setAcceptRichText(false);
    textEdit_atInput->setCompleter(completer_atInput);
    QVBoxLayout *vbox_atInput = new QVBoxLayout;
    vbox_atInput->addWidget(textEdit_atInput);
    ui->widget_atInput->setLayout(vbox_atInput);

    recvBuffer = &tcpBuffer;
//    createThread();
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
}
/**************************************************************************
** 功能	@brief :  AT命令发送
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::ATCMDSend(QString data)
{
    qInfo()<<__FUNCTION__;
    if(!checkIsConnectDev())
    {
        return;
    }
    QString dataHear = "";
    if(ui->checkBox_logTime->isChecked())
    {
        dataHear = dataHear + "["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]";
    }
    dataHear = dataHear + "[TX]";
    this->father->logTextEditDisplay(dataHear,TEXTEDITAPPENDMODE);
    QString dataValue = data+"\r\n";
    if(ui->checkBox_logSave->isChecked())
    {
        this->logfile->fileManagerWrite(dataHear.toLatin1()+dataValue.toLatin1(),FileKeepMode);
    }
    this->father->logTextEditDisplay(dataValue.toUtf8(),TEXTEDITINSERTMODE);
    if(this->socket != NULL && this->socket->isOpen())
    {
//        if(debugLevel == QtCriticalMsg)
            qDebug()<<"send data:"<<dataValue;
//        this->father->setrecvSerialSendMutex(true);
        int ret = this->socket->write(dataValue.toUtf8(),dataValue.toUtf8().length());
        this->socket->flush();
        if(ret == -1)
        {
            this->socket->close();
            ui->pushButton_tcp->setEnabled(true);
            ui->lineEdit_remoteIp->setEnabled(true);
            ui->spinBox_remotePort->setEnabled(true);
            ui->spinBox_localPort->setEnabled(true);
            ui->comboBox_localIp->setEnabled(true);
            ui->pushButton_uartOpen->setEnabled(true);
            ui->pushButton_tcp->setText(tr("打开TCP"));
        }
    }
    else if(this->qserial->serialIsOpen())
    {
        qInfo()<<" seriel send";
//        this->father->setrecvSerialSendMutex(true);
        this->qserial->SerialSend(dataValue.toUtf8());
    }
    if(debugLevel == QtCriticalMsg)
        qInfo()<<" data atcmdSendAnalysis start";
    bool ret = this->father->atcmdprocess->atcmdSendAnalysis(data.toLatin1());
    Q_UNUSED(ret);
    atHubInstance.atSendLogSave(data);
//    if(ret == false);
    if(debugLevel == QtCriticalMsg)
        qInfo()<<" data atcmdSendAnalysis end";
}


/**************************************************************************
** 功能	@brief :  AT命令发送
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::ATCMDSend(QByteArray data,bool externFlag)
{
    Q_UNUSED(externFlag);
    qInfo()<<__FUNCTION__;
    if(!checkIsConnectDev())
    {
        return;
    }
    QString dataHear;
    if(ui->checkBox_logTime->isChecked())
    {
        dataHear.append("["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]");
    }
    dataHear.append("[TX]");
    this->father->logTextEditDisplay(dataHear,TEXTEDITAPPENDMODE);
    QByteArray dataValue = data+QString("\r\n").toUtf8();

    if(ui->checkBox_logSave->isChecked())
    {
        this->logfile->fileManagerWrite(dataHear.toLatin1()+dataValue,FileKeepMode);
    }
    this->father->logTextEditDisplay(dataValue,TEXTEDITINSERTMODE);
    if(this->socket != NULL && this->socket->isOpen())
    {
//        if(debugLevel == QtCriticalMsg)
//        this->father->setrecvSerialSendMutex(true);
        int ret = this->socket->write(dataValue.data(),dataValue.size());
        this->socket->flush();
        if(ret == -1)
        {
            this->socket->close();
            ui->pushButton_tcp->setEnabled(true);
            ui->lineEdit_remoteIp->setEnabled(true);
            ui->spinBox_remotePort->setEnabled(true);
            ui->spinBox_localPort->setEnabled(true);
            ui->comboBox_localIp->setEnabled(true);
            ui->pushButton_uartOpen->setEnabled(true);
            ui->pushButton_tcp->setText(tr("打开TCP"));
        }
    }
    else if(this->qserial->serialIsOpen())
    {
        qInfo()<<" seriel send";
//        this->father->setrecvSerialSendMutex(true);
        this->qserial->SerialSend(dataValue);
    }
//    else
//    {
//        MMessageBox::information(this,tr("提示"),tr("请重新打开串口或者TCP"),tr("确认"));
//        return;
//    }
    if(debugLevel == QtCriticalMsg)
        qInfo()<<" data atcmdSendAnalysis start";
    bool ret = this->father->atcmdprocess->atcmdSendAnalysis(data);
    Q_UNUSED(ret);
//    if(ret == false);
    if(debugLevel == QtCriticalMsg)
        qInfo()<<" data atcmdSendAnalysis end";
}
/**************************************************************************
** 功能	@brief :  AT命令发送hex数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::ATCMDSendHex(QByteArray hexData)
{
    qInfo()<<__FUNCTION__<<"hexData:"<<hexData<<hexData.length();
    if(!checkIsConnectDev())
    {
        return;
    }
    QString dataHear;
    if(ui->checkBox_logTime->isChecked())
    {
        dataHear.append("["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]");
    }
    
    dataHear.append("[TX]");
    this->father->logTextEditDisplay("\n"+dataHear+QString(hexData),TEXTEDITAPPENDMODE);
    if(this->socket != NULL && this->socket->isOpen())
    {
        int ret = this->socket->write(hexData);
        if(hexData.count() >0)
        {
            if(this->father->comboxHexDataSelect())
            {
                this->father->logTextEditDisplay("\n"+ tr("16进制数据显示")+":"+QString(hexData.toHex()),TEXTEDITINSERTMODE);
            }
        }
        if(ret == -1)
        {
            this->socket->close();
            ui->pushButton_tcp->setEnabled(true);
            ui->lineEdit_remoteIp->setEnabled(true);
            ui->spinBox_remotePort->setEnabled(true);
            ui->spinBox_localPort->setEnabled(true);
            ui->comboBox_localIp->setEnabled(true);
            ui->pushButton_uartOpen->setEnabled(true);
            ui->pushButton_tcp->setText(tr("打开TCP"));
        }
        this->father->atcmdprocess->sendDataHexFlag = false;
    }
    else if(this->qserial->serialIsOpen())
    {
        qDebug()<<"sendSum:"<<this->qserial->SerialSend(hexData);

        if(hexData.count() >0 && this->father->comboxHexDataSelect() == true)
        {
            this->father->logTextEditDisplay("\n" + tr("16进制数据显示")+":"+QString(hexData.toHex()),TEXTEDITINSERTMODE);
        }
        this->father->atcmdprocess->sendDataHexFlag = false;
    }
}
/**************************************************************************
** 功能	@brief :  AT命令发送
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::ATCMDSend(QString strData,QByteArray hexData)
{
    qInfo()<<__FUNCTION__;
    if(!checkIsConnectDev())
    {
        return;
    }
    QString dataHear;
    if(ui->checkBox_logTime->isChecked())
    {
        dataHear.append("["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]");
    }
    dataHear.append("[TX]");
    this->father->logTextEditDisplay(dataHear,TEXTEDITAPPENDMODE);
    QByteArray dataValue = (strData+"\r\n").toLatin1();
    this->father->atcmdprocess->sendDataHex = hexData;
    qDebug()<<"send data:"<<dataValue.data()<<"hexdata:"<<hexData.toHex();
    if(ui->checkBox_logSave->isChecked())
    {
        this->logfile->fileManagerWrite(dataHear.toLatin1()+dataValue,FileKeepMode);
    }
    this->father->logTextEditDisplay(QString(dataValue).toUtf8(),TEXTEDITINSERTMODE);
    if(this->socket != NULL && this->socket->isOpen())
    {
//        if(this->father->getrecvSerialSendMutex())
//        {
//            qDebug()<<QString("当前使用中");
//            return;
//        }
//        this->father->setrecvSerialSendMutex(true);
        int ret = this->socket->write(dataValue);
        if(ret == -1)
        {
            this->socket->close();
            ui->pushButton_tcp->setEnabled(true);
            ui->lineEdit_remoteIp->setEnabled(true);
            ui->spinBox_remotePort->setEnabled(true);
            ui->spinBox_localPort->setEnabled(true);
            ui->comboBox_localIp->setEnabled(true);
            ui->pushButton_uartOpen->setEnabled(true);
            ui->pushButton_tcp->setText(tr("打开TCP"));
        }
    }
    else if(this->qserial->serialIsOpen())
    {
        if(this->father->getrecvSerialSendMutex())
        {
            qDebug()<<"当前使用中";
            return;
//            this->father->setrecvSerialSendMutex(false);
//        this->father->setrecvSerialSendMutex(true);
        }
//        this->father->setrecvSerialSendMutex(true);
        this->qserial->SerialSend(dataValue);
    }
    this->father->atcmdprocess->sendDataHexFlag = true;
    this->father->atcmdprocess->atcmdSendAnalysis(strData.toLatin1());
}

void debugWindow::ATCMDSignalSend(QString data)
{
    if(!checkIsConnectDev())
    {
        return;
    }
    QString dataHear;
    if(ui->checkBox_logTime->isChecked())
    {
        dataHear.append("["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]");
    }
    dataHear.append("[TX]");
    this->father->logTextEditDisplay(dataHear,TEXTEDITAPPENDMODE);
    QString dataValue = data+"\r\n";
    if(ui->checkBox_logSave->isChecked())
    {
        this->logfile->fileManagerWrite(dataHear.toLatin1()+dataValue.toLatin1(),FileKeepMode);
    }
    this->father->logTextEditDisplay(dataValue.toUtf8(),TEXTEDITINSERTMODE);
    if(this->socket != NULL && this->socket->isOpen())
    {
        qDebug()<<"send data:"<<dataValue;
        int ret = this->socket->write(dataValue.toUtf8(),dataValue.toUtf8().length());
        this->socket->flush();
        if(ret == -1)
        {
            this->socket->close();
            ui->pushButton_tcp->setEnabled(true);
            ui->lineEdit_remoteIp->setEnabled(true);
            ui->spinBox_remotePort->setEnabled(true);
            ui->spinBox_localPort->setEnabled(true);
            ui->comboBox_localIp->setEnabled(true);
            ui->pushButton_uartOpen->setEnabled(true);
            ui->pushButton_tcp->setText(tr("打开TCP"));
        }
    }
    else if(this->qserial->serialIsOpen())
    {
        qInfo()<<" seriel send";
        this->qserial->SerialSend(dataValue.toUtf8());
    }
}
/**************************************************************************
** 功能	@brief :  串口信息更新
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::portListNoticSlot(QList<QString> portList)
{
    qInfo()<<__FUNCTION__;
    qDebug()<<"串口列表发生变化"<<portList;
    if(portList.count() <= 0)
    {
        qWarning()<<"串口列表为空";
        return;
    }
    QString currentPort = ui->comboBox_uartPort->currentText();
    ui->comboBox_uartPort->clear();
    ui->comboBox_uartPort->addItems(portList);

    if(this->qserial->serialIsOpen() && portList.contains(currentPort) == false)
    {
        on_pushButton_uartOpen_clicked();
    }
    else if(this->qserial->serialIsOpen() == false)
    {
            ui->comboBox_uartPort->setCurrentText(portList.first());
    }
    else
    {
        ui->comboBox_uartPort->setCurrentText(currentPort);
    }

    //调整下拉列表宽度，完整显示
    int max_len = 0;
    for(int idx = 0; idx < ui->comboBox_uartPort->count(); idx ++)
    {
        if(max_len < ui->comboBox_uartPort->itemText(idx).toLocal8Bit().length())
        {
            max_len = ui->comboBox_uartPort->itemText(idx).toLocal8Bit().length();
        }
    }

    if (max_len * 12 * 0.75 < ui->comboBox_uartPort->width())
    {
        ui->comboBox_uartPort->view()->setFixedWidth(ui->comboBox_uartPort->width());
    }
    else
    {
        ui->comboBox_uartPort->view()->setFixedWidth(max_len * 12 * 0.75);
    }

}
/**************************************************************************
** 功能	@brief :  窗口关闭事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::closeSocket()
{
    if(this->qserial->serialIsOpen())
    {
        on_pushButton_uartOpen_clicked();
    }
    else
    {
        on_pushButton_runStart_clicked();
    }
}
/**************************************************************************
** 功能	@brief :  设置DMP的连接
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::setDMPConnectStatus(int mode)
{
    qDebug()<<"mode:"<<mode;
    if(0 == mode)
    {
        ui->pushButton_runStart->setText(tr("启动连接"));
        ui->comboBox_runMode->setEnabled(true);
        this->father->userconfigStatus(SETUSERINFO,MQTTSTATUS,false);
    }
    else
    {
        ui->pushButton_runStart->setText(tr("停止连接"));
        ui->comboBox_runMode->setEnabled(false);
        this->father->userconfigStatus(SETUSERINFO,MQTTSTATUS,true);
    }

}

#include <QTextCodec>
#include <QFile>
enum class EncodingFormat : int
{
    ANSI = 0,//GBK
    UTF16LE,
    UTF16BE,
    UTF8,
    UTF8BOM,
};

/*!
 * \brief 检查文件编码
 */
EncodingFormat FileCharacterEncoding(const QString data)
{
    //假定默认编码utf8
    EncodingFormat code = EncodingFormat::UTF8;
    //读取3字节用于判断
    QByteArray buffer = data.toLocal8Bit();
    quint8 sz1st = buffer.at(0);
    quint8 sz2nd = buffer.at(1);
    quint8 sz3rd = buffer.at(2);
    if (sz1st == 0xFF && sz2nd == 0xFE)
    {
        code = EncodingFormat::UTF16LE;
    }
    else if (sz1st == 0xFE && sz2nd == 0xFF)
    {
        code = EncodingFormat::UTF16BE;
    }
    else if (sz1st == 0xEF && sz2nd == 0xBB && sz3rd == 0xBF)
    {
        code = EncodingFormat::UTF8BOM;
    }
    else
    {
        //尝试用utf8转换,如果无效字符数大于0,则表示是ansi编码
        QTextCodec::ConverterState cs;
        QTextCodec* tc = QTextCodec::codecForName("utf-8");
        tc->toUnicode(buffer.constData(), buffer.size(), &cs);
        code = (cs.invalidChars > 0) ? EncodingFormat::ANSI : EncodingFormat::UTF8;
    }
    qDebug()<<"code:"<<(code == EncodingFormat::ANSI ? 0: 1);

    return code;
}


bool debugWindow::retranslationUi()
{
    bool isOpenTcpStatus = false;
    bool isOpenPortStatus = false;
    bool isConnectStatus = false;

    FileCharacterEncoding(ui->pushButton_uartOpen->text().toUtf8());
    FileCharacterEncoding("关闭串口");
    qDebug()<<ui->pushButton_uartOpen->text().toUtf8()<<","<<tr("停止连接").toUtf8();
    if(ui->pushButton_tcp->text() == "关闭TCP" || ui->pushButton_tcp->text() == "Disable TCP")
    {
        isOpenTcpStatus = true;
    }
    if(ui->pushButton_uartOpen->text() == "关闭串口" || ui->pushButton_uartOpen->text() == "Disable Serial Port")
    {
        isOpenPortStatus = true;
    }
    if(ui->pushButton_runStart->text() == "停止连接" || ui->pushButton_runStart->text() == "Stop Connection")
    {
        isConnectStatus = true;
    }
    ui->retranslateUi(this);

    qDebug()<<"isOpenTcpStatus:"<<isOpenTcpStatus<<",isOpenPortStatus:"<<isOpenPortStatus<<",isConnectStatus:"<<isConnectStatus;
    if(isOpenTcpStatus)
    {
        ui->pushButton_tcp->setText(tr("关闭TCP"));
    }
    if(isOpenPortStatus)
    {
        ui->pushButton_uartOpen->setText(tr("关闭串口"));
    }
    if(isConnectStatus)
    {
        ui->pushButton_runStart->setText(tr("停止连接"));
//        ((ATcmdWindow*)this->father->atcmdwindow)->getTslValue();
    }

    return true;
}

void debugWindow::nonBlockingSleep(int sectime)
{

    QTime dieTime = QTime::currentTime().addMSecs(sectime);

    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    }

}

bool debugWindow::checkIsConnectDev()
{
    if( (this->socket== NULL || !this->socket->isOpen()) && !this->qserial->serialIsOpen())
    {
        MMessageBox::critical(this,tr("错误"),tr("请先打开串口或者TCP"),tr("确认"));
        this->father->resetSendMutex();
        return false;
    }
    return true;
}

/*
* @param [int]str 字符串
* @param [int]size 字符串长度
* @param [out]qstring QString 引用
*/
QString bytesToHex(char str)
{
    QString qstring;
    char tempOne = str >> 4;

    if (tempOne >= 0x0a)
    {
        tempOne = 0x41 - 0x0A;
    }
    else
    {
        tempOne = tempOne + 0x30;
    }

    char tempTwo = str & 0x0F;

    if (tempTwo >= 0x0a)
    {
        tempTwo = tempTwo + 0x41 - 0x0a;
    }
    else
    {
        tempTwo = tempTwo + 0x30;
    }
    qstring.append(tempOne);
    qstring.append(tempTwo);
    qstring.append(" ");
    return qstring;
}

QString debugWindow::handleDisplayData(QByteArray array)
{
    QString string;
    for(int i=0;i<array.length();i++)
    {
        char character = char(array[i]);
//        if((character <33 || character >126) && (character != 10 && character != 13 && character != 32))
        if(0 == character || character > 126)
        {
            string.append("\\x"+bytesToHex(character));
        }
        else
        {
            string.append(character);
        }
    }
    return string;
}



/**************************************************************************
** 功能	@brief :  串口操作
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_pushButton_uartOpen_clicked()
{
    qInfo()<<__FUNCTION__;
    uartIsOpen = false;

    if(ui->pushButton_tcp->text() == tr("关闭TCP"))
    {
        MMessageBox::information(this,tr("提示"),tr("请先关闭TCP"),tr("确认"));
        return;
    }
    if(ui->pushButton_uartOpen->text() == tr("打开串口"))
    {
        qDebug()<< "打开串口";
        if( true == this->qserial->serialOpen(ui->comboBox_uartPort->currentText(),ui->comboBox_uartBate->currentText().toInt(),ui->comboBox_uartData->currentText().toInt(),QSerialPort::NoParity,ui->comboBox_uartStop->currentText().toInt(),QSerialPort::NoFlowControl))
        {

            ui->pushButton_uartOpen->setText(tr("关闭串口"));
            ui->comboBox_uartPort->setEnabled(false);
            ui->comboBox_uartBate->setEnabled(false);
            ui->comboBox_uartData->setEnabled(false);
            ui->comboBox_uartStop->setEnabled(false);
            ui->pushButton_tcp->setEnabled(false);
            this->father->userconfigStatus(SETUSERINFO,SOCKETSTATUS,true);
            serialBuffer.clear();
            recvBuffer = &serialBuffer;
            resetUIValue();
            uartIsOpen = true;
        }
        else
        {
            MMessageBox::critical(this,tr("错误"),tr("串口打开失败，请检查是否被占用"),tr("确认"));
        }
    }
    else
    {
        this->qserial->SerialClose();
        ui->pushButton_uartOpen->setText(tr("打开串口"));
        ui->comboBox_uartPort->setEnabled(true);
        ui->comboBox_uartBate->setEnabled(true);
        ui->comboBox_uartData->setEnabled(true);
        ui->comboBox_uartStop->setEnabled(true);
        ui->pushButton_tcp->setEnabled(true);
        this->father->userconfigStatus(SETUSERINFO,SOCKETSTATUS,false);
        closeDmpConnect();
//        this->father->ATCMDSend("AT+QIOTREG=0");
    }
}

/**************************************************************************
** 功能	@brief :  tcp连接
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_pushButton_runStart_clicked()
{
    qInfo() << __func__<<__LINE__<<"on_pushButton_runStart_clicked";
    if(ui->pushButton_runStart->text() == tr("启动连接"))
    {
        if(this->qserial->serialIsOpen() || (this->socket != NULL && this->socket->isOpen()))
        {
            setDMPConnectStatus(1);
            this->father->ATCMDSend("AT+QIOTREG="+QString::number(ui->comboBox_runMode->currentIndex()+1));
            sendSTL->start(1000);
        }
        else
        {
            MMessageBox::critical(this,tr("错误"),tr("请先打开串口或TCP"),tr("确认"));
        }
    }
    else
    {
        setDMPConnectStatus(0);
        this->father->ATCMDSend("AT+QIOTREG=0");
    }
}
/**************************************************************************
** 功能	@brief :  AT命令发送
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_pushButton_atSend_clicked()
{
    qInfo()<<__FUNCTION__;
    for(int i=0;i<model_atInput->rowCount();i++)
    {
        if(model_atInput->index(i,0).data().toString() == textEdit_atInput->toPlainText())
        {
            model_atInput->removeRow(i);
            break;
        }
    }
    model_atInput->insertRow(0);
    model_atInput->setData(model_atInput->index(0, 0), textEdit_atInput->toPlainText());
    if(model_atInput->rowCount() > 30)
    {
        model_atInput->removeRow(30);
    }
    ATCMDSignalSend(textEdit_atInput->toPlainText());
    // this->father->ATCMDSend(textEdit_atInput->toPlainText());

    this->father->configprocess->historySave("ATSend",*model_atInput);
    this->father->statusBarShow(tr("AT指令已发送"),3);
}
/**************************************************************************
** 功能	@brief :  输出日志保存
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_pushButton_logFile_clicked()
{
    qInfo()<<__FUNCTION__;
    QString fileName ="queciot_uartLog_"+QDateTime::currentDateTime().toString("yyMMdd_hhmmss")+".txt";
    QString file = QFileDialog::getSaveFileName(0,tr("选择日志存储路径"),fileName,"*.txt");
    if(file.isEmpty())
        return;
    this->logfile = new fileManager();
    if(this->logfile->fileManagerOpen(file,FileKeepMode) == false)
    {
        MMessageBox::critical(this,tr("错误"),tr("日志文件创建失败"),tr("确认"));
        return;
    }
    ui->lineEdit_logPath->setText(file);
    ui->lineEdit_logPath->setToolTip(file);
}
/**************************************************************************
** 功能	@brief :  输出日志清空
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_pushButton_logClear_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->logTextEditClean();
    this->father->tablewidgetDataAnalysisClear();
}
/**************************************************************************
** 功能	@brief :  连接模式设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::csdkConnectStatus(int status)
{
    qInfo()<<__FUNCTION__;
    ui->comboBox_runStatus->setCurrentIndex(status);
}
/**************************************************************************
** 功能	@brief :  输出日志保存地址
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_checkBox_logSave_toggled(bool checked)
{
    qInfo()<<__FUNCTION__;
    if(checked && ui->lineEdit_logPath->text().isEmpty())
    {
        ui->checkBox_logSave->setChecked(false);
        MMessageBox::information(this,tr("提示"),tr("请先选择日志保存路径"),tr("确认"));
    }
}
/**************************************************************************
** 功能	@brief :  ip地址更新
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::localIpListUpdate(void)
{
    qInfo()<<__FUNCTION__;
    ui->comboBox_localIp->clear();
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        // 主机地址为空
        if (address.isNull())
            continue;
        QAbstractSocket::NetworkLayerProtocol nProtocol = address.protocol();
        QString strAddress = address.toString();
        // 如果是IPv4
        if (nProtocol == QAbstractSocket::IPv4Protocol)
        {
            bool Ok = false;
            address.toIPv4Address(&Ok);
            if (Ok)
            {
                ui->comboBox_localIp->addItem(strAddress);
            }
        }
        // 如果是IPv6
        else if (nProtocol == QAbstractSocket::IPv6Protocol)
        {
            QStringList IPV6List("");
            Q_IPV6ADDR IPV6 = address.toIPv6Address();
            for (int i = 0; i < 16; ++i)
            {
                quint8 nC = IPV6[i];
                IPV6List << QString::number(nC);
            }
        }
    }
    ui->comboBox_localIp->setCurrentText("127.0.0.1");
}
/**************************************************************************
** 功能	@brief :  tcp断开
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::socket_Disconnected(void)
{
    qInfo()<<__FUNCTION__;
    this->father->userconfigStatus(SETUSERINFO,SOCKETSTATUS,false);
    if(this->socket != NULL && this->socket->isOpen())
        this->socket->close();
    ui->pushButton_tcp->setText(tr("打开TCP"));
    ui->comboBox_runStatus->setCurrentIndex(0);
    ui->pushButton_uartOpen->setEnabled(true);
    ui->comboBox_runMode->setEnabled(true);
    ui->pushButton_tcp->setEnabled(true);
    ui->lineEdit_remoteIp->setEnabled(true);
    ui->spinBox_remotePort->setEnabled(true);
    ui->spinBox_localPort->setEnabled(true);
    ui->comboBox_localIp->setEnabled(true);
    ui->pushButton_uartOpen->setEnabled(true);
    setDMPConnectStatus(0);
}
/**************************************************************************
** 功能	@brief :  tcp连接
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool debugWindow::tcpConnect()
{
    if(this->socket != NULL && this->socket->isOpen())
    {
        this->socket->close();
    }
    this->socket = new QTcpSocket();
    if(this->socket->state() == QAbstractSocket::ConnectedState)
    {
        this->socket->abort();
    }
    if(this->socket->state() == QAbstractSocket::UnconnectedState)
    {
//        if (this->socket->state() != this->socket->BoundState)
//        {
//            if(!(this->socket->bind(QHostAddress(ui->comboBox_localIp->currentText()),ui->spinBox_localPort->value())))
//            {
//                qCritical()<<QString("bind失败");
//            }
//        }

        QObject::connect(this->socket, &QTcpSocket::readyRead, this, &debugWindow::socket_Read_Data);
        QObject::connect(this->socket, &QTcpSocket::disconnected, this, &debugWindow::socket_Disconnected);
        this->socket->connectToHost(QHostAddress(ui->lineEdit_remoteIp->text()), qint16(ui->spinBox_remotePort->value()));
        //等待连接成功
        if(!(this->socket->waitForConnected(3000)))
        {
            this->socket->disconnectFromHost();
            this->socket->close();
            return false;
        }
    }
    return true;
}
/**************************************************************************
** 功能	@brief :  窗口控件状态清除
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::resetUIValue()
{
    ui->comboBox_runMode->setCurrentIndex(0);
    ui->pushButton_runStart->setText(tr("启动连接"));
    this->father->userconfigStatus(SETUSERINFO,MQTTSTATUS,false);
    ui->checkBox_logSave->setChecked(false);
    if(NULL != this->logfile && this->logfile->fileInfo.len > 0)
    {
        this->logfile->fileManagerClose();
        this->logfile = NULL;
    }
    ui->lineEdit_logPath->clear();
    this->father->resetUIValue();
}

/**************************************************************************
** 功能	@brief :  tcp重连
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::tcpConnectTimeout()
{
    tcpConnectTimer->stop();
    if(reConnectSum <2)
    {
        reConnectSum++;
        QString dataHear;
        if(ui->checkBox_logTime->isChecked())
        {
            dataHear.append("["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]");
        }
        this->father->logTextEditDisplay(dataHear+tr("重连尝试 第%1次").arg(QString::number(reConnectSum)),TEXTEDITAPPENDMODE);
        if( tcpConnect() == true)
        {
            reConnectSum = 0;
            ui->pushButton_tcp->setEnabled(true);
            ui->pushButton_tcp->setText(tr("关闭TCP"));
            this->father->configprocess->uartConfigSave("remote_ip",ui->lineEdit_remoteIp->text());
            this->father->configprocess->uartConfigSave("remote_port",QString::number(ui->spinBox_remotePort->value()));
            this->father->configprocess->uartConfigSave("local_port",QString::number(ui->spinBox_localPort->value()));
            this->father->userconfigStatus(SETUSERINFO,SOCKETSTATUS,true);
            delete tcpConnectTimer;
            tcpBuffer.clear();
            recvBuffer = &tcpBuffer;
            resetUIValue();
            this->tcpRecvDelaytimer->start(200);
        }
        else
        {
            tcpConnectTimer->start(1000);
        }
    }
    else
    {
        reConnectSum = 0;
        ui->pushButton_tcp->setEnabled(true);
        ui->lineEdit_remoteIp->setEnabled(true);
        ui->spinBox_remotePort->setEnabled(true);
        ui->spinBox_localPort->setEnabled(true);
        ui->comboBox_localIp->setEnabled(true);
        ui->pushButton_uartOpen->setEnabled(true);
        MMessageBox::critical(this,tr("错误"),tr("TCP连接失败"),tr("确认"));
        delete tcpConnectTimer;
        this->tcpRecvDelaytimer->stop();
    }
}

/**************************************************************************
** 功能	@brief :  tcp操作
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_pushButton_tcp_clicked()
{
    qInfo()<<__FUNCTION__;
    tcpIsOpen = false;
    if(ui->pushButton_uartOpen->text() == tr("关闭串口"))
    {
        MMessageBox::warning(this,tr("警告"),tr("请先关闭串口"),tr("确认"));
        return;
    }
    if(ui->pushButton_tcp->text() == tr("打开TCP"))
    {
        if(0 == ui->lineEdit_remoteIp->text().compare(ui->comboBox_localIp->currentText())  && 0 == ui->spinBox_remotePort->text().compare(ui->spinBox_localPort->text()))
        {
                MMessageBox::information(this,tr("提示"),tr("远程服务器地址和端口号不能和本地完全一致"),tr("确认"));
                return;
        }
        ui->pushButton_tcp->setEnabled(false);
        QString dataHear;
        if(ui->checkBox_logTime->isChecked())
        {
            dataHear.append("["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]");
        }
        this->father->logTextEditDisplay(dataHear+tr("开始连接tcp服务器"),TEXTEDITAPPENDMODE);
        nonBlockingSleep(10);
        bool ret = tcpConnect();
        if(ret == false)
        {

            this->father->logTextEditDisplay(dataHear+tr("连接失败，尝试重连"),TEXTEDITAPPENDMODE);
            this->father->userconfigStatus(SETUSERINFO,SOCKETSTATUS,false);
            tcpConnectTimer = new QTimer();
            connect(tcpConnectTimer,SIGNAL(timeout()),this,SLOT(tcpConnectTimeout()));
            tcpConnectTimer->start(1000);
            qInfo()<<tr("启动定时器");
        }
        else
        {
            ui->pushButton_tcp->setText(tr("关闭TCP"));
            this->father->configprocess->uartConfigSave("remote_ip",ui->lineEdit_remoteIp->text());
            this->father->configprocess->uartConfigSave("remote_port",QString::number(ui->spinBox_remotePort->value()));
            this->father->configprocess->uartConfigSave("local_port",QString::number(ui->spinBox_localPort->value()));
            ui->pushButton_tcp->setEnabled(true);
            this->father->userconfigStatus(SETUSERINFO,SOCKETSTATUS,true);
            tcpBuffer.clear();
            recvBuffer = &tcpBuffer;
            resetUIValue();
            this->tcpRecvDelaytimer->start(200);
            tcpIsOpen = true;
        }
        ui->lineEdit_remoteIp->setEnabled(false);
        ui->spinBox_remotePort->setEnabled(false);
        ui->spinBox_localPort->setEnabled(false);
        ui->comboBox_localIp->setEnabled(false);
        ui->pushButton_uartOpen->setEnabled(false);
    }
    else if(ui->pushButton_tcp->text() == tr("关闭TCP"))
    {
        closeDmpConnect();
        QObject::disconnect(this->socket, &QTcpSocket::readyRead, this, &debugWindow::socket_Read_Data);
        QObject::disconnect(this->socket, &QTcpSocket::disconnected, this, &debugWindow::socket_Disconnected);
        socket_Disconnected();
        this->tcpRecvDelaytimer->stop();
    }
}
/**************************************************************************
** 功能	@brief :  当前sdk工作状态获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_pushButton_status_clicked()
{
    qInfo()<<__FUNCTION__;
    if(this->qserial->serialIsOpen() || (this->socket != NULL && this->socket->isOpen()))
    {
        this->father->ATCMDSend("AT+QIOTSTATE?");
    }
    else
    {
        MMessageBox::critical(this,tr("错误"),tr("请先打开串口或TCP"),tr("确认"));
    }
}
/**************************************************************************
** 功能	@brief :  窗口输出信息的时间戳显示
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_checkBox_logTime_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    qInfo()<<__FUNCTION__;
    this->father->configprocess->uartConfigSave("logTime",QString::number(ui->checkBox_logTime->isChecked()));
}
/**************************************************************************
** 功能	@brief : 调试日志保存
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::debugfileSave(QString data,int currentEvenValue)
{
    QString dataHear;
    if(ui->checkBox_logTime->isChecked())
    {
        dataHear.append("["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]");
    }
    dataHear.append("[TX]");
    QString dataValue = data+"\r\n";
    if(currentEvenValue >= this->MaxDebugEvenValue)
    {
        this->lock.lockForWrite();
        this->debugfile->fileManagerWrite(dataHear.toLatin1()+dataValue.toLatin1(),FileKeepMode);
        this->lock.unlock();
    }
}

extern int debugLevel;
/**************************************************************************
** 功能	@brief :  调试日志等级设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_comboBox_debugSelect_currentIndexChanged(int index)
{
    this->MaxDebugEvenValue = index;
    debugLevel = index;
}
/**************************************************************************
** 功能	@brief :  清除调试日志
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::on_pushButton_clearDebugFile_clicked()
{
    QFile debugLogFile;
    QString configfile = vitalFilePath+".debug.ini";
    debugLogFile.setFileName(configfile);
    readWriteLock.lockForWrite();
    debugLogFile.open(QIODevice::WriteOnly | QIODevice::Append);
    if(debugLogFile.isOpen())
        debugLogFile.resize(0);
    else
        MMessageBox::critical(this,tr("错误"),tr("无法清除调试"),tr("确认"));
    debugLogFile.close();
    readWriteLock.unlock();
}
/**************************************************************************
** 功能	@brief :  tcp接收超时
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::tcpRecvTimeout()
{
    if(!tcpBuffer.isEmpty() && !tcpDataHandleFlag)
    {
        emit TcpRecvData();
        if(errorHandleRecvData < 10 && this->tcpRecvDelaytimer->interval() > 10 )
        {
            this->tcpRecvDelaytimer->setInterval(1);
            this->tcpRecvDelaytimer->start();
        }
    }
    else if(tcpBuffer.isEmpty())
    {
        this->tcpRecvDelaytimer->setInterval(10);
        this->tcpRecvDelaytimer->start();
    }
}


// 接收处理
void debugWindow::socket_Read_Data(void)
{
    qInfo()<<__FUNCTION__;

    QByteArray recvData = socket->readAll();
    tcpBuffer.append(recvData);
    QString dataHear;
    if(ui->checkBox_logTime->isChecked())
    {
        dataHear.append("["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]");
    }
    dataHear.append("[RX]");
    this->father->logTextEditDisplay(dataHear,TEXTEDITAPPENDMODE);
    this->father->logTextEditDisplay(recvData,TEXTEDITINSERTMODE);
//    handleDisplayData(recvData)

    if(ui->checkBox_logSave->isChecked())
    {
        qDebug()<<this->logfile->fileManagerWrite(dataHear.toLatin1()+recvData,FileKeepMode);
    }
}

/**************************************************************************
** 功能	@brief :  tcp数据处理
** 输入	@param :
** 输出	@retval:
***************************************************************************/

void debugWindow::tcpRecvDataHandle()
{
//    qInfo()<<__FUNCTION__;
    tcpDataHandleFlag = true;
    QByteArray hexData;
    int readLen = 0;
    int oldreadLen =0 ;
    while(!tcpBuffer.isEmpty())
    {
        readLen = this->father->atcmdprocess->atcmdRecvAnalysis(tcpBuffer,hexData);
        qDebug()<<"readLen:"<<readLen;
        if(readLen != 0)
        {
            if(hexData.count() >0 && this->father->comboxHexDataSelect() == true)
            {
                this->father->logTextEditDisplay(tr("16进制数据：")+QString(hexData.toHex()),TEXTEDITAPPENDMODE);
                hexData.clear();
            }
            tcpBuffer.remove(0,readLen);
            this->father->atcmdprocess->runNextFunc();
        }
        else
            break;
        if(oldreadLen != readLen)
        {
            oldreadLen = readLen;
            errorHandleRecvData = 0;
        }
        else
        {
            errorHandleRecvData++;
            break;
        }
    }
    tcpDataHandleFlag = false;
}


/**************************************************************************
** 功能	@brief :  串口接收定时器超时
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void debugWindow::dataReadNoticSlot(QByteArray recvData)
{
    qInfo()<<__FUNCTION__;
    serialBuffer.append(recvData);
    //新建定时器200ms
    this->uartTimer.stop();
    this->uartTimer.start(200);

}
/**************************************************************************
** 功能	@brief :  串口数据处理
** 输入	@param :
** 输出	@retval:
***************************************************************************/
static int serialBufferLength = 0;
static int maxHandle = 0;
void debugWindow::uartRecvTimeout()
{
//    qInfo()<<__FUNCTION__;
    QString dataHear;

    if(serialBufferLength != serialBuffer.length())
    {
        if(ui->checkBox_logTime->isChecked())
        {
            dataHear.append("["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]");
        }
        dataHear.append("[RX]");
        this->father->logTextEditDisplay(dataHear,TEXTEDITAPPENDMODE);
        this->father->logTextEditDisplay(QString(serialBuffer.right(serialBuffer.length()-serialBufferLength)).toUtf8(),TEXTEDITINSERTMODE);
        if(ui->checkBox_logSave->isChecked())
        {
            qDebug()<<this->logfile->fileManagerWrite(dataHear.toLatin1()+serialBuffer.toHex(),FileKeepMode);
        }
    }
    while (serialBuffer.length())
    {

        QByteArray hexData;
        int readLen = this->father->atcmdprocess->atcmdRecvAnalysis(serialBuffer,hexData);
        if(readLen > 0)
        {
            if(hexData.count() >0 && this->father->comboxHexDataSelect() == true)
            {
                this->father->logTextEditDisplay(tr("16进制数据：")+QString(hexData.toHex()),TEXTEDITINSERTMODE);
            }
            serialBuffer.remove(0,readLen);
            this->father->atcmdprocess->runNextFunc();
        }
        else if(readLen == 0)
        {
            break;
        }
    }
    if(serialBuffer.length() >0 && maxHandle < 5)
    {
        maxHandle++;
    }
    else if(serialBuffer.length() >0 && maxHandle >= 5)
    {
        serialBuffer.clear();
        maxHandle = 0;
    }
    serialBufferLength= serialBuffer.length();
}

void debugWindow::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 0:
    {
        QJsonValue value = QJsonValue(index);
        this->father->userConfigFile->addJsonToFile("interface_connect_mode",&value);
        break;
    }
    case 1:
    {
        QJsonValue value = QJsonValue(index);
        this->father->userConfigFile->addJsonToFile("interface_connect_mode",&value);
        break;
    }
    default:
        break;
    }
}

void debugWindow::closeDmpConnect()
{
    if(ui->pushButton_runStart->text() == tr("停止连接"))
    {
        setDMPConnectStatus(0);
    }
}

void debugWindow::on_pushButton_modeCom_clicked()
{
    ui->widget_com->show();
    ui->widget_tcp->hide();
    on_tabWidget_currentChanged(0);
}

void debugWindow::on_pushButton_modeTcp_clicked()
{
    ui->widget_com->hide();
    ui->widget_tcp->show();
    on_tabWidget_currentChanged(1);
}

void debugWindow::on_pushButton_productConfig_clicked()
{
    emit showProductConfigWindow();
}

void debugWindow::setProductConfigSlot(QString pk, QString ps, QString model)
{
    if (pk == "" || ps == "" || model == "")
    {
        ui->label_pk->setText("- - -");
        ui->label_ps->setText("- - -");
        ui->label_model->setText("- - -");
    }
    else
    {
        ui->label_pk->setText(pk);
        ui->label_ps->setText(ps);
        ui->label_model->setText(model);
    }
    emit closeProductConfigWindow();
}

void debugWindow::restoreProductInformationSlot(QString pk, QString ps, QString model)
{
    if (pk == "" || ps == "" || model == "")
    {
        ui->label_pk->setText("- - -");
        ui->label_ps->setText("- - -");
        ui->label_model->setText("- - -");
    }
    else
    {
        ui->label_pk->setText(pk);
        ui->label_ps->setText(ps);
        ui->label_model->setText(model);
    }
}

void debugWindow::on_pushButton_productGet_clicked()
{
    emit productGetSignal();
}

void debugWindow::on_pushButton_productSet_clicked()
{
    emit productSetSignal();
}

void debugWindow::openLogTime(bool isCheck)
{
    if (isCheck)
    {
        ui->checkBox_logTime->setChecked(true);
    }
    else
    {
        ui->checkBox_logTime->setChecked(false);
    }
}
