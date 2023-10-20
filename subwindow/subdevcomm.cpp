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
#include "subdevcomm.h"
#include "ui_subdevcomm.h"
#include <QGridLayout>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

subDevComm::subDevComm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::subDevComm)
{
    ui->setupUi(this);
    uiAttributeSet();
}

subDevComm::subDevComm(QStringList data,QWidget *parent):
    QWidget(parent),
    ui(new Ui::subDevComm)
{
    infoInit(data);
}

subDevComm::subDevComm(QStringList data, QList<deviceInfo> userInfo, QWidget *parent):
    QWidget(parent),
    ui(new Ui::subDevComm)
{
    Q_UNUSED(userInfo);
    infoInit(data);
    ((passWindow *)this->tabPass)->setFuncPermission(1);
}
/**************************************************************************
** 功能	@brief : 初始化信息
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::infoInit(QStringList data)
{
    if(data.size() == 3)
    {
        ui->setupUi(this);
        uiAttributeSet();
        /**暂时关闭**/
        if(0 == QString(data[0]).compare(tr("全部")))
        {
            hideSubWidget(2);
        }
        /**暂时关闭**/
        QString text_pk = data[0];
        ui->label_pk->setText(text_pk);
        QString text_dk = data[1];
        ui->label_dk->setText(text_dk);
        QString status;
        if(0 == QString(data[2]).toInt())
        {
            status.append(tr("离线"));
            ui->label_status->setStyleSheet("color: rgb(255, 85, 0);font-size:14px;font-weight:bold;");
        }
        else
        {
            status.append(tr("在线"));
            ui->label_status->setStyleSheet("color: rgb(83, 250, 0);font-size:14px;font-weight:bold;");
        }
        QString text_status =status;
        ui->label_status->setText(text_status);
    }
}

void subDevComm::setPassReadMaxCount(int value)
{
    ((passWindow *)this->tabPass)->setReadMaxCount(value);
}

void subDevComm::setTabbModuleReadMaxLen(int len)
{
    ((modelWindow *)this->tabModule)->setModelReadMaxLen(len);
}

bool subDevComm::retranslationUi()
{
    if(NULL != ((passWindow *)this->tabPass))
    {
        ((passWindow *)this->tabPass)->retranslationUi();
    }
    if(NULL != ((modelWindow *)this->tabModule))
    {
        ((modelWindow *)this->tabModule)->retranslationUi();
    }
    return true;
}


