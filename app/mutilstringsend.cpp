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
#include "mutilstringsend.h"
#include "ui_mutilstringsend.h"
#include "baseQTConfigure.h"
#include "userconfigure.h"
#include "third/editpushbutton.h"
#include "kernel/filemanager.h"
#include "kernel/mysqlite.h"
#include "kernel/userconfigsave.h"
#include "kernel/mmessagebox.h"
#include "../subwindow/inputdialog.h"

#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>


mutilStringSend::mutilStringSend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mutilStringSend)
{
    ui->setupUi(this);
    uiAttributeSet();
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabWidget_currentChanged(int)));
    initSqlite();
    QRect rect = appRect;
    this->setGeometry(rect.right(),rect.top(),this->width(),rect.height());
    //去除窗体边框
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    //窗体关闭信号
    connect(ui->pushButton_close, &QPushButton::clicked, [=]() {close(); });
//    this->setWindowModality(Qt::ApplicationModal);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#C0C0C0"));
    shadow->setBlurRadius(15);
    shadow->setOffset(5);
    ui->widget->setGraphicsEffect(shadow);
    ui->scrollArea_stringShow->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea_stringShow->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

mutilStringSend::~mutilStringSend()
{
    delete ui;
}
/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::mousePressEvent(QMouseEvent *event)
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

void mutilStringSend::mouseMoveEvent(QMouseEvent *event)
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

void mutilStringSend::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

