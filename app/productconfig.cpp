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
#include "productconfig.h"
#include "configwindow.h"
#include "ui_productconfig.h"
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include "atcmdwindow.h"

QWidget *pconfig;

ProductConfig::ProductConfig(QWidget *config,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProductConfig)
{
    ui->setupUi(this);
    //去除窗体边框
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    //窗体关闭信号
    connect(ui->pushButton_close, &QPushButton::clicked, [=]() {close(); });
    this->setWindowModality(Qt::ApplicationModal);

    pconfig = config;
    QGridLayout *layoutAtcmd = new QGridLayout(ui->widget_config);
    layoutAtcmd->setContentsMargins(0,0,0,0);
    layoutAtcmd->addWidget(pconfig);

    this->setAttribute(Qt::WA_TranslucentBackground, true);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#C0C0C0"));
    shadow->setBlurRadius(15);
    shadow->setOffset(5);
    ui->widget_2->setGraphicsEffect(shadow);
    ui->pushButton_Cancel->hide();
//    ui->pushButton_close->hide();
}

ProductConfig::~ProductConfig()
{
    delete ui;
}

/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ProductConfig::mousePressEvent(QMouseEvent *event)
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

void ProductConfig::mouseMoveEvent(QMouseEvent *event)
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

void ProductConfig::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
//    ((configWindow*)pconfig)->getProductInformation();
}

void ProductConfig::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

void ProductConfig::on_pushButton_Cancel_clicked()
{
    this->close();
}

void ProductConfig::on_pushButton_OK_clicked()
{
    ((configWindow*)pconfig)->getProductInformation();
}

void ProductConfig::productSetSlots()
{
    ((configWindow*)pconfig)->configWindowProductSet();
}

void ProductConfig::productGetSlots()
{
    ((configWindow*)pconfig)->configWindowProductGet();
}
