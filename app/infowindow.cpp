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
#include "infowindow.h"
#include "ui_infowindow.h"
#include "kernel/mmessagebox.h"

/*
 * 右侧ui改变： 内置和外置分开
 */
enum
{
    INFO_INER = 0,
    INFO_EXTERN,
};

infoWindow::infoWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::infoWindow)
{
    ui->setupUi(this);
    ui->tabWidget_gpsInfo->setCurrentIndex(0);
    ui->tableWidget_locationInfoExc_get->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->tableWidget_locationInfoExc_get->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    ui->tableWidget_locationInfo_get->setSelectionBehavior ( QAbstractItemView::SelectRows);
    ui->tableWidget_locationInfo_get->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
//    ui->tableWidget_locationInfo_get->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableWidget_locationInfo_get->verticalHeader()->setDefaultSectionSize(40);
    ui->tableWidget_locationInfoExc_get->verticalHeader()->setDefaultSectionSize(40);
    ui->tableWidget_info->verticalHeader()->setDefaultSectionSize(40);
    ui->tableWidget_locationInfo_get->setSelectionBehavior(QAbstractItemView::SelectRows);
//    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_locationInfo_get->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_locationInfo_get->setShowGrid(false);
    ui->tableWidget_locationInfo_get->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
    ui->tableWidget_locationInfo_get->verticalHeader()->setStyleSheet("QHeaderView::section{ \
                                                            color: rgb(104, 112, 130); \
                                                            font-size:16px; \
                                                            background: transparent; \
                                                            background: rgb(246, 249, 255); \
                                                            padding: 1px;border: none; \
                                                            border-style:solid; \
                                                            border-right-width:0px; \
                                                            border-left-width:0px; \
                                                            }");
    ui->tableWidget_locationInfo_get->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              border-right-width:0px; \
                                                              border-left-width:0px; \
                                                              }");
    ui->tableWidget_locationInfo_get->setStyleSheet("QTableWidget{ \
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
    ui->tableWidget_locationInfoExc_get->setSelectionBehavior(QAbstractItemView::SelectRows);
    //    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_locationInfoExc_get->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_locationInfoExc_get->setShowGrid(false);
    ui->tableWidget_locationInfoExc_get->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
    ui->tableWidget_locationInfoExc_get->verticalHeader()->setStyleSheet("QHeaderView::section{ \
                                                            color: rgb(104, 112, 130); \
                                                            font-size:16px; \
                                                            background: transparent; \
                                                            background: rgb(246, 249, 255); \
                                                            padding: 1px;border: none; \
                                                            border-style:solid; \
                                                            border-right-width:0px; \
                                                            border-left-width:0px; \
                                                            }");
    ui->tableWidget_locationInfoExc_get->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                            color: rgb(104, 112, 130); \
                                                            font-size:16px; \
                                                            background: transparent; \
                                                            background: rgb(246, 249, 255); \
                                                            padding: 1px;border: none; \
                                                            border-style:solid; \
                                                            border-right-width:0px; \
                                                            border-left-width:0px; \
                                                            }");
    ui->tableWidget_locationInfoExc_get->setStyleSheet("QTableWidget{ \
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
    ui->tableWidget_info->setSelectionBehavior(QAbstractItemView::SelectRows);
    //    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_info->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_info->setShowGrid(false);
    ui->tableWidget_info->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
    ui->tableWidget_info->verticalHeader()->setStyleSheet("QHeaderView::section{ \
                                                            color: rgb(104, 112, 130); \
                                                            font-size:16px; \
                                                            background: transparent; \
                                                            background: rgb(246, 249, 255); \
                                                            padding: 1px;border: none; \
                                                            border-style:solid; \
                                                            border-right-width:0px; \
                                                            border-left-width:0px; \
                                                            }");
    ui->tableWidget_info->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                            color: rgb(104, 112, 130); \
                                                            font-size:16px; \
                                                            background: transparent; \
                                                            background: rgb(246, 249, 255); \
                                                            padding: 1px;border: none; \
                                                            border-style:solid; \
                                                            border-right-width:0px; \
                                                            border-left-width:0px; \
                                                            }");
    ui->tableWidget_info->setStyleSheet("QTableWidget{ \
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
}

