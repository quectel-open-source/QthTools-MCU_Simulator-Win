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
#include "configwindow.h"
#include "ui_configwindow.h"
#include <QStandardPaths>
#include <QInputDialog>
#include "baseQTConfigure.h"
#include "userconfigure.h"
#include "modelwindow.h"
#include "../subwindow/inputdialog.h"
#include "kernel/mmessagebox.h"

configWindow::configWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::configWindow)
{
    ui->setupUi(this);
    ui->label_info->hide();
    showTimer = new QTimer();
    connect(showTimer, SIGNAL(timeout()), this, SLOT(showTimerTimeout()));
    showTimer->setSingleShot(true);
}

configWindow::~configWindow()
{
    delete ui;
}

void configWindow::showTimerTimeout()
{
    ui->label_info->hide();
}

void configWindow::setParm(void *father)
{
    this->father = (MainWindow*)father;
    this->father->configprocess->configInfoRead(ui->tableWidget_config,&this->currentConfigInfo);
    //配置信息获取
    uiAttributeSet();
}
/**************************************************************************
** 功能	@brief :  控件属性配置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::uiAttributeSet()
{
    ui->tableWidget_config->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->tableWidget_config->setColumnWidth(0,100);
    ui->tableWidget_config->setColumnWidth(1,200);
    ui->tableWidget_config->setColumnWidth(2,100);
    ui->tableWidget_config->setShowGrid(false);
    ui->tableWidget_config->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
    ui->tableWidget_config->verticalHeader()->setDefaultSectionSize(48);
    ui->tableWidget_config->setStyleSheet("QTableWidget{ \
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

    ui->tableWidget_config->setContextMenuPolicy(Qt::NoContextMenu);
    ui->search_lineEdit->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));

    ui->tableWidget_config->setDragEnabled(false);

    ui->tableWidget_config->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_config->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	this->father->setModelWindowMess(0,0 == this->currentConfigInfo.format.compare("TTLV")?true:false);
    searchProductInfoHistoryRow = -1;
    emit configChangeSignal(this->currentConfigInfo.filePath);
    connect(ui->tableWidget_config,SIGNAL(itemSelectionChanged()),this,SLOT(CurrentConfigInfoChange()));
    connect(ui->tableWidget_config,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(writeItemNote(QTableWidgetItem *)));
    connect(&atHubInstance,SIGNAL(signalAtAck(QString)),this,SLOT(handleAtAck(QString)));
}
/**************************************************************************
** 功能	@brief :  发送产品信息恢复信号
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::restoreProductInformation()
{
    emit restoreProductInformationSignal(this->currentConfigInfo.pk,this->currentConfigInfo.ps,this->currentConfigInfo.model);
}
/**************************************************************************
** 功能	@brief :  添加新产品
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::on_pushButton_configAdd_clicked()
{
    qInfo()<<__FUNCTION__;

    if(addconfigwindow == NULL)
    {
        addconfigwindow = new addConfigWindow();
        addconfigwindow->setAttribute(Qt::WA_DeleteOnClose);
        connect(addconfigwindow,SIGNAL(addConfigSignal(configInfo_t)),this,SLOT(addConfigSlot(configInfo_t)));
        connect(addconfigwindow,SIGNAL(sendCloseEvent(QCloseEvent *)),this,SLOT(addconfigwindowCloseEvent(QCloseEvent *)));
        addconfigwindow->show();
    }
    else
    {
        if(addconfigwindow->isHidden())
        {
            addconfigwindow->show();
        }
        else
        {
            addconfigwindow->hide();
//            addconfigwindow->show();
        }
    }
}
/**************************************************************************
** 功能	@brief :  添加新产品确认
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::addConfigSlot(configInfo_t configInfo)
{
    qInfo()<<__FUNCTION__;
    int row = ui->tableWidget_config->rowCount();
    for(int i =0;i<row;i++)
    {
        if(0 == ui->tableWidget_config->item(i,CONFIG_TABLE_PK)->text().compare(configInfo.pk))
        {
            MMessageBox::critical(this,tr("错误"),tr("不能存在相同的产品号"),tr("确认"));
            return;
        }
    }
    if(configInfo.filePath.isEmpty())
    {
        ui->tableWidget_config->setRowCount(row+1);
        ui->tableWidget_config->setItem(row,CONFIG_TABLE_JSON,new QTableWidgetItem(tr("透传数据")));
    }
    else
    {
        if(configInfo.filePath.isEmpty() || configInfo.pk.isEmpty() || configInfo.ps.isEmpty())
        {
            MMessageBox::warning(this,tr("警告"),tr("创建失败"),tr("确认"));
            return;
        }
        QStringList strList = configInfo.filePath.split("/");
        QDateTime time = QDateTime::currentDateTime();
        QString destFile = vitalFilePath+QString::number(time.toTime_t())+"_"+strList.constLast();
        if(!QFile::copy(configInfo.filePath,destFile))
        {
            MMessageBox::critical(this,tr("错误"),tr("文件拷贝失败"),tr("确认"));
            return ;
        }
        QTableWidgetItem *model =  new QTableWidgetItem();
        model->setText(tr("物模型"));
        model->setToolTip(destFile);
        ui->tableWidget_config->setRowCount(row+1);
        ui->tableWidget_config->setItem(row,CONFIG_TABLE_JSON,model);
    }

    ui->tableWidget_config->setItem(row,CONFIG_TABLE_PK,new QTableWidgetItem(configInfo.pk));
    ui->tableWidget_config->setItem(row,CONFIG_TABLE_PS,new QTableWidgetItem(configInfo.ps));
    ui->tableWidget_config->setItem(row,CONFIG_TABLE_NOTE,new QTableWidgetItem(configInfo.note));
    ui->tableWidget_config->setCurrentCell(row,CONFIG_TABLE_PK);
}
/**************************************************************************
** 功能	@brief :  删除产品
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::on_pushButton_configDel_clicked()
{
    qInfo()<<__FUNCTION__;
    if(ui->tableWidget_config->currentRow() == -1)
    {
        MMessageBox::warning(this,tr("警告"),tr("请先选择需要删除的行"),tr("确认"));
        return;
    }
    MMessageBox *box = new MMessageBox(MMessageBox::Information,tr("提示"),tr("是否确定删除配置文件"),tr("是"), tr("否"));
    int ret = box->exec();
    if(ret == MMessageBox::No)
        return;
    QTableWidgetItem *model =  ui->tableWidget_config->item(ui->tableWidget_config->currentRow(),CONFIG_TABLE_JSON);
    QFile::remove(model->toolTip());
    ui->tableWidget_config->removeRow(ui->tableWidget_config->currentRow());
    if(ui->tableWidget_config->rowCount() > 0)
    {
        ui->tableWidget_config->setCurrentCell(0,CONFIG_TABLE_PK);
    }
    else
    {
        this->father->configprocess->configInfoSave(ui->tableWidget_config,&this->currentConfigInfo);
    }
}

/**************************************************************************
** 功能	@brief :  当前产品切换
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::CurrentConfigInfoChange()
{
    qInfo()<<__FUNCTION__;
    bool changeModel = false;
    int row = ui->tableWidget_config->currentRow();
    if(row == -1)
    {
        this->father->configprocess->configInfoSave(ui->tableWidget_config,&(this->currentConfigInfo));
        return;
    }    
    this->currentConfigInfo.pk = ui->tableWidget_config->item(row,CONFIG_TABLE_PK)->text();
    this->currentConfigInfo.ps = ui->tableWidget_config->item(row,CONFIG_TABLE_PS)->text();
    this->currentConfigInfo.note = ui->tableWidget_config->item(row,CONFIG_TABLE_NOTE)->text();

    if(tr("透传数据") == ui->tableWidget_config->item(row,CONFIG_TABLE_JSON)->text())
    {
        if(this->currentConfigInfo.filePath != ui->tableWidget_config->item(row,CONFIG_TABLE_JSON)->text())
        {
            this->currentConfigInfo.filePath.clear();
            changeModel = true;
        }
    }
    else if(tr("物模型") == ui->tableWidget_config->item(row,CONFIG_TABLE_JSON)->text())
    {
        if(this->currentConfigInfo.filePath != ui->tableWidget_config->item(row,CONFIG_TABLE_JSON)->text())
        {
            this->currentConfigInfo.filePath = ui->tableWidget_config->item(row,CONFIG_TABLE_JSON)->toolTip();
            changeModel = true;
        }
    }
    this->father->configprocess->configInfoSave(ui->tableWidget_config,&(this->currentConfigInfo));
    if(changeModel)
    {
        emit configChangeSignal(this->currentConfigInfo.filePath);
    }
}
/**************************************************************************
** 功能	@brief :  产品PK,PS设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::configWindowProductSet()
{
    qInfo()<<__FUNCTION__;
    int row = ui->tableWidget_config->currentRow();
    if(row < 0)
    {
//        MMessageBox::information(this,tr("提示"),tr("请先选择对应的产品信息"),tr("确认"));
        return;
    }
    QString pk = ui->tableWidget_config->item(row,CONFIG_TABLE_PK)->text();
    QString ps = ui->tableWidget_config->item(row,CONFIG_TABLE_PS)->text();
    QString model = ui->tableWidget_config->item(row,CONFIG_TABLE_JSON)->text();
    this->father->ATCMDSend("AT+QIOTCFG=\"productinfo\",\""+pk+"\",\""+ps+"\"");
    this->currentConfigInfo.pk = pk;
    this->currentConfigInfo.ps = ps;
    this->currentConfigInfo.model = model;
    emit setProductConfigSignal(pk,ps,model);
}
/**************************************************************************
** 功能	@brief :  产品PK,PS获取并发送信号修改debug信息
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::getProductInformation()
{
    qInfo()<<__FUNCTION__;
    int row = ui->tableWidget_config->currentRow();
    if(row < 0)
    {
        emit setProductConfigSignal(NULL,NULL,NULL);
//        MMessageBox::information(this,tr("提示"),tr("请先选择对应的产品信息"),tr("确认"));
        return;
    }
    QString pk = ui->tableWidget_config->item(row,CONFIG_TABLE_PK)->text();
    QString ps = ui->tableWidget_config->item(row,CONFIG_TABLE_PS)->text();
    QString model = ui->tableWidget_config->item(row,CONFIG_TABLE_JSON)->text();
    this->currentConfigInfo.pk = pk;
    this->currentConfigInfo.ps = ps;
    this->currentConfigInfo.model = model;
    emit setProductConfigSignal(pk,ps,model);
}
/**************************************************************************
** 功能	@brief :  产品PK,PS获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::configWindowProductGet()
{
    qInfo()<<__FUNCTION__;
    this->father->ATCMDSend("AT+QIOTCFG=\"productinfo\"");
}
/**************************************************************************
** 功能	@brief :  产品备注
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::writeItemNote(QTableWidgetItem *item)
{
    if(item->column() == CONFIG_TABLE_NOTE)
    {
        QString oldText;
        if(item != NULL && item->text() != NULL)
        {
            oldText.append(item->text());
        }
        bool ok;

        QString text = InputDialog::getText(tr("备注"),oldText, &ok, tr("确定"), tr("取消"));
        if (ok)
        {
            QTableWidgetItem *newItem = new QTableWidgetItem();
            newItem->setText(text);
            ui->tableWidget_config->setItem(item->row(),item->column(),newItem);
            CurrentConfigInfoChange();
        }
    }
}
/**************************************************************************
** 功能	@brief :  搜索产品
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::searchPK()
{
    QString text = ui->search_lineEdit->text();
    if(text != NULL && text.count() > 0)
    {
        int rowSum = ui->tableWidget_config->rowCount();
        int i= 0;
        bool searchResult = false;
        bool loopSearch = false;
        for(i = searchProductInfoHistoryRow+1;i<rowSum;i++)
        {
            if(ui->tableWidget_config->item(i,0)->text().contains(text,Qt::CaseSensitive))
            {
                ui->tableWidget_config->setCurrentCell(i,1);
                ui->tableWidget_config->selectRow(i);
                searchResult = true;
                break;
            }
            if(rowSum == i+1 && false == loopSearch)
            {
                i = 0;
                rowSum = searchProductInfoHistoryRow+1;
                loopSearch = true;
            }
        }
        searchProductInfoHistoryText.swap(text);
        if(true == searchResult && text.count() <= 6)
        {
            searchProductInfoHistoryRow = i;
        }
        else
        {
            searchProductInfoHistoryRow = -1;
            ui->label_info->show();
            ui->label_info->setText(tr("搜索不到匹配内容或已搜索到底部"));
            showTimer->start(3000);
//            this->father->statusBarShow(tr("搜索不到匹配内容或已搜索到底部"),3);
            ui->tableWidget_config->setCurrentCell(0,1);
            ui->tableWidget_config->selectRow(0);
        }
    }
    else
    {
        ui->label_info->show();
        ui->label_info->setText(tr("搜索内容不能为空"));
        showTimer->start(3000);
//        this->father->statusBarShow(tr("搜索内容不能为空"));
    }
}
/**************************************************************************
** 功能	@brief :  搜索模糊匹配的下一个产品
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::searchNextPK()
{
    int rowSum = ui->tableWidget_config->rowCount();
    int rowNum;
    bool searchResult = false;
    bool loopSearch = false;
    for(rowNum = searchProductInfoHistoryRow+1;rowNum<rowSum;rowNum++)
    {
        if(ui->tableWidget_config->item(rowNum,0)->text().contains(searchProductInfoHistoryText,Qt::CaseInsensitive))
        {
            ui->tableWidget_config->setCurrentCell(rowNum,1);
            ui->tableWidget_config->selectRow(rowNum);
            searchResult = true;
            break;
        }
        if(rowSum == rowNum+1 && false == loopSearch)
        {
            rowNum = 0;
            rowSum = searchProductInfoHistoryRow+1;
            loopSearch = true;
        }
    }
    if(true == searchResult)
    {
        searchProductInfoHistoryRow = rowNum;
    }
    else
    {
        MMessageBox::warning(this,tr("警告"),tr("已轮询一次，请重新输入搜索值"),tr("确认"));
//            pop_menu->removeAction(next_search_action);
        searchProductInfoHistoryRow = -1;
        searchProductInfoHistoryText.clear();
        ui->tableWidget_config->setCurrentCell(0,1);
    }
}

void configWindow::handleConfigGetEvent(QList<QByteArray> paraList)
{
    qDebug()<<"configWindow:"<<__FUNCTION__<<"paraList:"<<paraList;
    if(paraList.size() >0)
    {
        bool ret = false;
        if(QString(paraList[0]) == "productinfo")
        {
            if(paraList.size() == 3)
            {
                for(int i=0;i<ui->tableWidget_config->rowCount();i++)
                {
                    if(ui->tableWidget_config->item(i,0)->text() == paraList[1] && ui->tableWidget_config->item(i,1)->text() == paraList[2])
                    {
                        ui->tableWidget_config->selectRow(i);
                        // 要前面区分set和get 才能使用此步骤（发送设置命令）
                        ret = true;
                    }
                }
            }
        }
        qDebug()<<"ret:"<<ret;
        if(ret)
        {
            CurrentConfigInfoChange();
        }
    }
}

bool configWindow::retranslationUi()
{
    if(NULL != addconfigwindow)
    {
        addconfigwindow->retranslationUi();
    }
    ui->retranslateUi(this);
    int rowCount = ui->tableWidget_config->rowCount();
    for (int i = 0; i < rowCount; i ++)
    {
        if (ui->tableWidget_config->item(i, 2)->text() == "物模型" || ui->tableWidget_config->item(i, 2)->text() == "TSL")
        {
            ui->tableWidget_config->item(i, 2)->setText(tr("物模型"));
        }
        else if (ui->tableWidget_config->item(i, 2)->text() == "透传数据" || ui->tableWidget_config->item(i, 2)->text() == "Transparent")
        {
            ui->tableWidget_config->item(i, 2)->setText(tr("透传数据"));
        }
    }
    return true;
}
/**************************************************************************
** 功能	@brief :  搜索按键响应
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::on_search_pushButton_clicked()
{
    searchPK();
}

/**************************************************************************
** 功能	@brief :  添加产品的窗口关闭响应事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::addconfigwindowCloseEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    addconfigwindow = NULL;
}
/**************************************************************************
** 功能	@brief :  处理AT指令反馈
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configWindow::handleAtAck(QString cmdData)
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
