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
#include "lanwindow.h"
#include "ui_lanwindow.h"
#include "kernel/mmessagebox.h"

#include "debugwindow.h"
/**************************************************************************
** 功能	@brief : wifi信息配置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
lanwindow::lanwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lanwindow)
{
    ui->setupUi(this);
    ui->lineEdit_InfoPassword_bindcode->setValidator(new QRegExpValidator(QRegExp("[a-fA-F0-9]+$")));
    ui->lineEdit_InfoPassword_bindcode->setMaxLength(16);
//    ui->comboBox_wifiFuncInfoSelect->setView(new QListView());
//    ui->comboBox_wifiFuncOperateSelect->setView(new QListView());
//    ui->comboBox_wifiFuncInfoSelect->setSizeAdjustPolicy(QComboBox::AdjustToContents);
//    QList<QComboBox *> comboBoxList = this->findChildren<QComboBox *>();
//    for (int i = 0; i < comboBoxList.count(); i ++)
//    {
//        comboBoxList.at(i)->installEventFilter(this);
//    }
}

lanwindow::~lanwindow()
{
    delete ui;
}
/**************************************************************************
** 功能	@brief :  过滤QComboBox、QSpinBox控件的滚轮信号
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool lanwindow::eventFilter(QObject *obj, QEvent *event)
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
** 功能	@brief :  设置界面属性 （注：在类实例化化后实现）
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void lanwindow::setParm(void *father)
{
    this->father = (MainWindow*)father;
    QJsonValue apData = this->father->userConfigFile->readJsonFile("wifi_ap");
    QJsonValue wifiData = this->father->userConfigFile->readJsonFile("wifi_info");
    if(!apData.isNull())
    {
        QStringList data = apData.toString().split(";");
        if(!data.isEmpty() && data.at(0) != NULL)
        {
            g_lan.g_ap.name.append(data.at(0));
        }
        if(data.count() == 2 &&  data.at(1) != NULL)
        {
            g_lan.g_ap.password.append(data.at(1));
        }
    }
    if(!wifiData.isNull())
    {
        QStringList data = wifiData.toString().split(";");
        if(!data.isEmpty() &&data.at(0) != NULL)
        {
            g_lan.g_wifi.name.append(data.at(0));
        }
        if(data.count() == 2 && data.at(1) != NULL)
        {
            g_lan.g_wifi.password.append(data.at(1));
        }
    }
    QJsonValue bindcode = this->father->userConfigFile->readJsonFile("lan_bindcode");
    if(!bindcode.isNull())
    {
        g_lan.bindcode.append(bindcode.toString());
    }
    // 蓝牙关闭功能预留，暂不可用
//    QJsonValue combox_num = this->father->userConfigFile->readJsonFile("wifi_userinfo_selectId");
//    if(!combox_num.isNull())
//    {
//        ui->comboBox_wifiFuncInfoSelect->setCurrentIndex(combox_num.toInt());
//    }
//    on_comboBox_wifiFuncInfoSelect_currentIndexChanged(ui->comboBox_wifiFuncInfoSelect->currentIndex());
}
/**************************************************************************
** 功能	@brief :  清空历史操作
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void lanwindow::resetUIValue()
{
    ui->lineEdit_InfoName_AP->clear();
    ui->lineEdit_InfoPassword_AP->clear();
    ui->lineEdit_InfoName_WIFI->clear();
    ui->lineEdit_InfoPassword_WIFI->clear();
//    ui->lineEdit_InfoName_bindcode->clear();
    ui->lineEdit_InfoPassword_bindcode->clear();
}
/**************************************************************************
** 功能	@brief :  翻译转换
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool lanwindow::retranslationUi()
{
    ui->retranslateUi(this);
    return true;
}
/**************************************************************************
** 功能	@brief :  发送蓝牙关闭AT指令
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void lanwindow::on_pushButton_close_clicked()
{
    qInfo()<<__FUNCTION__;
    MMessageBox::critical(this,tr("错误"),tr("WIFI模块暂不支持蓝牙关闭"),tr("确认"));
    //this->father->ATCMDSend("AT+QBTDEBUG=0");
}
/**************************************************************************
** 功能	@brief :  发送蓝牙开启AT指令
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void lanwindow::on_pushButton_open_clicked()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QBTDEBUG=1");
}

void lanwindow::on_pushButton_set_AP_clicked()
{
    if(ui->lineEdit_InfoName_AP->text().isEmpty())
    {
        MMessageBox::warning(this,tr("警告"),tr("请先填写信息"),tr("确认"));
    }
    else
    {
        QString name = ui->lineEdit_InfoName_AP->text();
        QString password = ui->lineEdit_InfoPassword_AP->text();
        QString data;
        QString dataJson;
        if(!name.isEmpty())
        {
            data.append("AT+QSTAAPINFO=\""+name+"\"");
            dataJson.append(name);
        }
        if(!password.isEmpty())
        {
            data.append(","+password);
            dataJson.append(";"+password);
        }
        this->father->ATCMDSend(data);
        QJsonValue value = QJsonValue(dataJson);
        this->father->userConfigFile->addJsonToFile("wifi_ap",&value);
    }
}

void lanwindow::on_pushButton_set_WIFI_clicked()
{
    if(ui->lineEdit_InfoName_WIFI->text().isEmpty()|| ui->lineEdit_InfoPassword_WIFI->text().isEmpty())
    {
        MMessageBox::warning(this,tr("警告"),tr("请先填写信息"),tr("确认"));
    }
    else
    {
        QString name = ui->lineEdit_InfoName_WIFI->text();
        QString password = ui->lineEdit_InfoPassword_WIFI->text();
        QString mac = ui->lineEdit_mac->text();
        QString data;
        QString dataJson;
        if(!name.isEmpty())
        {
            data.append("AT+QIOTCFG=\"wifi_sta\",\"" + name + "\"");
            dataJson.append(name);
        }
        if(!password.isEmpty())
        {
            dataJson.append(";"+password);
        }
        if(!mac.isEmpty())
        {
            dataJson.append(";"+mac);
        }
         data.append(",\""+password+"\""",\""+mac+"\"");
        this->father->ATCMDSend(data);
        QJsonValue value = QJsonValue(dataJson);
        this->father->userConfigFile->addJsonToFile("wifi_info",&value);
    }
}

void lanwindow::on_pushButton_set_bindcode_clicked()
{
    QString bindcode = ui->lineEdit_InfoPassword_bindcode->text();
    QString timeout = QString::number(ui->spinBox_time->value());
    if (bindcode.isEmpty())
    {
        this->father->ATCMDSend("AT+QIOTBINDCODE="+timeout);
    }
    else
    {
        this->father->ATCMDSend("AT+QIOTBINDCODE="+timeout+",\""+bindcode+"\"");
    }
    QJsonValue value = QJsonValue(bindcode);
    this->father->userConfigFile->addJsonToFile("lan_bindcode",&value);
}

void lanwindow::on_pushButton_set_clear_clicked()
{
    this->father->ATCMDSend("AT+QTESTFLASH");
}

void lanwindow::on_pushButton_set_reset_clicked()
{
    this->father->ATCMDSend("AT+QIOTRST");
}