bool mutilStringSend::retranslationUi()
{
    ui->retranslateUi(this);
    uiAttributeSet();
    return true;
}
/**************************************************************************
** 功能	@brief :  初始化数据库
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::initSqlite()
{

    // 创建sql库和表
    userInfoSql = new mySqlite();
    ((mySqlite *)userInfoSql)->createSqlDataBase("mutilString_sqlite","normal_info");


    // 获取/新建表名
    ((mySqlite *)userInfoSql)->createSqlQuery("mutilString_tabName","create table mutilString_tabName (sortId TEXT(1) PRIMARY KEY,tabName TEXT(150))");
    QStringList sqlTabNameList = ((mySqlite *)userInfoSql)->execSqlDataList("SELECT tabName FROM mutilString_tabName;");
    if(sqlTabNameList.count()>0)
    {
        disconnect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabWidget_currentChanged(int)));
        if(ui->tabWidget->count() > 0)
        {
            ui->tabWidget->clear();
        }
        QStringList tabNameList = QString(sqlTabNameList[0]).split(";");
        // 创建历史Tab
        for(int i = 0;i<tabNameList.count()-1;i++)
        {
            QWidget *widget = new QWidget();
            ui->tabWidget->addTab(widget,QString(tabNameList.at(i)));
        }
        if(0 != ui->tabWidget->tabText(ui->tabWidget->count()-1).compare("+"))
        {
            QWidget *widget = new QWidget();
            ui->tabWidget->addTab(widget,QString("+"));
        }
        connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabWidget_currentChanged(int)));
    }
    else
    {
        QString nameList;
        for(int i =0;i<ui->tabWidget->count();i++)
        {
            nameList.append(ui->tabWidget->tabText(i)+";");
        }
        nameList.chop(1);
        if(!((mySqlite *)userInfoSql)->insertSqlData(QString("insert into mutilString_tabName values ('%1','%2')").arg(0).arg(nameList)))
        {
            ((mySqlite *)userInfoSql)->modifySqlData(QString("update mutilString_tabName set tabName='%1'").arg(nameList));
        }
    }
    // 获取数据
    ((mySqlite *)userInfoSql)->createSqlQuery("mutilString_userInfo","create table mutilString_userInfo (sortId INT(1) PRIMARY KEY,data TEXT(150),note TEXT(32),dataType TEXT(1))");
    QStringList countList = ((mySqlite *)userInfoSql)->execSqlDataList("SELECT count(sortId) AS number FROM mutilString_userInfo;");
    int countSum = QString(countList.at(0)).toInt();
    int pageSum = countSum/100;
    QList<QStringList> list ;
    if(pageSum > 0)
    {
        list = ((mySqlite *)userInfoSql)->findSqlDataListEx(QString("select * from mutilString_userInfo limit %1,%2").arg(0).arg(currentLineCounts));
    }
    QGridLayout *dstLayout;
    QList<QObject *>objList =ui->scrollAreaWidgetContents->findChildren<QObject *>();
    for(int i = objList.count()-1;i>=0;i--)
    {
        delete objList.at(i);
    }

    if(NULL ==ui->scrollAreaWidgetContents->layout())
    {
        dstLayout = new QGridLayout();
    }
    else
    {
        dstLayout = (QGridLayout*)(ui->scrollAreaWidgetContents->layout());
    }
    findHistoryUpdateWidget(dstLayout,list,list.count() >0 ?false:true);
    ui->scrollAreaWidgetContents->setLayout(dstLayout);
    ui->tabWidget->setCurrentIndex(0);
    m_oldTabColumn = 0;
}

/**************************************************************************
** 功能	@brief :  做后续参考：持续增加字符串条数
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::updateLineCounts(int currentVaildLine)
{
    if(currentLineCounts - currentVaildLine <= 20 )
    {
        currentLineCounts++;
        insertLine(((QGridLayout *)ui->scrollAreaWidgetContents->layout()),currentLineCounts,"",QString::number(currentLineCounts),false);
    }
}

/**************************************************************************
** 功能	@brief :  新增字符串
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::insertLine(QGridLayout *dstLayout,int line, QString text, QString note,bool mode)
{
    QHBoxLayout *vLayout = new QHBoxLayout();
    QLineEdit *lineEdit = new QLineEdit(text);
    lineEdit->setObjectName(QString("lineEdit_%1").arg(line));
    lineEdit->setMaxLength(150);
    lineEdit->setMinimumHeight(36);
    editPushButton *button = new editPushButton(note);
    button->setMaximumWidth(80);
    button->setObjectName(QString("pushButton_%1").arg(line));
    connect(button,SIGNAL(clicked()),this,SLOT(sendString()));
    connect(button,SIGNAL(buttonTextChange(int,QString)),this,SLOT(recvButtonTextChange(int,QString)));
    vLayout->addWidget(lineEdit);
    vLayout->addWidget(button);
    dstLayout->addLayout(vLayout,line,0);
    if(NULL != userInfoSql && mode)
    {
        ((mySqlite *)userInfoSql)->insertSqlData(QString("insert into mutilString_userInfo values (%1,'%2','%3','%4')").arg(line).arg(text).arg(note).arg("0"));
    }
}

void mutilStringSend::findHistoryUpdateWidget(QGridLayout *layout,QList<QStringList> list,bool mode,int startPlace)
{
    bool isNotExsitFlag = true;
    for(int i =startPlace;i< currentLineCounts+startPlace;i++)
    {
        isNotExsitFlag = true;
        for(int j = 0;j<list.size();j++)
        {
            QStringList lineInfo = list.at(j);
            if(lineInfo.size() == 4 && QString(lineInfo.at(0)).toInt() == i)
            {
                insertLine(layout,i, QString(lineInfo.at(1)),QString(lineInfo.at(2)),mode);
                isNotExsitFlag = false;
                break;
            }
        }
        if(isNotExsitFlag)
        {
            insertLine(layout,i,"",QString::number(i),mode);
        }
    }
}
/**************************************************************************
** 功能	@brief : 界面ui设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::uiAttributeSet()
{
    this->setWindowTitle(tr("多字符串界面"));
    ui->tabWidget->setToolTip(tr("鼠标左键双击组名即可修改组名"));
}
/**************************************************************************
** 功能	@brief : 发送AT指令
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::sendString()
{
    editPushButton *sigBtn = qobject_cast<editPushButton*>(sender());
    QString value =sigBtn->objectName().split('_').last();
    QObjectList testList =sigBtn->parentWidget()->children();
    qDebug()<<"sendString:"<<testList.at(1)->objectName()<<"value"<<value;
//    for(int i =0;i<testList.count();i++)
//    {
//        qDebug()<<"i:"<<testList.at(i)->objectName();
//    }
    qDebug()<<"objectName:"<<(qobject_cast<QLineEdit*>(testList.at(2*(value.toInt()%100)+1)))->objectName();
    QString data = (qobject_cast<QLineEdit*>(testList.at(2*(value.toInt()%100)+1)))->text();
    if(data.isEmpty())
    {
        MMessageBox::critical(this,tr("错误"),tr("AT指令不能为空"),tr("确认"));
        return;
    }
    QList<QStringList> list = ((mySqlite *)userInfoSql)->findSqlDataListEx("select sortId from mutilString_userInfo;"); // limit %1,%2

//    bool handleFlag = false;
    qDebug()<<"data:"<<data;
//    for(int i = 0;i<list.size();i++)
//    {
//        QStringList lineInfo = list.at(i);
//        if(lineInfo.size() == 1 && lineInfo.contains(value))
//        {
    ((mySqlite *)userInfoSql)->modifySqlData(QString("update mutilString_userInfo set data='%2' where sortId=%1").arg(value).arg(data));
//            handleFlag = true;
//            break;
//        }
//    }
//    if(false == handleFlag)
//        ((mySqlite *)userInfoSql)->insertSqlData(QString("insert into mutilString_userInfo values ('%1','%2','%3','%4')").arg(value).arg(data).arg(sigBtnText).arg("0"));
    emit ATCMDsend(data);
}

/**************************************************************************
** 功能	@brief : 多界面的组界面切换
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::tabWidget_currentChanged(int index)
{
    qDebug()<<"index:"<<index<<"ui->tabWidget->count():"<<ui->tabWidget->count();
    if(index == ui->tabWidget->count()-1 )
    {
        if(index < 8)
        {
            QWidget *widget = new QWidget();
            ui->tabWidget->insertTab(index,widget,QString::number(index+1));
            QList<QObject *>objList =ui->scrollAreaWidgetContents->findChildren<QObject *>();
            for(int i = objList.count()-1;i>=0;i--)
            {
                delete objList.at(i);
            }
            QGridLayout *dstLayout  = new QGridLayout();
            for(int i =0;i< currentLineCounts;i++)
            {
                insertLine((QGridLayout *)dstLayout,i+index*currentLineCounts,"",QString::number(i+index*currentLineCounts),true);
            }
            ui->scrollAreaWidgetContents->setLayout((QGridLayout *)dstLayout);
            m_oldTabColumn = index;
            QString nameList;
            for(int i = 0;i<ui->tabWidget->count();i++)
            {
                 nameList.append(ui->tabWidget->tabText(i)+";");
            }
            nameList.chop(1);
            if(NULL != userInfoSql)
            {
                if(QString(((mySqlite *)userInfoSql)->execSqlDataList("select count(sortId) AS number from mutilString_tabName").at(0)).toInt() > 0)
                {
                    ((mySqlite *)userInfoSql)->modifySqlData(QString("update mutilString_tabName set tabName='%1'").arg(nameList));
                }
                else
                {
                    ((mySqlite *)userInfoSql)->insertSqlData(QString("insert into mutilString_tabName values ('%1','%2')").arg(0).arg(nameList));
                }
            }
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-2);
//            QJsonValue value = QJsonValue(nameList);
//            userConfigSaveInstance::globalInstance()->addJsonToFile("mutilSend_tabNameList",&value);
        }
        else
        {
            MMessageBox::critical(this,tr("错误"),tr("不可超过8个数组"),tr("确认"));
//            disconnect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabWidget_currentChanged(int)));
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-2);
//            connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabWidget_currentChanged(int)));
        }
    }
    else
    {
        if(m_oldTabColumn != index)
        {
            qDebug()<<"m_oldTabColumn:"<<m_oldTabColumn;
            m_oldTabColumn = index;
            QList<QStringList> list = ((mySqlite *)userInfoSql)->findSqlDataListEx(QString("select * from mutilString_userInfo limit %1,%2").arg(index*currentLineCounts).arg(currentLineCounts));
            QList<QObject *>objList =ui->scrollAreaWidgetContents->findChildren<QObject *>();
            for(int i = objList.count()-1;i>=0;i--)
            {
                delete objList.at(i);
            }

            QGridLayout *dstLayout  = new QGridLayout();
            findHistoryUpdateWidget((QGridLayout *)dstLayout,list,false,index*currentLineCounts);
            ui->scrollAreaWidgetContents->setLayout((QGridLayout *)dstLayout);
            ui->scrollAreaWidgetContents->update();
        }
    }
}
/**************************************************************************
** 功能	@brief : 多界面中对组名进行双击可编辑组名
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::on_tabWidget_tabBarDoubleClicked(int index)
{
    QString oldText = ui->tabWidget->tabText(index);
    bool ok;
    QString text = InputDialog::getText(tr("请输入更改的表名"),oldText, &ok, tr("确定"), tr("取消"));
//    QString text = QInputDialog::getText(0, tr("备注"),tr("请输入更改的表名"), QLineEdit::Normal,oldText, &ok,Qt::WindowCloseButtonHint);
    if (ok)
    {
        if(text.length()>150)
        {
            MMessageBox::critical(this,tr("错误"),tr("长度不能超过150字节"),tr("确认"));
            return;
        }
        ui->tabWidget->setTabText(index,text);
        QString nameList;
        for(int i = 0;i<ui->tabWidget->count();i++)
        {
             nameList.append(ui->tabWidget->tabText(i)+";");
        }
        nameList.chop(1);
//        QJsonValue value = QJsonValue(nameList);
//        userConfigSaveInstance::globalInstance()->addJsonToFile("mutilSend_tabNameList",&value);
        if(NULL != userInfoSql)
        {
            ((mySqlite *)userInfoSql)->modifySqlData(QString("update mutilString_tabName set tabName='%1'").arg(nameList));
        }
        tabWidget_currentChanged(index);
    }
}
/**************************************************************************
** 功能	@brief : 导入数据文件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::on_pushButton_importDb_clicked()
{
    QString fileName;
    fileName = QFileDialog::getOpenFileName(NULL,tr("选取文件"),".",tr("(数据库文件(*.db)"));
    if(fileName.isEmpty())
        return;
    qDebug()<<"fileName:"<<fileName;
    // 判断当前db是否符合格式
    QFileInfo newDataBseFile(fileName);
//    mySqlite *newSqlite = new mySqlite();
//    newSqlite->createSqlDataBase("mutilString_sqlite",QString());
//    newSqlite->setSqlPath(fileName);
//    qDebug()<<"getDataBaseName:"<<newSqlite->getDataBaseName();
//    if(!newSqlite->isMathQuery("mutilString_tabName") || !newSqlite->isMathQuery("mutilString_userInfo"))
//    {
//        newSqlite->closeSqlDataBase();
//        MMessageBox::critical(this,QString("错误"),QString("数据库格式校验失败，请重新导入"),"确认");
//        return;
//    }
//    newSqlite->closeSqlDataBase();
    // 关闭原来打开的数据库
    QString oldDataBasePath;
    if(NULL != userInfoSql)
    {
        oldDataBasePath = ((mySqlite *)userInfoSql)->getDataBaseName();
        qDebug()<<"oldDataBasePath:"<<oldDataBasePath;
        ((mySqlite *)userInfoSql)->closeSqlDataBase();
    }
    else
    {
        oldDataBasePath = vitalFilePath+"normal_info.db";
    }

    // 复制粘贴 更改名字
    if(newDataBseFile.exists())
    {
        QFileInfo oldDataBseFile(oldDataBasePath);
        if(oldDataBseFile.exists())
        {
            QDir dir;
            dir.remove(oldDataBasePath);
        }
        if(!QFile::copy(fileName,oldDataBasePath))
        {
            MMessageBox::critical(this,tr("错误"),tr("数据库导入失败"),tr("确认"));
        }
        // clear tab
//        disconnect(ui->tabWidget,&QTabWidget::currentChanged,this,&mutilStringSend::tabWidget_currentChanged);
        // 打开新的数据库 //更新UI界面
        initSqlite();
//        connect(ui->tabWidget,&QTabWidget::currentChanged,this,&mutilStringSend::tabWidget_currentChanged);
    }
    else
    {
        // 重新打开
        initSqlite();
    }
}
/**************************************************************************
** 功能	@brief : 导出数据文件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::on_pushButton_exportDb_clicked()
{
//    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
//    qDebug()<<"dir:"<<dir;
//    QString newFilePath = dir + "normal_info.db";
//    if(!QFile::copy(fileName,oldDataBasePath))
//    {
//        qCritical()<<u8"复制失败";
//    }

    QString strSaveName = QFileDialog::getSaveFileName(this,tr("需要保存的文件"),tr("."),tr("Items files(*.db)"));
    QString oldDataBasePath = vitalFilePath+"normal_info.db";
    QFileInfo fileInfo(strSaveName);
    if(fileInfo.exists())
    {
        QFile::remove(strSaveName);
    }
    if(!QFile::copy(oldDataBasePath,strSaveName))
    {
        MMessageBox::critical(this,tr("错误"),tr("数据库导出失败"),tr("确认"));
    }
}
///**************************************************************************
//** 功能	@brief : 删除数据
//** 输入	@param :
//** 输出	@retval:
//***************************************************************************/
//void mutilStringSend::recvTabClose(int index)
//{
//    // 删除
//    if(NULL != userInfoSql)
//    {
//        // 删除数据
//        for(int i = index * currentLineCounts;i < (index+1) * currentLineCounts;i++)
//        {
//            ((mySqlite *)userInfoSql)->delSqlData(QString("delete from mutilString_userInfo where sortId=%1;").arg(i));
//        }
//        // 更新tab名字
//        ((mySqlite *)userInfoSql)->delSqlData("delete from mutilString_tabName where sortId=0;");
//        ui->tabWidget->removeTab(index);
//        QString nameList;
//        for(int i = 0;i<ui->tabWidget->count();i++)
//        {
//             nameList.append(ui->tabWidget->tabText(i)+";");
//        }
//        nameList.chop(1);
//        ((mySqlite *)userInfoSql)->insertSqlData(QString("insert into mutilString_tabName values ('%1','%2')").arg(0).arg(nameList));
//        // 更新顺序c
////        ((mySqlite *)userInfoSql)->modifySqlData(QString("update mutilString_userInfo set sortId=sortId-100 where sortId>%1;").arg(index * currentLineCounts));
////        ((mySqlite *)userInfoSql)->modifySqlData(QString("update mutilString_userInfo set note=cast(sortId as text) where sortId>%1 and ;").arg(index * currentLineCounts));
//    }

//}
/**************************************************************************
** 功能	@brief : 更新数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mutilStringSend::recvButtonTextChange(int index, QString text)
{
    ((mySqlite *)userInfoSql)->modifySqlData(QString("update mutilString_userInfo set note='%2' where sortId=%1").arg(index).arg(text));
}
