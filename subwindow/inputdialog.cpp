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
#include "inputdialog.h"
#include "ui_inputdialog.h"
#include <QCloseEvent>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

InputDialog::InputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDialog)
{
    ui->setupUi(this);
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
    ui->textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

InputDialog::~InputDialog()
{
    delete ui;
}

/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void InputDialog::mousePressEvent(QMouseEvent *event)
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

void InputDialog::mouseMoveEvent(QMouseEvent *event)
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

void InputDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

QString InputDialog::getText(QString titleText, QString oldText, bool *isOk, QString bottonOK, QString bottonCancel)
{
    InputDialog *inputDialog = new InputDialog();
    inputDialog->setWindowTitle(titleText);
    if (bottonOK != NULL)
    {
        inputDialog->ui->pushButton_OK->setText(bottonOK);
    }
    if (bottonCancel != NULL)
    {
        inputDialog->ui->pushButton_Cancel->setText(bottonCancel);
    }

    inputDialog->ui->label_windowTitle->setText(titleText);
    inputDialog->ui->textEdit->setText(oldText);
    if (inputDialog->exec() == QDialog::Accepted)
    {
        *isOk = true;
        return inputDialog->ui->textEdit->toPlainText();
    }
    *isOk = false;
    return oldText;
}

QString InputDialog::getText2(QString titleText, bool *isOk, QString bottonOK, QString bottonCancel)
{
    InputDialog *inputDialog = new InputDialog();
    if (bottonOK != NULL)
    {
        inputDialog->ui->pushButton_OK->setText(bottonOK);
    }
    if (bottonCancel != NULL)
    {
        inputDialog->ui->pushButton_Cancel->setText(bottonCancel);
    }
    QString rString;
    inputDialog->ui->label_windowTitle->setText(titleText);
    if (inputDialog->exec() == QDialog::Accepted)
    {
        *isOk = true;
        rString = inputDialog->ui->textEdit->toPlainText();
    }
    else
    {
        *isOk = false;
    }
    return rString;
}

void InputDialog::on_pushButton_Cancel_clicked()
{
    done(Rejected);
}

void InputDialog::on_pushButton_OK_clicked()
{
    done(Accepted);
}
