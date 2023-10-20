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
#include "treelistwindows.h"
#include "ui_treelistwindows.h"
#include "QGraphicsDropShadowEffect"
#include <QMouseEvent>
#include <QHeaderView>

QTableWidget *treeTableWidget;

treeListWindows::treeListWindows(QTableWidget *tableWidget, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::treeListWindows)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    tableWidget->horizontalHeader()->setFont(QFont("黑体", 25));
    tableWidget->horizontalHeader()->setDefaultSectionSize(50);  //设置默认宽度
    tableWidget->verticalHeader()->setDefaultSectionSize(40);  //设置默认高度
    tableWidget->setColumnWidth(0,50);
    tableWidget->setColumnWidth(1,200);
    tableWidget->setColumnWidth(2,200);
    tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);//允许拉伸
    tableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    tableWidget->setWordWrap(false);//取消自动换行
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setShowGrid(false);
    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              border-right-width:0px; \
                                                              border-left-width:0px; \
                                                              }");
    tableWidget->setStyleSheet("QTableWidget{ \
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

    TableWidget = tableWidget;
    QGridLayout *layout = new QGridLayout(ui->widget_treeList);
    layout->setContentsMargins(16,10,16,10);
    layout->addWidget(TableWidget);

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
//    this->setAttribute(Qt::WA_DeleteOnClose);
}

treeListWindows::~treeListWindows()
{
    delete ui;
}

/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void treeListWindows::mousePressEvent(QMouseEvent *event)
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

void treeListWindows::mouseMoveEvent(QMouseEvent *event)
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

void treeListWindows::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

void treeListWindows::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    done(1);
}

void treeListWindows::on_pushButton_OK_clicked()
{
    this->close();
}