infoWindow::~infoWindow()
{
    delete ui;
}
/**************************************************************************
** 功能	@brief :  设置父
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::setParm(void *father)
{
    this->father = (MainWindow*)father;
}
/**************************************************************************
** 功能	@brief :  获取设备信息
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_infoGet_clicked()
{
    this->father->ATCMDSend("AT+QIOTINFO?");
}
/**************************************************************************
** 功能	@brief :  上报设备信息
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_infoReport_clicked()
{
    this->father->ATCMDSend("AT+QIOTINFO=\"up\"");
//    ui->tableWidget_info->clear();
}
/**************************************************************************
** 功能	@brief :  模组信息获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::csdkInfoGet(int count,QList<QByteArray> paraList)
{
    qInfo()<<__FUNCTION__;
//    int rowCount =  ui->tableWidget_info->rowCount();
    ui->tableWidget_info->setRowCount(count);

    for(int i=0;i<count;i++)
    {
        QString key = QString(paraList.at(2*i)).replace("\"","");
        QString value = QString(paraList.at(2*i+1)).replace("\"","");
        ui->tableWidget_info->setItem(i,0,new QTableWidgetItem(key));
        ui->tableWidget_info->setItem(i,1,new QTableWidgetItem(value));
    }
}

/**************************************************************************
** 功能	@brief :  获取用户选择的行，并由大到小排序
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QList<int> infoWindow::getListSelectedRows(QTableWidget * tableWidget)
{
    qInfo()<<__FUNCTION__;
    QList<QTableWidgetSelectionRange>ranges = tableWidget->selectedRanges();
    QList<int> rowList;
    int count=ranges.count();;
    for(int i=0;i<count;i++)
    {

       int topRow=ranges.at(i).topRow();
       int bottomRow=ranges.at(i).bottomRow();
       for(int j=topRow;j<=bottomRow;j++)
       {
           rowList.append(j);
       }
    }
    std::sort(rowList.begin(), rowList.end());
    QList<int> rowListReverse;
    for(int i =rowList.count()-1;i>=0;i--)
    {
        rowListReverse.append(rowList[i]);
    }
    return rowListReverse;
}
/**************************************************************************
** 功能	@brief :  获取内置定位的指令格式
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_configTest_clicked()
{
    this->father->ATCMDSend("AT+QIOTLOCIN=?");
}
/**************************************************************************
** 功能	@brief :  获取内置定位
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_configGet_clicked()
{
    this->father->ATCMDSend("AT+QIOTLOCIN?");
    ui->tabWidget_gpsInfo->setCurrentIndex(0);
}
/**************************************************************************
** 功能	@brief :  设置内置定位
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_configSet_clicked()
{
    qInfo()<<__FUNCTION__;
    QString str;
    QList<int> indexList = getListSelectedRows(ui->tableWidget_locationInfo_get);
    if(indexList.count() > 0)
    {
        foreach (int index, indexList)
        {
            str.append("\""+ui->tableWidget_locationInfo_get->item(index,0)->text()+"\",");
        }
        str.chop(1);
        this->father->ATCMDSend("AT+QIOTLOCIN=1,"+str);
    }
    else
    {
        MMessageBox::warning(this,tr("警告"),tr("请先选择需要配置的类型"),tr("确认"));
    }
}

/**************************************************************************
** 功能	@brief :  定位更新
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::locationUpdate(QByteArrayList data, int number)
{
    qInfo()<<__FUNCTION__;
    QTableWidget *widget;
    if(INFO_INER == number)
    {
        widget = ui->tableWidget_locationInfo_get;
    }
    else if(INFO_EXTERN == number)
    {
        widget = ui->tableWidget_locationInfoExc_get;
    }
    if(data.count() <= 0)
    {
        return;
    }
    if(data.at(0).indexOf('$') >= 0)
    {
        locaionDataUpdate(data,widget);
    }
    else
    {
        locationNameUpdata(data,widget);
    }
}

/**************************************************************************
** 功能	@brief :  定位类型更新
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::locationNameUpdata(QByteArrayList data, QTableWidget *widget)
{
    qInfo()<<__FUNCTION__;
    if(data.count())
    {
        widget->setRowCount(0);
        widget->setRowCount(data.count());
        for(int i=0;i<data.count();i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(QString(data.at(i)));
//            item->setFlags(item->flags() & (~Qt::ItemIsEditable));
//            item->setBackground(QBrush(QColor(Qt::lightGray)));
            widget->setItem(i,0,item);
        }
    }
}

/**************************************************************************
** 功能	@brief :  定位数据更新
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::locaionDataUpdate(QByteArrayList data, QTableWidget *widget)
{
    qInfo()<<__FUNCTION__;
    int sum = widget->rowCount();
    int place=0;
    if(data.count()<=0)
    {
        return;
    }
    QStringList strlist;
    QString str = NULL;
    for(int i = 0;i < data.count();i++)
    {

        if(data.at(i).indexOf('$') >=0)
        {
            if(str.length()>0)
            {
                str.replace(str.length()-1,1,';');
                strlist.append(str);
                str.clear();
            }
        }
        str.append(data[i]+',');
    }
    str = str.left(str.length()-1);
    strlist.append(str);
    str.clear();

    for(place =0;place<sum;place++)
    {
        if(data.at(0).indexOf(widget->item(place,0)->text()) >=0)
        {
            QString data2;
            for(int i =0;i<strlist.count();i++)
                data2.append(strlist[i]);
            QPushButton *button = new QPushButton(data2);
            widget->setCellWidget(place,1,button);
            if(widget == ui->tableWidget_locationInfo_get)
                connect(button,SIGNAL(clicked()),this,SLOT(OnBtnClicked()));
            else
                connect(button,SIGNAL(clicked()),this,SLOT(OnBtnOutlayClicked()));
            break;
        }
        else
            qWarning()<<"errno";
    }
}

/**************************************************************************
** 功能	@brief :  按钮事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::buttonEvent(QPushButton *senderObj, QTableWidget *widget)
{
    qInfo()<<__FUNCTION__;
    widget->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    QString data =senderObj->text();
    if(data == NULL)
        return;
    QStringList list = data.split(';');
    if(list.count()<=0)
        return;

    QTableWidget *newWidget = new QTableWidget();
    newWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    newWidget->setShowGrid(false);
    newWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              border-right-width:0px; \
                                                              border-left-width:0px; \
                                                              }");
    newWidget->setStyleSheet("QTableWidget{ \
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
    newWidget->setWindowTitle(tr("定位信息"));
    newWidget->setRowCount(list.count());
    newWidget->setColumnCount(1);
    newWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    newWidget->setWindowModality(Qt::ApplicationModal);
    for(int i =0;i<list.count();i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(list[i]);
        newWidget->setItem(i,0,item);
    }
    newWidget->show();
}
/**************************************************************************
** 功能	@brief :  重置控件状态
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::resetUIValue()
{
    ui->tableWidget_info->setRowCount(0);
    ui->tableWidget_locationInfoExc_get->setRowCount(0);
    ui->tableWidget_locationInfo_get->setRowCount(0);
}
/**************************************************************************
** 功能	@brief :  更换控件语言
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool infoWindow::retranslationUi()
{
    ui->retranslateUi(this);
    return true;
}

/**************************************************************************
** 功能	@brief :  获取外置定位的指令格式
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_outlayGPSTest_clicked()
{
    this->father->ATCMDSend("AT+QIOTLOCEXT=?");
    ui->tabWidget_gpsInfo->setCurrentIndex(1);
}
/**************************************************************************
** 功能	@brief :  设置外置定位
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_outlayGPSSet_clicked()
{
    qInfo()<<__FUNCTION__;
    QString str;
    QList<int> indexList = getListSelectedRows(ui->tableWidget_locationInfoExc_get);
    bool flag = true;
    if(indexList.count() > 0)
    {
        foreach (int index, indexList)
        {
            bool combineType = true;
            if(ui->tableWidget_locationInfoExc_get->item(index,0)==nullptr ||ui->tableWidget_locationInfoExc_get->item(index,0)->text().isEmpty())
            {
                 combineType = false;
            }
            if(ui->tableWidget_locationInfoExc_get->item(index,1)==nullptr ||ui->tableWidget_locationInfoExc_get->item(index,1)->text().isEmpty())
            {
                MMessageBox::information(this,tr("警告"),tr("第%1列数据不可为空").arg(QString::number(index+1)),tr("确认"));
                flag = false;
                break;
            }
            else
            {
                if(true == combineType)
                {
                    str.append("\"$"+ui->tableWidget_locationInfoExc_get->item(index,0)->text()+","+ui->tableWidget_locationInfoExc_get->item(index,1)->text()+"\",");
                }
                else
                {
                    str.append("\""+ui->tableWidget_locationInfoExc_get->item(index,1)->text()+"\",");
                }
            }
        }
        str.chop(1);
        if(flag == true)
        {
            this->father->ATCMDSend("AT+QIOTLOCEXT="+str);
        }
    }
    else
    {
        MMessageBox::information(this,tr("提示"),tr("请先选择需要查询的类型"),tr("确认"));
    }
}
/**************************************************************************
** 功能	@brief :  内置定位获取按钮槽
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::OnBtnClicked()
{
    qInfo()<<__FUNCTION__;
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr)
    {
        return;
    }
    buttonEvent(senderObj,ui->tableWidget_locationInfo_get);

}
/**************************************************************************
** 功能	@brief :  外置定位获取按钮槽
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::OnBtnOutlayClicked()
{
    qInfo()<<__FUNCTION__;
    QPushButton *senderObj=qobject_cast<QPushButton*>(sender());
    if(senderObj == nullptr)
    {
        return;
    }
    buttonEvent(senderObj,ui->tableWidget_locationInfoExc_get);
}
/**************************************************************************
** 功能	@brief :  添加外置定位数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_outlayGPSAdd_clicked()
{
    ui->tableWidget_locationInfoExc_get->insertRow(ui->tableWidget_locationInfoExc_get->rowCount());
    ui->tabWidget_gpsInfo->setCurrentIndex(1);
}
/**************************************************************************
** 功能	@brief :  删除外置定位数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_outlayGPSDelete_clicked()
{
    qInfo()<<__FUNCTION__;
    if(ui->tabWidget_gpsInfo->currentIndex() != 1)
    {
        MMessageBox::information(this,tr("提示"),tr("请选择需要删除的行"),tr("确认"));
        ui->tabWidget_gpsInfo->setCurrentIndex(1);
        return;
    }
    QModelIndexList selected = ui->tableWidget_locationInfoExc_get->selectionModel()->selectedRows();
    QList<int>selectRowNum;
    for(int i = 0;i<selected.count();i++)
        selectRowNum.append(selected[i].row());
    std::sort(selectRowNum.begin(),selectRowNum.end());
    for(int i =selectRowNum.count()-1;i>=0;i--)
    {
        ui->tableWidget_locationInfoExc_get->removeRow(selectRowNum[i]);
    }
}

/**************************************************************************
** 功能	@brief :  上报内置定位数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::on_pushButton_reportInsideLbs_clicked()
{
    qInfo()<<__FUNCTION__;
    QString str;
    QList<int> indexList = getListSelectedRows(ui->tableWidget_locationInfo_get);
    bool flag = true;
    if(indexList.count() > 0)
    {
        foreach (int index, indexList)
        {
            if(ui->tableWidget_locationInfo_get->item(index,0)==nullptr ||ui->tableWidget_locationInfo_get->item(index,0)->text().isEmpty())
            {
                MMessageBox::information(this,tr("提示"),tr("第%1行类型不可为空").arg(QString::number(index+1)),tr("确认"));
                flag = false;
                continue;
            }
            str.append("\""+ui->tableWidget_locationInfo_get->item(index,0)->text()+"\",");

        }
        str.chop(1);
        if(flag == true)
        {
            this->father->ATCMDSend("AT+QIOTLOCIN=0,"+str);
        }
    }
    else
    {
        MMessageBox::warning(this,tr("警告"),tr("上报定位类型为空"),tr("确认"));
    }
}

void infoWindow::on_pushButton_locationInfo_clicked()
{
    ui->tabWidget_gpsInfo->setCurrentIndex(0);
}

void infoWindow::on_pushButton_locationInfoExc_clicked()
{
    ui->tabWidget_gpsInfo->setCurrentIndex(1);
}

/**************************************************************************
** 功能	@brief :  设置信息显示模块
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void infoWindow::setInfoWindowModel(int model)
{
    if (model == 0)
    {
        ui->widget_location->hide();
        ui->widget_moduleInfo->show();
    }
    else if (model == 1)
    {
        ui->widget_location->show();
        ui->widget_moduleInfo->hide();
    }
}
