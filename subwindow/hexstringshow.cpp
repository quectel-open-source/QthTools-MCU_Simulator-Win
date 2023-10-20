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
#include "hexstringshow.h"
#include "ui_hexstringshow.h"
#include "third/hexshowtablewidget.h"
#include <QDebug>
#include <QPoint>
#include "kernel/mmessagebox.h"
#include <QEvent>
#include <windows.h>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

/**************************************************************************
** 功能	@brief :  16进制数据制作界面
** 输入	@param :
** 输出	@retval:
***************************************************************************/
hexStringShow::hexStringShow(QWidget *parent, QString data):
    QWidget(parent),
    ui(new Ui::hexStringShow)
{
    ui->setupUi(this);
    int dataCounts = data.count()/2;
    itemsSum = dataCounts;
    int rows = dataCounts/10 + 1;
    ui->tableWidget_showHex->setRowCount(rows);
    ui->tableWidget_showHex->setAttribute(Qt::WA_InputMethodEnabled,false);
    int i=0,j=0;
    while(dataCounts >= 0)
    {
        QTableWidgetItem *item = new QTableWidgetItem(data.mid(i*20+j*2,2));
        ui->tableWidget_showHex->setItem(i,j,item);
        j++;
        if(j == 10)
        {
            j=0;
            i++;
        }
        dataCounts--;
    }
    ui->tableWidget_showHex->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_showHex->horizontalHeader()->setFont(QFont("黑体", 25));
    ui->tableWidget_showHex->verticalHeader()->setDefaultSectionSize(40);  //设置默认高度
    ui->tableWidget_showHex->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              border-right-width:0px; \
                                                              border-left-width:0px; \
                                                              }");
    ui->tableWidget_showHex->verticalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              border-right-width:0px; \
                                                              border-left-width:0px; \
                                                              }");
    ui->tableWidget_showHex->setStyleSheet("QTableWidget{ \
                                          color: rgb(104, 112, 130); \
                                          text-align:center;\
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

    //去除窗体边框
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    //窗体关闭信号
    connect(ui->pushButton_close, &QPushButton::clicked, [=]() {close(); });
    this->setWindowModality(Qt::ApplicationModal);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#C0C0C0"));
    shadow->setBlurRadius(15);
    shadow->setOffset(5);
    ui->widget->setGraphicsEffect(shadow);
}

hexStringShow::~hexStringShow()
{
    delete ui;
}

/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void hexStringShow::mousePressEvent(QMouseEvent *event)
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

void hexStringShow::mouseMoveEvent(QMouseEvent *event)
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

void hexStringShow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

void hexStringShow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    event = NULL;
    emit dataChangeForHexWiget(NULL,0);
}

void hexStringShow::on_pushButton_ok_clicked()
{
    int rowSum = itemsSum/10 + (itemsSum%10?1:0);
    QByteArray data;
    for(int i =0;i<rowSum;i++)
    {

        QByteArray rowData;
        for(int j=0;j<10;j++)
        {
            if(i == rowSum-1 && j ==  itemsSum-(rowSum-1)*10)
                break;
            if(ui->tableWidget_showHex->item(i,j)!= NULL && !ui->tableWidget_showHex->item(i,j)->text().isEmpty())
            {
                QByteArray itemText = ui->tableWidget_showHex->item(i,j)->text().toLatin1();
                if(itemText.count() == 1)
                {
                    itemText.insert(0,'0');
                }
                rowData.append(itemText);
            }
            else
            {
                QString replaceNumber = "00";
                QTableWidgetItem *item = new QTableWidgetItem(replaceNumber);
                ui->tableWidget_showHex->setItem(i,j,item);
                rowData.append(replaceNumber.toLatin1());
            }
        }
        if(!rowData.isEmpty())
            data.append(rowData);
    }

    if(ui->spinBox->value()*2 > data.size())
    {
        MMessageBox::critical(this,tr("错误"),tr("输出长度不能大于已输入的长度"),tr("确认"));
        return;
    }
    data.resize(ui->spinBox->value()*2);
    emit dataChangeForHexWiget(data,1);
}

void hexStringShow::on_pushButton_clearLog_clicked()
{
    ui->tableWidget_showHex->setRowCount(0);
    ui->tableWidget_showHex->insertRow(0);
    ui->spinBox->setValue(0);
    itemsSum =0;
}



void hexStringShow::on_tableWidget_showHex_cellChanged(int row, int column)
{
//    int rowSum = ui->tableWidget_showHex->rowCount();
//    int columnSum = ui->tableWidget_showHex->columnCount();
//    int num=0;
//    for(int i=0;i<columnSum-1;i++)
//    {
//        QTableWidgetItem *item = ui->tableWidget_showHex->item(rowSum-1,i);
//        if(item != NULL && !item->text().isEmpty())
//             num++;
//    }
//    int sum = num + (rowSum-1)*(columnSum-1);
//    ui->spinBox->setValue(sum);
     if(column !=10)
     {
        int currentItemSum = row*10+column+1;
        if(itemsSum < currentItemSum && !ui->tableWidget_showHex->item(row,column)->text().isEmpty())
            itemsSum = currentItemSum;
        ui->spinBox->setValue(itemsSum);
     }
}

void hexStringShow::on_pushButton_cancel_clicked()
{
    this->close();
}