subDevComm::~subDevComm()
{
    delete ui;
}
/**************************************************************************
** 功能	@brief : 初始化ui界面信息
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::uiAttributeSet()
{
    QGridLayout *layoutPass = new QGridLayout(ui->tab_sudDev_passData);
    layoutPass->setContentsMargins(0,0,0,0);
    this->tabPass = new passWindow(1,ui->tab_sudDev_passData);
    QWidget *newWidget = new QWidget();
    layoutPass->addWidget(newWidget);
    layoutPass->addWidget(this->tabPass);
    ((passWindow *)this->tabPass)->hideWidgetForSubDev();
    ui->tab_sudDev_passData->setLayout(layoutPass);

    QGridLayout *layoutModule = new QGridLayout(ui->tab_sudDev_moduleData);
    layoutModule->setContentsMargins(0,0,0,0);
    this->tabModule = new modelWindow(1,ui->tab_sudDev_moduleData);
    layoutModule->addWidget(this->tabModule);
    ui->tab_sudDev_moduleData->setLayout(layoutModule);
    this->setWindowTitle(tr("子设备交互界面"));
    //去除窗体边框
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    //窗体关闭信号
    connect(ui->pushButton_close, &QPushButton::clicked, [=]() {close(); });
    connect(ui->pushButton_close_2, &QPushButton::clicked, [=]() {close(); });
    connect(ui->pushButton_Maximize, &QPushButton::clicked, [=](){
                if (this->isMaximized())
                {
                    ui->pushButton_Maximize->setStyleSheet("QPushButton:hover{background-color: rgb(220, 220, 220);}QPushButton{image: url(:/png/MaximizeOff.png);border-width:0;border-style:outset;background-color:transparent;}");
                    QVBoxLayout *layout = this->findChild<QVBoxLayout *>();


                    layout->setContentsMargins(10,10,10,10);
                    this->showNormal();
                }
                else
                {
                    ui->pushButton_Maximize->setStyleSheet("QPushButton:hover{background-color: rgb(220, 220, 220);}QPushButton{image: url(:/png/MaximizeOn.png);border-width:0;border-style:outset;background-color:transparent;}");
                    QVBoxLayout *layout = this->findChild<QVBoxLayout *>();

                    layout->setContentsMargins(0,0,0,0);
                    this->showMaximized();
                }
                });
    connect(ui->pushButton_Minimize, &QPushButton::clicked, [=]() {this->showMinimized(); });
    //关闭最大化和最小化
    ui->pushButton_Minimize->hide();
    ui->pushButton_Maximize->hide();
//    this->setWindowModality(Qt::ApplicationModal);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#C0C0C0"));
    shadow->setBlurRadius(15);
    shadow->setOffset(5);
    ui->widget->setGraphicsEffect(shadow);
    ui->scrollArea_pass->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea_pass->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea_mode->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea_mode->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    //判断是否是鼠标左键信号，是否处于最大化状态
    if(event->button() == Qt::LeftButton &&  !this->isMaximized())
    {
        if (ui->widget_windowTitle->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        {
            m_bDrag = true;
            mouseStartPoint = event->globalPos();
            windowTopLeftPoint = this->frameGeometry().topLeft();
        }
    }
}

void subDevComm::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bDrag)
    {
        //获得鼠标移动的距离
        QPoint distance = event->globalPos() - mouseStartPoint;
        //QPoint distance = event->pos() - mouseStartPoint;
        //改变窗口的位置
        this->move(windowTopLeftPoint + distance);
    }
}

void subDevComm::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

/**************************************************************************
** 功能	@brief : 获取设备pk
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QString subDevComm::getPKInfo()
{
    qDebug()<<ui->label_pk->text();
//    if(ui->label_pk->text().contains(":"))
//    {
//        return ui->label_pk->text();
//    }
    return ui->label_pk->text();
}
/**************************************************************************
** 功能	@brief : 获取设备dk
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QString subDevComm::getDKInfo()
{
    qDebug()<<ui->label_dk->text();
//    if(ui->label_dk->text().contains(":"))
//    {
//        return ui->label_dk->text().split(":").at(1);
//    }
    return ui->label_dk->text();
}
/**************************************************************************
** 功能	@brief : 获取设备信息读取的长度限制
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int subDevComm::getReadValue(int userType)
{
    if(0 == userType)
    {
        return ((passWindow*)this->tabPass)->getReadCurrentCount();
    }
    else if(1 == userType)
    {
        return ((modelWindow*)this->tabModule)->getReadCurrentCount();
    }
    return 0;
}
/**************************************************************************
** 功能	@brief : 处理透传数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::handlePassData(QList<QByteArray> paraList)
{
    qDebug()<<"paraList:"<<paraList;
    switch (paraList[2].toInt()) {
    case 4:
    {

        break;
    }
    case 5:
    {
        if(10200 == paraList[3].toInt())
        {
            if(paraList.size() == 6)
            {
                ((passWindow*)this->tabPass)->csdkPassDataIssue(paraList.at(5));
            }
            else
            {
                ((passWindow*)this->tabPass)->csdkPassDataIssue(NULL);
            }
        }
        break;
    }
    default:
        break;
    }

}

/**************************************************************************
** 功能	@brief : 处理物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::handleModuleData(QList<QByteArray> paraList)
{
    if(10210 == paraList[3].toInt())
    {
        if(paraList.size() == 6)
        {
            ((modelWindow*)this->tabModule)->csdkModelIssue(paraList.at(5));
            myObjMessServer::getObjMessServer()->notify("mainwindow_logTextEditDisplay",Q_ARG(QString,QString(paraList.at(5).toHex())),Q_ARG(bool,TEXTEDITINSERTMODE));
        }
        else
        {
            ((modelWindow*)this->tabModule)->csdkModelIssue(NULL);
        }
    }
    else if(10211 == paraList[3].toInt())
    {
        if(paraList.size() >= 5)
        {
            ((modelWindow*)this->tabModule)->csdkModelGet(paraList.at(4).toInt(),paraList.mid(5));
        }
    }

}
/**************************************************************************
** 功能	@brief : 物模型文件更新
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::tabModuleChangeJson(QString path)
{
    ((modelWindow *)this->tabModule)->modelTableUpdate(path);
}
/**************************************************************************
** 功能	@brief : 设置物模型文件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::setModuleFile(QString pathFile)
{
    if(pathFile.isEmpty())
    {
        hideModuleWidget();
    }
    else
    {
        ((modelWindow*)(this->tabModule))->modelTableUpdate(pathFile);
    }
    qDebug()<<"pathFile:"<<pathFile;
}
/**************************************************************************
** 功能	@brief : 设置子设备的透传界面的读取信息
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::setPassDataRecvInfo(QList<QByteArray> paraList, int mode)
{
    if(0 == mode)
    {
        ((passWindow *)this->tabPass)->csdkPassDataRead(paraList[3],paraList[4],paraList[5]);
    }
}
/**************************************************************************
** 功能	@brief : 设置子设备的物模型界面的读取信息
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::setModelDataRecvInfo(QList<QByteArray> paraList, int mode)
{
    if(0 == mode)
    {
        if(paraList.count() == 6)
        {
            ((modelWindow*)this->tabModule)->csdkModelRead(paraList.at(3),paraList.at(4),paraList.at(5));
        }
        else
        {
            ((modelWindow*)this->tabModule)->csdkModelRead(paraList.at(3),paraList.at(4),NULL);
        }
    }
}
/**************************************************************************
** 功能	@brief : 设置子设备的状态
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::setSubDevStatus(QString PK, QString DK, bool checked)
{
    if (DK == ui->label_dk->text() && PK == ui->label_pk->text())
    {
        if(false == checked && ui->label_status->text().contains(tr("在线")))
        {
            QString text_status = tr("离线");
            ui->label_status->setText(text_status);
            ui->label_status->setStyleSheet("color: rgb(255, 85, 0);font-size:14px;font-weight:bold;");
        }
        else if(true == checked && ui->label_status->text().contains(tr("离线")))
        {
            QString text_status = tr("在线");
            ui->label_status->setText(text_status);
            ui->label_status->setStyleSheet("color: rgb(83, 250, 0);font-size:14px;font-weight:bold;");
        }
    }
}
/**************************************************************************
** 功能	@brief : 隐藏物模型界面
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevComm::hideModuleWidget()
{
    ui->pushButton_model->hide();
    ui->tabWidget->setCurrentIndex(0);
}

void subDevComm::hideSubWidget(int index)
{
    switch (index) {
    case 0:
        ui->pushButton_pass->setEnabled(false);
        break;
    case 1:
        hideModuleWidget();
        break;
    case 2:
        ui->pushButton_pass->setEnabled(false);
        ui->pushButton_model->setEnabled(false);
        break;
    default:
        break;
    }
}

void subDevComm::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit closeWidgetEvent();
    this->tabPass->deleteLater();
    this->tabModule->deleteLater();
}

void subDevComm::on_pushButton_pass_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
}

void subDevComm::on_pushButton_model_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}
