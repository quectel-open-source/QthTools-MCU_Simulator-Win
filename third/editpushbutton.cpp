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
#include "editpushbutton.h"
#include <QEvent>
#include <QDebug>
#include <QKeyEvent>
#include <QInputDialog>
#include "kernel/mmessagebox.h"
#include <QToolTip>
#include "../subwindow/inputdialog.h"

editPushButton::editPushButton(QWidget *parent):QPushButton(parent)
{
    this->installEventFilter(this);
}

editPushButton::editPushButton(const QString &text, QWidget *parent):QPushButton(parent)
{
    this->installEventFilter(this);
    this->setText(text);
    this->setMinimumSize(80, 36);
    this->setMaximumSize(80, 36);
//    this->setStyleSheet("");
}

bool editPushButton::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if(mouseEvent->button() == Qt::RightButton)
            {
    //            if(NULL == lineEdit)
    //            {
    //                lineEdit = new QLineEdit(this);
    //                lineEdit->setMaxLength(32);
    //                lineEdit->setText(this->text());
    //            }
    //            lineEdit->show();
                QString oldText;
                if(!this->text().isEmpty())
                {
                    oldText.append(this->text());
                }
                bool ok;
                QString text = InputDialog::getText(tr("请输入备注"),oldText, &ok, tr("确定"), tr("取消"));
//                QString text = QInputDialog::getText(0, tr("提示"),tr("请输入备注"), QLineEdit::Normal,oldText, &ok,Qt::WindowCloseButtonHint);
                if (ok && !text.isEmpty()&& text.size() <= 10)
                {
                    this->setText(text);
                    QString index = this->objectName().split('_').last();
                    emit buttonTextChange(index.toInt(),text);
                }
                else if (text.size() > 10)
                {
                    MMessageBox::critical(0,tr("错误"),tr("输入文本的长度不能大于10"),tr("确认"));
                }
                else if(text.isEmpty())
                {
                    MMessageBox::critical(0,tr("错误"),tr("输入文本的长度不能为空"),tr("确认"));
                }
                return true;
            }
        }
        else
        {
//            qDebug()<<"this->geometry():"<<this->geometry();
//            qDebug()<<"QCursor::pos():"<<QCursor::pos();
            if(event->type() == QEvent::HoverEnter)
            {
                if(text().length() > 5)
                {
//                    if(mouseHoverSec > 10 && !isShowTootip)
//                    {
                        QToolTip::showText(QCursor::pos(), text());
//                        mouseHoverSec = 0;
//                    }
//                    else
//                    {
//                        mouseHoverSec++;
//                    }
                }
            }
        }
    }
//    if(event->type() == QEvent::KeyPress)
//    {
//        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
//        if(keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return )
//        {
//            this->setText(lineEdit->text());
//            lineEdit->hide();
//            return true;
//        }
//    }
    return QPushButton::eventFilter(watched,event);
}
