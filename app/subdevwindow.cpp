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
#include "subdevwindow.h"
#include "debugwindow.h"
#include "ui_subDevwindow.h"
#include "third/switchbutton.h"
#include "subwindow/subdevcomm.h"
#include "kernel/mmessagebox.h"
#include "../subwindow/inputdialog.h"

/*
 *  增加添加设备时，对必要信息前加上
 */

subDevwindow::subDevwindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::subDevwindow)
{
    ui->setupUi(this);
    subWidgetIsVaild = false;
    liveListCheckTimer = new QTimer();
    liveListCheckTimer->setInterval(10000);
    connect(liveListCheckTimer,SIGNAL(timeout()),this,SLOT(liveListCheck()));
    ui->pushButton_subDevConnect->hide();
    ui->pushButton_subDevDisconnect->hide();

    searchHistoryRow = 0;
//    ui->tableWidget_subDev->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_subDev->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

enum
{
    SUB_DEVICE_SORT=0,
    SUB_DEVICE_PK=1,
    SUB_DEVICE_DK=2,
    SUB_DEVICE_LIVE=3,
    SUB_DEVICE_PS=4,
    SUB_DEVICE_DS=5,
    SUB_DEVICE_LIFETIME=6,
    SUB_DEVICE_TLS=7,
    SUB_DEVICE_STATUS=8,
    SUB_DEVICE_CONSTATUS=9,
    SUB_DEVICE_NOTE=10,
};

subDevwindow::~subDevwindow()
{
    delete ui;
}
/**************************************************************************
** 功能	@brief : 设置父和界面属性 （注：在类实例化化后实现）
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::setParm(void *father)
{
    this->father = (MainWindow*)father;
    uiAttributeSet();
}
/**************************************************************************
** 功能	@brief : 设置界面属性
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::uiAttributeSet()
{
    ui->tableWidget_subDev->setShowGrid(false);
    ui->tableWidget_subDev->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
    ui->tableWidget_subDev->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              border-right-width:0px; \
                                                              border-left-width:0px; \
                                                              }");
    ui->tableWidget_subDev->setStyleSheet("QTableWidget{ \
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

    ui->tableWidget_subDev->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_subDev->setRowCount(0); //设置行数
//    ui->tableWidget_subDev->setColumnCount(10); //设置列数
    ui->tableWidget_subDev->setWindowTitle(tr("子设备列表"));
    ui->tableWidget_subDev->setSelectionMode(QAbstractItemView::ContiguousSelection);

    ui->tableWidget_subDev->horizontalHeader()->setDefaultSectionSize(50);  //设置默认宽度
    ui->tableWidget_subDev->verticalHeader()->setDefaultSectionSize(48);   //设置一行默认高度
    ui->tableWidget_subDev->setColumnWidth(SUB_DEVICE_PK,100);
    ui->tableWidget_subDev->setColumnWidth(SUB_DEVICE_DK,150);
    ui->tableWidget_subDev->setColumnWidth(SUB_DEVICE_LIVE,140);
    ui->tableWidget_subDev->setColumnWidth(SUB_DEVICE_STATUS,120);
    ui->tableWidget_subDev->setColumnWidth(SUB_DEVICE_CONSTATUS,120);
    ui->tableWidget_subDev->setSortingEnabled(true);
    ui->tableWidget_subDev->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_subDev->setColumnHidden(SUB_DEVICE_PS, true);
    ui->tableWidget_subDev->setColumnHidden(SUB_DEVICE_DS, true);
    ui->tableWidget_subDev->setColumnHidden(SUB_DEVICE_LIFETIME, true);
    ui->tableWidget_subDev->setColumnHidden(SUB_DEVICE_TLS, true);
    ui->tableWidget_subDev->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_subDev->setSelectionMode(QAbstractItemView::ExtendedSelection);

//    connect(ui->tableWidget_subDev->horizontalHeader(),SIGNAL(sectionDoubleClicked(int)),this,SLOT(headViewDoubleClickEvent(int)));
    connect(this,SIGNAL(sendLiveData(QString,QString)),this,SLOT(handleSendLiveData(QString,QString)));
    myObjMessServer::getObjMessServer()->attach("subdevwindow_ATSendPassData",this,"ATSendPassData");
    myObjMessServer::getObjMessServer()->attach("subdevwindow_ATSendDirectlyData",this,"ATSendDirectlyData");
    myObjMessServer::getObjMessServer()->attach("subdevwindow_ATRecvPassData",this,"ATRecvPassData");
    myObjMessServer::getObjMessServer()->attach("subdevwindow_ATSendModuleData",this,"ATSendModuleData");
    myObjMessServer::getObjMessServer()->attach("subdevwindow_ATRecvModuleData",this,"ATRecvModuleData");
    myObjMessServer::getObjMessServer()->attach("subdevwindow_ATSubSendData",this,"ATSubSendData");

    // 创建sql库和表
    userInfoSql = new mySqlite();
    ((mySqlite *)userInfoSql)->createSqlDataBase("tools_sqlite","subDev_info");
    QStringList versionList = this->father->configprocess->historyRead("sqliteVersion");
    QString version;
    if(versionList.size() > 0 )
    {
        version = versionList.at(0);
    }
    else
        version = "0.0";
    if(version.toFloat() < sqliteCurVersion)
    {
        ((mySqlite *)userInfoSql)->delSqlData("DROP TABLE tools_sqlite.subDev_userInfo;");
        QStringList list;
        list.append(QString::number(sqliteCurVersion));
        this->father->configprocess->historySave("sqliteVersion",list);
    }

    ((mySqlite *)userInfoSql)->createSqlQuery("subDev_userInfo","create table subDev_userInfo (dk TEXT(32),pk TEXT(16),ps TEXT(16),buffer TEXT(1),lifetime TEXT(3),filePath TEXT(500),note TEXT(500),ds TEXT(32),tls TEXT(1),unique(dk,pk))");
    QVector<subDevConfigInfo_t>info = ((mySqlite *)userInfoSql)->findSqlAllList("select * from subDev_userInfo");
    for(int i = 0;i<info.size();i++)
    {
        QFileInfo fileInfo(info[i].filePath);
        if( (true == fileInfo.exists() && fileInfo.size() <= 0) || info[i].pk.isEmpty() || info[i].ps.isEmpty())
        {
            QString delInfo = QString("delete from subDev_userInfo where dk='%1' and pk='%2'").arg(info[i].dk).arg(info[i].pk);
            ((mySqlite *)userInfoSql)->delSqlData(delInfo);
            continue;
        }
        else
        {
            tableWidgetAddRow(info[i],1);
        }
    }
}
/**************************************************************************
** 功能	@brief : 添加子设备信息到表格中
** 输入	@param :subDevConfigInfo：物模型结构体
**              mode: 0 为创建 1为历史获取
** 输出	@retval:
***************************************************************************/
void subDevwindow::tableWidgetAddRow(subDevConfigInfo_t subDevConfigInfo,int mode)
{

    int addTabRow = ui->tableWidget_subDev->rowCount();
    ui->tableWidget_subDev->insertRow(addTabRow);

    QTableWidgetItem *check=new QTableWidgetItem(QString::number(0));
    check->setCheckState(Qt::Unchecked);  //是否设置选中框
    check->setText(" ");  //显示序号
    ui->tableWidget_subDev->setItem(addTabRow,SUB_DEVICE_SORT,check); //插入复选框

    QTableWidgetItem *itemPk = new QTableWidgetItem(subDevConfigInfo.pk);
    ui->tableWidget_subDev->setItem(addTabRow,SUB_DEVICE_PK,itemPk);

    SwitchButton *switchButton = new SwitchButton();
    switchButton->setEnabled(false);

    connect(switchButton,SIGNAL(checkedChanged(bool)),this,SLOT(switchButtonChange(bool)));
    ui->tableWidget_subDev->setCellWidget(addTabRow,SUB_DEVICE_STATUS,switchButton);

    QTableWidgetItem *itemDk = new QTableWidgetItem(subDevConfigInfo.dk);
    ui->tableWidget_subDev->setItem(addTabRow,SUB_DEVICE_DK,itemDk);

    QTableWidgetItem *itemPs = new QTableWidgetItem(subDevConfigInfo.ps);
    ui->tableWidget_subDev->setItem(addTabRow,SUB_DEVICE_PS,itemPs);

    if(!subDevConfigInfo.ds.isEmpty())
    {
        QTableWidgetItem *itemDs = new QTableWidgetItem(subDevConfigInfo.ds);
        ui->tableWidget_subDev->setItem(addTabRow,SUB_DEVICE_DS,itemDs);
    }
    else
    {
        QTableWidgetItem *itemDs = new QTableWidgetItem();
        ui->tableWidget_subDev->setItem(addTabRow,SUB_DEVICE_DS,itemDs);
    }

    QTableWidgetItem *itemLifeTime = new QTableWidgetItem(subDevConfigInfo.lifetime);
    ui->tableWidget_subDev->setItem(addTabRow,SUB_DEVICE_LIFETIME,itemLifeTime);


    QTableWidgetItem *itemTls = new QTableWidgetItem(subDevConfigInfo.tls);
    ui->tableWidget_subDev->setItem(addTabRow,SUB_DEVICE_TLS,itemTls);


    QTableWidgetItem *itemNote = new QTableWidgetItem(subDevConfigInfo.note);
    ui->tableWidget_subDev->setItem(addTabRow,SUB_DEVICE_NOTE,itemNote);


    SwitchButton *switchButtonLive = new SwitchButton();
    switchButtonLive->setEnabled(true);
    connect(switchButtonLive,SIGNAL(checkedChanged(bool)),this,SLOT(switchButtonLiveChange(bool)));
    ui->tableWidget_subDev->setCellWidget(addTabRow,SUB_DEVICE_LIVE,switchButtonLive);

    QPushButton *connectButton = new QPushButton();
    connectButton->setText(tr("连接网关"));
    connectButton->setStyleSheet("QPushButton {\
                                 color: rgb(47, 105, 235);\
                                 font-size:14px;\
                                 border-style:solid;\
                                 border-width:1px;\
                                 border-color:rgb(47, 105, 235);\
                                 border-radius:4px;\
                                 padding-left:15px;\
                                 padding-right:15px;\
                             }\
                             QPushButton:hover{\
                                 color: rgb(47, 105, 235);\
                                 background-color: rgba(47, 105, 235, 0.08);\
                             }\
                             QPushButton:pressed{\
                                 color: rgb(47, 105, 235);\
                                 background-color: rgba(47, 105, 235, 0.2);\
                             }");
//    connectButton->setObjectName(QString::number(addTabRow));
    connect(connectButton, SIGNAL(clicked()), this, SLOT(subDevConnectSlot()));
    ui->tableWidget_subDev->setCellWidget(addTabRow,SUB_DEVICE_CONSTATUS,connectButton);

    if(0 == mode)
    {
        QWidget *liveWidget =ui->tableWidget_subDev->cellWidget(addTabRow,SUB_DEVICE_LIVE);
        if(NULL == liveWidget || 0 != QString(liveWidget->metaObject()->className()).compare("SwitchButton") )
        {
            qDebug()<<"get liveWidget failed,"<<__LINE__;
            MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"), tr("确定"));
            return;
        }
        ((SwitchButton *)liveWidget)->setChecked(false);
    }
    QString destFile;
    if(0 == mode)
    {
        if(!subDevConfigInfo.filePath.isEmpty())
        {
            QStringList strList = subDevConfigInfo.filePath.split("/");
            qDebug()<<"strList:"<<strList;
            QDateTime time = QDateTime::currentDateTime();
            QString path=vitalFilePath+"subdevice_info/";
            fileManager *fM = new fileManager();
            if(false == fM->isDirExist(path))
            {
                MMessageBox::warning(this,tr("警告"),tr("文件夹创建失败"),tr("确认"));
                ui->tableWidget_subDev->removeRow(addTabRow);
                return ;
            }
            destFile = path+QString::number(time.toTime_t())+"_"+strList.constLast();
            if(!QFile::copy(subDevConfigInfo.filePath,destFile))
            {
                MMessageBox::warning(this,tr("警告"),tr("文件拷贝失败"),tr("确认"));
                ui->tableWidget_subDev->removeRow(addTabRow);
                return ;
            }
        }
        qDebug()<<__FUNCTION__<<subDevConfigInfo.pk;
        // 保存设备信息
        QString insertInfo = QString("insert into subDev_userInfo values ('%1','%2','%3','%4','%5','%6','%7','%8','%9')")
                .arg(subDevConfigInfo.dk)
                .arg(subDevConfigInfo.pk)
                .arg(subDevConfigInfo.ps)
                .arg(subDevConfigInfo.buffer)
                .arg(subDevConfigInfo.lifetime)
                .arg(destFile.isEmpty()?subDevConfigInfo.filePath:destFile)
                .arg(subDevConfigInfo.note)
                .arg(subDevConfigInfo.ds)
                .arg(subDevConfigInfo.tls);
        if(false == ((mySqlite *)userInfoSql)->insertSqlData(insertInfo))
        {
            MMessageBox *box = new MMessageBox(MMessageBox::Warning,tr("警告"),tr("历史存在此设备号,是否覆盖"),tr("是"),tr("否"));
            int ret = box->exec();
            if(ret == MMessageBox::Yes)
            {
                // 保存设备信息
                QString updateInfo = QString("update subDev_userInfo set ps='%3',buffer='%4',lifetime='%5',filePath='%6',note='%7',ds='%8',tls='%9' where dk='%1' and pk='%2'")
                        .arg(subDevConfigInfo.dk)
                        .arg(subDevConfigInfo.pk)
                        .arg(subDevConfigInfo.ps)
                        .arg(subDevConfigInfo.buffer)
                        .arg(subDevConfigInfo.lifetime)
                        .arg(subDevConfigInfo.filePath)
                        .arg(subDevConfigInfo.note)
                        .arg("")
                        .arg(subDevConfigInfo.tls);
                ((mySqlite *)userInfoSql)->modifySqlData(updateInfo);

            }
            ui->tableWidget_subDev->removeRow(addTabRow);
        }
        else
        {
            qDebug()<<"insert sqllite success";
        }
        QStringList result = ((mySqlite *)userInfoSql)->findSqlDataList("select dk from subDev_userInfo");
        qDebug()<<"findSqlDataList result:"<<result;
    }
    ui->tableWidget_subDev->update();
}
/**************************************************************************
** 功能	@brief : 子设备连接开关信号处理
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::subDevConnectSlot()
{
    if (((debugWindow*)this->father->tabDebug)->uartIsOpen || ((debugWindow*)this->father->tabDebug)->tcpIsOpen)
    {
        int row = -1;
        QList<int>rowArray = getCurrentCheckRow();
        if (rowArray.size() == 1)
        {
            row = rowArray.at(0);
        }
        if (row == -1)
        {
            return;
        }
        if (((QPushButton*)sender())->text() == tr("连接网关"))
        {
            QString sendData = "AT+QIOTSUBCONN="+ui->tableWidget_subDev->item(row,SUB_DEVICE_TLS)->text()+","+ui->tableWidget_subDev->item(row,SUB_DEVICE_LIFETIME)->text()+",\""+
                    ui->tableWidget_subDev->item(row,SUB_DEVICE_PK)->text()+"\",\""+ui->tableWidget_subDev->item(row,SUB_DEVICE_PS)->text()+"\",\""+ui->tableWidget_subDev->item(row,SUB_DEVICE_DK)->text()+"\"";
            if(!ui->tableWidget_subDev->item(row,SUB_DEVICE_DS)->text().isEmpty())
            {
                sendData.append(",\""+ui->tableWidget_subDev->item(row,SUB_DEVICE_DS)->text()+"\"");
            }
            this->father->ATCMDSend(sendData);
        }
        else if (((QPushButton*)sender())->text() == tr("断开网关"))
        {
            QString sendData = "AT+QIOTSUBDISCONN=\""+ui->tableWidget_subDev->item(row,SUB_DEVICE_PK)->text()+"\",\""+ui->tableWidget_subDev->item(row,SUB_DEVICE_DK)->text()+"\"";
            this->father->ATCMDSend(sendData);

            QWidget *widget=ui->tableWidget_subDev->cellWidget(row,3);
            if(NULL == widget || 0 != QString(widget->metaObject()->className()).compare("SwitchButton") )
            {
                qDebug()<<"get widget failed,"<<__LINE__;
                MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
                return;
            }
            ((SwitchButton *)widget)->setChecked(false);
            ((QPushButton*)sender())->setText(tr("连接网关"));
        }
    }
    else
    {
        MMessageBox::critical(0,tr("错误"),tr("请先打开串口或者TCP"),tr("确认"));
    }
}

/**************************************************************************
** 功能	@brief : 处理子设备的下发信息
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::handleSubDevEvent(QList<QByteArray> paraList)
{
    qDebug()<<__FUNCTION__<<"paraList:"<<paraList;
    switch (paraList[2].toInt())
    {
        case 1:
        {
            if(paraList[3].toInt() == 10200)// 注册
            {
                saveSubDevDs(paraList);
            }
            break;
        }
        case 2:
        {
            if(paraList[3].toInt() == 10200)// 登录
            {
                int i = 0;
                QString event_PK = paraList[0];
                QString event_DK = paraList[1];
                for(;i<ui->tableWidget_subDev->rowCount();i++)
                {
                    if(0 == ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text().compare(QString(paraList[1])) && 0 == ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text().compare(QString(paraList[0])))
                    {

                        qDebug()<<"colums:"<<ui->tableWidget_subDev->columnCount()<<","<<ui->tableWidget_subDev->horizontalHeaderItem(SUB_DEVICE_STATUS)->text();
                        QWidget *widget = ui->tableWidget_subDev->cellWidget(i,SUB_DEVICE_STATUS);
                        if(NULL == widget || 0 != QString(widget->metaObject()->className()).compare("SwitchButton") )
                        {
                            qDebug()<<"get widget failed,"<<__LINE__;
                            MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
                            return;
                        }
                        ((SwitchButton *)widget)->setChecked(true);
                        QWidget *liveWidget=ui->tableWidget_subDev->cellWidget(i,SUB_DEVICE_LIVE);
                        if(NULL == liveWidget || 0 != QString(liveWidget->metaObject()->className()).compare("SwitchButton") )
                        {
                            qDebug()<<"get liveWidget failed,"<<__LINE__;
                            MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
                            return;
                        }
                        ((SwitchButton *)liveWidget)->setChecked(true);
                        QPushButton *connectButton = (QPushButton*)ui->tableWidget_subDev->cellWidget(i, SUB_DEVICE_CONSTATUS);
                        connectButton->setText(tr("断开网关"));
                        if(!liveListCheckTimer->isActive())
                        {
                            liveListCheckTimer->start();
                        }
                    }
                    if(subWidgetIsVaild)
                    {
                        if(QString(commWidget->metaObject()->className()).contains("subDevComm"))
                        {
                            QString PK=((subDevComm *)commWidget)->getPKInfo();
                            QString DK=((subDevComm *)commWidget)->getDKInfo();
                            if(0 == PK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text()) && 0 == DK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text()))
                            {
                                qDebug()<<ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text()<<ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text();
                                if (0 == event_PK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text()) && 0 == event_DK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text()))
                                {
                                    ((subDevComm *)commWidget)->setSubDevStatus(ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text(), ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text(), true);
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
//        case 4:
//        {
//            if(paraList[3].toInt() == 10210)
//            {
//                this->father->atcmdprocess->sendDataHexFlag = false;
//            }
//             break;
//        }
        case 5:
        {
            if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
            {
                if(paraList[3].toInt() == 10200)// 收到透传数据
                {
                    ((subDevComm *)commWidget)->handlePassData(paraList);
                }
                else if(paraList[3].toInt() == 10210 || paraList[3].toInt() == 10211)// 收到物模型数据
                {
                   ((subDevComm *)commWidget)->handleModuleData(paraList);
                }
            }
            break;
        }
        case 6:
        {
                if(paraList[3].toInt() == 10200)
                {
                    int i =0;
                    QString event_PK = paraList[0];
                    QString event_DK = paraList[1];
                    for(;i<ui->tableWidget_subDev->rowCount();i++)
                    {
//                        qDebug()<<ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text();
//                        qDebug()<<QString(paraList[1]);
                        if(0 == ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text().compare(QString(paraList[1])) && 0 == ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text().compare(QString(paraList[0])))
                        {
                            QWidget *widget=ui->tableWidget_subDev->cellWidget(i,SUB_DEVICE_STATUS);
                            if(NULL == widget || 0 != QString(widget->metaObject()->className()).compare("SwitchButton") )
                            {
                                qDebug()<<"get widget failed,"<<__LINE__;
                                MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
                                return;
                            }
                            ((SwitchButton *)widget)->setChecked(false);
                            QWidget *liveWidget=ui->tableWidget_subDev->cellWidget(i,SUB_DEVICE_LIVE);
                            if(NULL == liveWidget || 0 != QString(liveWidget->metaObject()->className()).compare("SwitchButton") )
                            {
                                qDebug()<<"get widget failed,"<<__LINE__;
                                MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
                                return;
                            }
                            ((SwitchButton *)liveWidget)->setChecked(false);
                            QPushButton *connectButton = (QPushButton*)ui->tableWidget_subDev->cellWidget(i, SUB_DEVICE_CONSTATUS);
                            connectButton->setText(tr("连接网关"));
                        }
                        if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
                        {
//                            qDebug()<<"className:"<<commWidget->metaObject()->className();
                            QString PK=((subDevComm *)commWidget)->getPKInfo();
                            QString DK=((subDevComm *)commWidget)->getDKInfo();
                            if(0 == PK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text()) && 0 == DK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text()))
                            {
                                qDebug()<<ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text()<<ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text();
                                if (0 == event_PK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text()) && 0 == event_DK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text()))
                                {
                                    ((subDevComm *)commWidget)->setSubDevStatus(ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text(), ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text(), false);
                                }
                            }
                        }
                    }

                }
            break;
        }
        default:
            break;
    }


}
/**************************************************************************
** 功能	@brief : 保存子设备的DS信息
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::saveSubDevDs(QList<QByteArray> paraList)
{
    qDebug()<<"saveSubDevDs";
    for(int i = 0;i<ui->tableWidget_subDev->rowCount();i++)
    {
        qDebug()<<"ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text():"<<ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text();
        if(0 == ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text().compare(QString(paraList[1])) && 0 == ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text().compare(QString(paraList[0])))
        {
            // 设置DS
            if(paraList.size()>=5)
            {
                QTableWidgetItem *item = new QTableWidgetItem(QString(paraList[4]));
                ui->tableWidget_subDev->setItem(i,SUB_DEVICE_DS,item);
                // 更新sql数据
                QString updateInfo = QString("update subDev_userInfo set ds='%1' where dk='%2' and pk='%3'")
                        .arg(QString(paraList[4]))
                        .arg(QString(paraList[1]))
                        .arg(QString(paraList[0]));
                ((mySqlite *)userInfoSql)->modifySqlData(updateInfo);
            }
            qDebug()<<"ui->tableWidget_subDev->item(i,SUB_DEVICE_LIFETIME)->text().toInt():"<<QString::number(ui->tableWidget_subDev->item(i,SUB_DEVICE_LIFETIME)->text().toInt());
            addSendLiveDataList(QString(paraList[0]),QString(paraList[1]),ui->tableWidget_subDev->item(i,SUB_DEVICE_LIFETIME)->text().toInt(),true);
        }
    }

}
/**************************************************************************
** 功能	@brief : 子设备透传数据下发处理
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::handleSubDevPassDataRecv(QList<QByteArray> paraList)
{
    qDebug()<<"handleSubDevPassDataRecv paraList:"<<paraList;
    if(paraList.size()<6)
    {
        return;
    }
    if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
    {
        QString PK=((subDevComm *)commWidget)->getPKInfo();
        QString DK=((subDevComm *)commWidget)->getDKInfo();
        if(0 == PK.compare(QString(paraList[0])) && 0 == DK.compare(QString(paraList[1])))
        {
            ((subDevComm *)commWidget)->setPassDataRecvInfo(paraList,0);
        }
    }
}
/**************************************************************************
** 功能	@brief : 子设备物模型数据下发处理
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::handleSubDevModelDataRecv(QList<QByteArray> paraList)
{
    qDebug()<<"handleSubDevModelDataRecv:";
    if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
    {
        QString PK=((subDevComm *)commWidget)->getPKInfo();
        QString DK=((subDevComm *)commWidget)->getDKInfo();
        if(0 == PK.compare(QString(paraList[0])) && 0 == DK.compare(QString(paraList[1])))
        {

            if(paraList.size()<6)
            {
                if( 3 == paraList.size())
                {
                    qDebug()<<"handleSubDevModelDataRecv:"<<paraList;
                    QString rangeMaxString = QString(paraList.at(2)).split("-").at(1);
                    QString MaxString = rangeMaxString.split(")").at(0);
                     qDebug()<<"MaxString:"<<MaxString;
                    ((subDevComm *)commWidget)->setTabbModuleReadMaxLen(MaxString.toInt());
                }
            }
            else
            {
                ((subDevComm *)commWidget)->setModelDataRecvInfo(paraList,0);
            }
        }
    }

}
/**************************************************************************
** 功能	@brief : 获取批量子设备的数量
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QList<int> subDevwindow::getCurrentCheckRow()
{
    QList<int>checkList;
    for(int i =0;i<ui->tableWidget_subDev->rowCount();i++)
    {
        if( Qt::Checked == ui->tableWidget_subDev->item(i,SUB_DEVICE_SORT)->checkState())
        {
            checkList.append(i);
        }
    }
    if(0 == checkList.size())
    {
        modelProcess *process = new modelProcess();
        checkList = process->getListSelectedRows(ui->tableWidget_subDev);
        if(0 == checkList.size())
        {
            MMessageBox::information(this,tr("提示"),tr("请选择需要操作的行"),tr("确认"));
        }
        delete process;
    }
    return checkList;
}


/**************************************************************************
** 功能	@brief : 增加子设备
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::on_pushButton_addSubDev_clicked()
{
    if(subWidgetIsVaild == false)
    {
        subDevCreate *subDevCreateWidget = new subDevCreate();
        subDevCreateWidget->setAttribute(Qt::WA_QuitOnClose,true);
        connect(subDevCreateWidget,SIGNAL(sendSubDevCreateInfo(subDevConfigInfo_t)),this,SLOT(recvSubDevCreateInfo(subDevConfigInfo_t)));
        connect(subDevCreateWidget,SIGNAL(closeWidgetEvent()),this,SLOT(closeSubWidget()));
        subDevCreateWidget->show();
        createWidget=subDevCreateWidget;
        subWidgetIsVaild = true;
    }
    else
    {
        MMessageBox *box = new MMessageBox(MMessageBox::Warning,tr("警告"),tr("当前子窗口未关闭"),tr("确认"));
        QTimer::singleShot(3000,box,SLOT(accept())); //也可将accept改为close
        box->exec();
    }
}

void subDevwindow::recvSubDevCreateInfo(subDevConfigInfo_t subDevConfigInfo)
{
    bool isRepeat = false;
    for(int i =0;i<ui->tableWidget_subDev->rowCount();i++)
    {
        if(0 == ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text().compare(subDevConfigInfo.pk) && 0 == ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text().compare(subDevConfigInfo.dk))
        {
            MMessageBox::critical(this,tr("错误"),tr("不能添加重复的产品号"),tr("确认"));
            isRepeat = true;
            break;
        }
    }
    if(!isRepeat)
    {
        tableWidgetAddRow(subDevConfigInfo,0);
        if(NULL != createWidget)
        {
            createWidget->close();
            createWidget = NULL;
        }
    }

}

/**************************************************************************
** 功能	@brief : 批量删除子设备
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::on_pushButton_delSubDev_clicked()
{
    QList<int>selectRowNum = getCurrentCheckRow();
    if(selectRowNum.size()!= 0)
    {
        MMessageBox *box = new MMessageBox(MMessageBox::Information,tr("提示"),tr("是否删除子设备"),tr("是"),tr("否"));
        if (box->exec() != MMessageBox::Yes)
        {
            return;
        }
        for(int i =selectRowNum.count()-1;i>=0;i--)
        {
           qDebug()<<"selectRowNum[i]:"<<selectRowNum[i];
           if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
            {
               QString PK=((subDevComm *)commWidget)->getPKInfo();
               QString DK=((subDevComm *)commWidget)->getDKInfo();
               if(0 == ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PK)->text().compare(PK) && 0 == ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DK)->text().compare(DK))
               {
                   MMessageBox::critical(this,tr("错误"),tr("无法删除当前正在运行的设备"),tr("确认"));
                   continue;
               }
            }
           
            // 这里尚未做注销前判断是否已连接网关
			if(!ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DS)->text().isEmpty())
            {
                QString sendData = "AT+QIOTSUBDISCONN=\""+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PK)->text()+"\",\""+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DK)->text()
                    +"\",\""+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PS)->text()+"\",\""+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DS)->text()+"\"";
                if (((debugWindow*)this->father->tabDebug)->uartIsOpen || ((debugWindow*)this->father->tabDebug)->tcpIsOpen)
                {
                    this->father->ATCMDSend(sendData);
                }
            }
            // 删除心跳
            delLiveList(ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PK)->text(),ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DK)->text());

            // 删除配置文件
            QString findString = QString("select filePath from subDev_userInfo where dk='%1' and pk='%2'").arg(ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DK)->text())
                    .arg(ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PK)->text());
            QString filePath = ((mySqlite *)userInfoSql)->findSqlDataString(findString);
            if(!filePath.isEmpty())
            {
                QFileInfo jsonFile(filePath);
                if(jsonFile.isFile())
                {
                    qDebug()<<"delete file";
                    QFile::remove(filePath);
                }
            }
            // 删除sql 数据
            QString delInfo = QString("delete from subDev_userInfo where dk='%1' and pk='%2'").arg(ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DK)->text())
                                                                                              .arg(ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PK)->text());
            ((mySqlite *)userInfoSql)->delSqlData(delInfo);
            ui->tableWidget_subDev->removeRow(selectRowNum[i]);
        }
    }
}
/**************************************************************************
** 功能	@brief : 栏目双击打开当前行的子设备交互界面
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::on_tableWidget_subDev_cellDoubleClicked(int row, int column)
{
    if(column <= 0)
    {
        return;
    }
    if(column == SUB_DEVICE_NOTE)
    {
        QString oldText;
        if(ui->tableWidget_subDev->item(row,column) != NULL && !ui->tableWidget_subDev->item(row,column)->text().isEmpty())
        {
            oldText.append(ui->tableWidget_subDev->item(row,column)->text());
        }
        bool ok;

//        QString text = QInputDialog::getText(this, tr("备注"),tr("请输入内容"), QLineEdit::Normal,oldText, &ok,Qt::WindowCloseButtonHint);
        QString text = InputDialog::getText(tr("备注"),oldText, &ok, tr("确定"), tr("取消"));
        if (ok)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem();
            newItem->setText(text);
            ui->tableWidget_subDev->setItem(row,column,newItem);
            QString updateInfo = QString("update subDev_userInfo set note='%2' where dk='%1'")
                    .arg(ui->tableWidget_subDev->item(row,SUB_DEVICE_DK)->text())
                    .arg(text);
            ((mySqlite *)userInfoSql)->modifySqlData(updateInfo);
        }
    }
    else
    {
        QStringList data;
        data.append(ui->tableWidget_subDev->item(row,SUB_DEVICE_PK)->text());
        data.append(ui->tableWidget_subDev->item(row,SUB_DEVICE_DK)->text());
        QWidget *widget=ui->tableWidget_subDev->cellWidget(ui->tableWidget_subDev->currentRow(),SUB_DEVICE_STATUS);
        if(NULL == widget || 0 != QString(widget->metaObject()->className()).compare("SwitchButton") )
        {
            qDebug()<<"get widget failed,"<<__LINE__;
            MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
            return;
        }
        bool value = ((SwitchButton *)widget)->getChecked();
        data.append(QString::number(value));
        if(subWidgetIsVaild == false)
        {
            if(false == value )
            {
                MMessageBox *box = new MMessageBox(MMessageBox::Warning,tr("警告"),tr("当前设备处于离线状态，请先连接"),tr("确认"));
//                QTimer::singleShot(3000,box,SLOT(accept())); //也可将accept改为close
                box->exec();
                return;
            }
            subDevComm *subDevCommWidget = new subDevComm(data);
            subDevCommWidget->setAttribute(Qt::WA_QuitOnClose,false);
            connect(subDevCommWidget,SIGNAL(closeWidgetEvent()),this,SLOT(closeSubWidget()));
            QString word =QString("select filePath from subDev_userInfo where dk='%1'and pk='%2';").arg(data[1]).arg(data[0]);
            QString filePath = ((mySqlite *)userInfoSql)->findSqlDataString(word);
            subDevCommWidget->setModuleFile(filePath);
    //        subDevCommWidget->tabModuleChangeJson(filePath);
            subDevCommWidget->show();
            commWidget = subDevCommWidget;
            subWidgetIsVaild = true;
        }
        else
        {
            MMessageBox *box = new MMessageBox(MMessageBox::Warning,tr("警告"),tr("当前子窗口未关闭"),tr("确认"));
            QTimer::singleShot(3000,box,SLOT(accept())); //也可将accept改为close
            box->exec();
        }
    }


}
/**************************************************************************
** 功能	@brief : 批量连接
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::on_pushButton_subDevConnect_clicked()
{
    QList<int>selectRowNum = getCurrentCheckRow();
    for(int i =selectRowNum.count()-1;i>=0;i--)
    {
        // 这里需要加子设备记录保存,下次开启就显示子设备列表且为登录非注册形式连接网关
        // 是否需要和网关绑定
        QString sendData = "AT+QIOTSUBCONN="+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_TLS)->text()+","+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_LIFETIME)->text()+",\""+
                ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PK)->text()+"\",\""+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PS)->text()+"\",\""+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DK)->text()+"\"";
        if(!ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DS)->text().isEmpty())
        {
            sendData.append(",\""+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DS)->text()+"\"");
        }
        this->father->ATCMDSend(sendData);
     }
}
/**************************************************************************
** 功能	@brief : 批量断开连接
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::on_pushButton_subDevDisconnect_clicked()
{
    QList<int>selectRowNum = getCurrentCheckRow();
    if(selectRowNum.size()!= 0)
    {
        for(int i =selectRowNum.count()-1;i>=0;i--)
        {

            QString sendData = "AT+QIOTSUBDISCONN=\""+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PK)->text()+"\",\""+ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DK)->text()+"\"";
            this->father->ATCMDSend(sendData);

             QWidget *widget=ui->tableWidget_subDev->cellWidget(selectRowNum[i],3);
             if(NULL == widget || 0 != QString(widget->metaObject()->className()).compare("SwitchButton") )
             {
                 qDebug()<<"get widget failed,"<<__LINE__;
                 MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
                 return;
             }
             ((SwitchButton *)widget)->setChecked(false);
        }
    }
}

/**************************************************************************
** 功能	@brief : 表头双击可打开批量界面，有瑕疵
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::headViewDoubleClickEvent(int idex)
{
    Q_UNUSED(idex);
    // 检测选择的子设备列表的合法性
    QList<int>rowArray = getCurrentCheckRow();
    if(rowArray.size() == 1)
    {
        on_tableWidget_subDev_cellDoubleClicked(rowArray.at(0),SUB_DEVICE_PK);
    }
    else if(rowArray.size() > 1)
    {
        MMessageBox::information(this,tr("提示"),tr("设备交互不支持批量处理，请选择其中一个子设备进行交互"),tr("确认"));
//        QList<deviceInfo>usersInfo;
//        for(int i =rowArray.count()-1;i>=0;i--)
//        {
//            QWidget *widget = ui->tableWidget_subDev->cellWidget(rowArray[i],SUB_DEVICE_STATUS);
//            if(true != ((SwitchButton *)widget)->getChecked())
//            {
//                MMessageBox::warning(this,tr("警告"),tr("选择的批量设备中存在未连接网关的子设备"),tr("确认"));
//                return;
//            }
//            else
//            {
//                deviceInfo devInfo;
//                devInfo.PK = ui->tableWidget_subDev->item(rowArray[i],SUB_DEVICE_PK)->text();
//                devInfo.DK = ui->tableWidget_subDev->item(rowArray[i],SUB_DEVICE_DK)->text();
//                usersInfo.append(devInfo);
//            }
//        }
//        if(0 == usersInfo.size())
//        {
//            return;
//        }
//        QStringList data;
//        data.append(tr("全部"));
//        data.append(tr("全部"));
//        data.append(QString::number(1));
//        if(subWidgetIsVaild == false)
//        {
//            subDevComm *subDevCommWidget = new subDevComm(data,usersInfo);
//            subDevCommWidget->setAttribute(Qt::WA_QuitOnClose,true);
//            connect(subDevCommWidget,SIGNAL(closeWidgetEvent()),this,SLOT(closeSubWidget()));
//            subDevCommWidget->show();
//            commWidget = subDevCommWidget;
//            subWidgetIsVaild = true;
//        }
//        else
//        {
//            MMessageBox *box = new MMessageBox(MMessageBox::Warning,tr("警告"),tr("当前子窗口未关闭"),tr("确认"));
//            QTimer::singleShot(3000,box,SLOT(accept())); //也可将accept改为close
//            box->exec();
//        }
    }
}
/**************************************************************************
** 功能	@brief : 关闭子设备界面
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::closeSubWidget()
{
    qDebug()<<"destory widget";
    subWidgetIsVaild = false;
    //commWidget->setAttribute(Qt::WA_DeleteOnClose,true);
    if(commWidget != NULL)
    {
        commWidget->close();
        commWidget = NULL;
    }
}
/**************************************************************************
** 功能	@brief : 发送子设备心跳包
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::handleSendLiveData(QString pk, QString dk)
{
    qDebug()<<"\n\n"<<QString("AT+QIOTSUBHTB=\""+pk+"\",\""+dk+"\"")<<"\n\n";
    myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QString,"AT+QIOTSUBHTB=\""+pk+"\",\""+dk+"\""));
}
/**************************************************************************
** 功能	@brief : 发送子设备透传数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::ATSendPassData(QByteArray data)
{
    qDebug()<<"send data";
    QString PK=((subDevComm *)commWidget)->getPKInfo();
    QString DK=((subDevComm *)commWidget)->getDKInfo();
    QString sendATcmd = "AT+QIOTSUBSEND=\""+PK+"\",\""+DK+"\","+QString::number(data.length());
    this->father->ATCMDSend(sendATcmd, data);
}
/**************************************************************************
** 功能	@brief : 发送子设备透传数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::ATSendDirectlyData(QByteArray data, bool externFlag)
{
    QString PK=((subDevComm *)commWidget)->getPKInfo();
    QString DK=((subDevComm *)commWidget)->getDKInfo();
    QByteArray sendData;
    sendData.append(QString("AT+QIOTSUBSEND=\""+PK+"\",\""+DK+"\","+QString::number(data.length())+",\"").toUtf8()+data+QString("\"").toUtf8());
    this->father->ATCMDSend(sendData,externFlag);
}
/**************************************************************************
** 功能	@brief : 读取子设备透传模式的缓存数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::ATRecvPassData(int value)
{
    qDebug()<<"recv pass data";
    if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
    {
        QString PK=((subDevComm *)commWidget)->getPKInfo();
        QString DK=((subDevComm *)commWidget)->getDKInfo();
        QString sendData;
        if(0 == value)
        {
            value = 512;
        }
        sendData.append("AT+QIOTSUBRD=\""+PK+"\",\""+DK+"\","+QString::number(value));
        this->father->ATCMDSend(sendData);
    }
}
/**************************************************************************
** 功能	@brief : 发送子设备物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::ATSendModuleData(QByteArray data)
{
    qDebug()<<"send module data";
    if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
    {
        QString PK=((subDevComm *)commWidget)->getPKInfo();
        QString DK=((subDevComm *)commWidget)->getDKInfo();
        QString sendData;
        sendData.append("AT+QIOTSUBTSLTD=\""+PK+"\",\""+DK+"\","+QString::number(data.length()));
        this->father->ATCMDSend(sendData,data);
//        this->father->setrecvSerialSendMutex(true);
    }
}
/**************************************************************************
** 功能	@brief : 发送子设备物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::ATSendModuleData(QByteArray data, int value)
{
    if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
    {
        QString PK=((subDevComm *)commWidget)->getPKInfo();
        QString DK=((subDevComm *)commWidget)->getDKInfo();
        QString sendData;
        sendData.append("AT+QIOTSUBTSLTD=\""+PK+"\",\""+DK+"\","+QString::number(data.length())+","+QString::number(value));
        this->father->ATCMDSend(sendData,data);
//        this->father->setrecvSerialSendMutex(true);
    }
}
/**************************************************************************
** 功能	@brief : 读取子设备物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::ATRecvModuleData(int mode)
{


    if(mode == 0)
    {
        if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
        {
            this->father->ATCMDSend("AT+QIOTSUBTSLRD=?");
        }
    }
    else
    {
        if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
        {
            QString PK=((subDevComm *)commWidget)->getPKInfo();
            QString DK=((subDevComm *)commWidget)->getDKInfo();
            QString sendData;
            int value = ((subDevComm *)commWidget)->getReadValue(1);
            if(0 == value)
            {
                value = 512;
            }
            sendData.append("AT+QIOTSUBTSLRD=\""+PK+"\",\""+DK+"\","+QString::number(value));
            this->father->ATCMDSend(sendData);
        }
    }
}
/**************************************************************************
** 功能	@brief : 发送子设备数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::ATSubSendData(QByteArray data,bool externFlag)
{
    if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
    {
        this->father->ATCMDSend(data,externFlag);
    }
}
/**************************************************************************
** 功能	@brief : 增加新设备心跳任务到链表
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::addSendLiveDataList(QString pk, QString dk, int lifeTime, bool status)
{
    liveSendDataStruct dataStruct;
    dataStruct.PK = pk;
    dataStruct.DK = dk;
    dataStruct.currentLifeTime = 0;
    dataStruct.lifeTime = lifeTime-20;
    qDebug()<<"dataStruct.lifeTime:"<<dataStruct.lifeTime;
    dataStruct.status = status;
//    for (int i = 0; i < sendLiveList.count(); i ++)
//    {
//        if (sendLiveList[i].DK != dataStruct.DK ||  sendLiveList[i].PK != dataStruct.PK)
//        {
//            sendLiveList.append(dataStruct);
//        }
//    }
    sendLiveList.append(dataStruct);
}
/**************************************************************************
** 功能	@brief : 从链表中删除设备心跳任务
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::delLiveList(QString pk,QString dk)
{
    if(liveMutex.tryLock(50))
    {
        for (int i = sendLiveList.size(); i > 0;i--)
        {
            if(0 == sendLiveList[i-1].DK.compare(dk) && 0 == sendLiveList[i-1].PK.compare(pk))
            {
                sendLiveList.removeAt(i-1);
            }
        }
        liveMutex.unlock();
    }
    else
    {
        MMessageBox::critical(this,tr("错误"),tr("无法删除设备心跳，请重新删除"),tr("确认"));
    }
}
/**************************************************************************
** 功能	@brief : 设置读取透传模式的数据最大长度
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::setPassReadMaxCount(int value)
{
    if(subWidgetIsVaild && QString(commWidget->metaObject()->className()).contains("subDevComm"))
    {
        ((subDevComm *)commWidget)->setPassReadMaxCount(value);
    }
}
/**************************************************************************
** 功能	@brief : 关闭心跳发送定时器
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::closeLiveTimer()
{
    qDebug()<<"closeLiveTimer";
    if(liveListCheckTimer->isActive())
    {
        // 查询当前子设备连接界面
        for(int i =0;i<ui->tableWidget_subDev->rowCount();i++)
        {
            QWidget *item=(ui->tableWidget_subDev->cellWidget(i,SUB_DEVICE_STATUS));
            if(NULL == item || 0 != QString(item->metaObject()->className()).compare("SwitchButton") )
            {
                qDebug()<<"get itemLive failed,"<<__LINE__;
                MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
                return;
            }
            if(((SwitchButton *)item)->getChecked())
            {
                QString sendData = "AT+QIOTSUBDISCONN=\""+ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text()+"\",\""+ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text()+"\"";
                if (((debugWindow*)this->father->tabDebug)->uartIsOpen || ((debugWindow*)this->father->tabDebug)->tcpIsOpen)
                {
                    this->father->ATCMDSend(sendData);
                }
                // 不保证联动性 不等待结果返回
                ((SwitchButton *)item)->setChecked(false);
                QWidget *itemLive=(ui->tableWidget_subDev->cellWidget(i,SUB_DEVICE_LIVE));
                if(NULL == itemLive || 0 != QString(itemLive->metaObject()->className()).compare("SwitchButton") )
                {
                    qDebug()<<"get itemLive failed,"<<__LINE__;
                    MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
                    return;
                }
                ((SwitchButton *)itemLive)->setChecked(false);
            }
            else
            {
                qDebug()<<"false";
            }
        }
        liveListCheckTimer->stop();
        sendLiveList.clear();
    }
}
/**************************************************************************
** 功能	@brief : 翻译转换
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool subDevwindow::retranslationUi()
{
    if(NULL != commWidget)
    {
//        MMessageBox::critical(this,tr("错误"),tr("请先关闭子设备交互界面再切换语言"),tr("确认"));
//        return false;
        QString PK=((subDevComm *)commWidget)->getPKInfo();
        QString DK=((subDevComm *)commWidget)->getDKInfo();
        closeSubWidget();
        for(int i =0;i< ui->tableWidget_subDev->rowCount();i++)
        {
            if(0 == PK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text()) && (0 == DK.compare(ui->tableWidget_subDev->item(i,SUB_DEVICE_DK)->text())))
            {
                on_tableWidget_subDev_cellDoubleClicked(i,SUB_DEVICE_PK);
                break;
            }
        }


    }
    if(NULL != createWidget)
    {
        ((subDevCreate *)createWidget)->retranslationUi();
    }
    ui->retranslateUi(this);
    int rowCount = ui->tableWidget_subDev->rowCount();
    for (int i = 0; i < rowCount; i ++)
    {
        if ((QPushButton *)ui->tableWidget_subDev->cellWidget(i, SUB_DEVICE_CONSTATUS) != NULL)
        {
            QPushButton *button = (QPushButton *)ui->tableWidget_subDev->cellWidget(i, SUB_DEVICE_CONSTATUS);
            if (button->text() == "连接网关" || button->text() == "Connect")
            {
                button->setText(tr("连接网关"));
            }
            else if (button->text() == "断开网关" || button->text() == "Disonnect")
            {
                button->setText(tr("断开网关"));
            }
        }
    }
    return true;
}

/**************************************************************************
** 功能	@brief : 心跳定时器检测列表
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::liveListCheck()
{
    qDebug()<<"sendLiveList.size():"<<sendLiveList.size();
    if(liveMutex.tryLock(50))
    {
        if(!this->father->userconfigStatus(GETUSERINFO,SOCKETSTATUS) && sendLiveList.size() > 0)
        {
            sendLiveList.clear();
            this->father->logTextEditDisplay(tr("当前网关已断开连接，已清除子设备心跳"),TEXTEDITAPPENDMODE);
            closeLiveTimer();
        }
        if(sendLiveList.size() == 0)
        {
            liveListCheckTimer->stop();
        }
        for (int i = sendLiveList.size(); i > 0;i--)
        {
            sendLiveList[i-1].currentLifeTime += 10;
            if(false == sendLiveList[i-1].status)
            {
                sendLiveList.removeAt(i-1);
            }
            else if(sendLiveList[i-1].currentLifeTime >= sendLiveList[i-1].lifeTime)
            {
                emit sendLiveData(sendLiveList[i-1].PK,sendLiveList[i-1].DK);
                sendLiveList[i-1].currentLifeTime = 0;
            }
        }
        liveMutex.unlock();
    }
    else
    {
        MMessageBox::critical(this,tr("错误"),tr("无法删除设备心跳，请重新删除"),tr("确认"));
    }
}



/**************************************************************************
** 功能	@brief : 处理当前心跳栏目的变动
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::switchButtonLiveChange(bool checked)
{
    SwitchButton *sigBtn = qobject_cast<SwitchButton*>(sender());
    QModelIndex modelIndex = ui->tableWidget_subDev->indexAt(sigBtn->pos());
    int row = modelIndex.row();
    QString dk = ui->tableWidget_subDev->item(row,SUB_DEVICE_DK)->text();
    QString pk = ui->tableWidget_subDev->item(row,SUB_DEVICE_PK)->text();
    bool changeFlag = false;
    for(int i = 0;i<sendLiveList.size();i++)
    {
        if(0 == pk.compare(sendLiveList[i].PK) && 0 == dk.compare(sendLiveList[i].DK) && true == sendLiveList[i].status)
        {
            sendLiveList[i].status = checked;
            changeFlag = true;
            break;
        }
    }
    qDebug()<<"dk:"<<dk;
    qDebug()<<"row:"<<row;
    if(false == changeFlag && true == checked)
    {
        QWidget *item=(ui->tableWidget_subDev->cellWidget(row,SUB_DEVICE_STATUS));
        if(NULL == item || 0 != QString(item->metaObject()->className()).compare("SwitchButton") )
        {
            qDebug()<<"get item failed,"<<__LINE__;
            MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
            return;
        }
        if(((SwitchButton *)item)->getChecked() == true)
        {
            qDebug()<<"ui->tableWidget_subDev->item(row,SUB_DEVICE_LIFETIME)->text().toInt():"<<ui->tableWidget_subDev->item(row,SUB_DEVICE_LIFETIME)->text().toInt();
            addSendLiveDataList(ui->tableWidget_subDev->item(row,SUB_DEVICE_PK)->text(),ui->tableWidget_subDev->item(row,SUB_DEVICE_DK)->text(),ui->tableWidget_subDev->item(row,SUB_DEVICE_LIFETIME)->text().toInt(),true);
        }
        else
        {
            this->father->statusBarShow(tr("请先连接网关"));
            QWidget *widget=(ui->tableWidget_subDev->cellWidget(row,SUB_DEVICE_LIVE));
            if(NULL == widget || 0 != QString(widget->metaObject()->className()).compare("SwitchButton") )
            {
                qDebug()<<"get widget failed,"<<__LINE__;
                MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确定"));
                return;
            }
            ((SwitchButton *)widget)->setChecked(false);
        }
    }

}
/**************************************************************************
** 功能	@brief : 心跳的栏目有变动则会触发此函数，初始为关闭
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::switchButtonChange(bool checked)
{
    if(true == checked)
    {
        if(!liveListCheckTimer->isActive())
        {
            liveListCheckTimer->start();
        }
    }
}
/**************************************************************************
** 功能	@brief : 遍历查找，从当前位置开始到前一个位置结束
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::on_pushButton_findSubDev_clicked()
{
    bool ok;
    bool findResult = false;

//    QString text = QInputDialog::getText(this, tr("搜索"),tr("请输入内容"), QLineEdit::Normal,NULL, &ok,Qt::WindowCloseButtonHint);
    QString text = InputDialog::getText2(tr("请输入内容"), &ok, tr("确定"), tr("取消"));
    if(text.isEmpty())
    {
        this->father->statusBarShow(tr("搜索的内容不能为空"));
        return;
    }
    int searchRangMax = ui->tableWidget_subDev->rowCount();
    bool loopSearch = false;
    if(ok)
    {
        for(int i = searchHistoryRow+1;i<searchRangMax;i++)
        {
            if(ui->tableWidget_subDev->item(i,SUB_DEVICE_PK)->text().contains(text,Qt::CaseInsensitive))
            {
                ui->tableWidget_subDev->setCurrentCell(i,1);
                ui->tableWidget_subDev->selectRow(i);
                findResult = true;
                searchHistoryRow = i;
                break;
            }
            if(searchRangMax == i+1 && false == loopSearch)
            {
                i = 0;
                searchRangMax = searchHistoryRow+1;
                loopSearch = true;
            }
        }
    }
    if(false == findResult)
    {
        this->father->statusBarShow(tr("未能找到相应产品号"),5*60);
        searchHistoryRow = -1;
        ui->tableWidget_subDev->setCurrentCell(0,1);
        ui->tableWidget_subDev->selectRow(0);
    }
}
/**************************************************************************
** 功能	@brief : 子设备交互界面弹出
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::on_pushButton_subDevCommunite_clicked()
{
    headViewDoubleClickEvent(0);
}
/**************************************************************************
** 功能	@brief : 清除子设备的ds
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevwindow::on_pushButton_clearDs_clicked()
{
    QList<int>selectRowNum = getCurrentCheckRow();
    if(selectRowNum.size()!= 0)
    {
        MMessageBox *box = new MMessageBox(MMessageBox::Information,tr("提示"),tr("确认是否删除"),tr("是"),tr("否"));
        if(box->exec() == MMessageBox::No)
            return;
        for(int i =selectRowNum.count()-1;i>=0;i--)
        {
            QWidget *widget = ui->tableWidget_subDev->cellWidget(selectRowNum[i],SUB_DEVICE_STATUS);
            if(NULL == widget || 0 != QString(widget->metaObject()->className()).compare("SwitchButton") )
            {
                qDebug()<<"get widget failed,"<<__LINE__;
                MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确认"));
                return;
            }
            if(!((SwitchButton *)widget)->getChecked())
            {
                 ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DS)->setText("");
                 // 删除sql 数据
                 QString delInfo = QString("update subDev_userInfo set ds='%1' where dk='%2' and pk='%3'").arg("").arg(ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_DK)->text())
                                                                                                   .arg(ui->tableWidget_subDev->item(selectRowNum[i],SUB_DEVICE_PK)->text());
                 ((mySqlite *)userInfoSql)->modifySqlData(delInfo);
                 QWidget *liveWidget=ui->tableWidget_subDev->cellWidget(selectRowNum[i],SUB_DEVICE_LIVE);
                 if(NULL == liveWidget || 0 != QString(liveWidget->metaObject()->className()).compare("SwitchButton") )
                 {
                     qDebug()<<"get liveWidget failed,"<<__LINE__;
                     MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确认"));
                     return;
                 }
                 ((SwitchButton *)liveWidget)->setChecked(false);
                 QWidget *statusWidget=ui->tableWidget_subDev->cellWidget(selectRowNum[i],SUB_DEVICE_STATUS);
                 if(NULL == statusWidget || 0 != QString(statusWidget->metaObject()->className()).compare("SwitchButton") )
                 {
                     qDebug()<<"get statusWidget failed,"<<__LINE__;
                     MMessageBox::critical(this,tr("错误"),tr("无法正确获取到组件信息"),tr("确认"));
                     return;
                 }
                 ((SwitchButton *)statusWidget)->setChecked(false);
            }
            else
            {
                MMessageBox::critical(this,tr("错误"),tr("请先断开与网关的连接，再清空"),tr("确认"));
            }
        }
    }
}
