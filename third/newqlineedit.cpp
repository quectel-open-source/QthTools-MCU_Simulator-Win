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
#include "newqlineedit.h"
#include <QMouseEvent>
#include <QDebug>
#include "kernel/mmessagebox.h"
#include <QRegExpValidator>
#include "userconfigure.h"
//HKL hCurKL= NULL;
/**************************************************************************
** 功能	@brief :  text类型数据读写
** 输入	@param :
** 输出	@retval:
***************************************************************************/
newQLineEdit::newQLineEdit(QWidget *parent):QLineEdit(parent)
{
    clickedCheckFlag = false;
    this->setClearButtonEnabled(true);
    this->setStyleSheet("color: rgb(104, 112, 130);\
                        font-size:14px;\
                        border-style:solid;\
                        border-width:1px;\
                        border-height：20px;\
                        border-color:rgb(229, 231, 234);\
                        border-radius:4px;\
                        padding-left:5px;\
                        ");
    connect(this,SIGNAL(clicked()),this,SLOT(handleClicked()));
    this->setMaxLength(4096);
//    this->setReadOnly(true);
    stringShowWidget = NULL;
    hexStringShowWidget = NULL;
}

void newQLineEdit::mousePressEvent(QMouseEvent *event)
{
    //如果单击了就触发clicked信号
    if (event->button() == Qt::LeftButton)
    {
        //触发clicked信号
        emit clicked();
    }
//    //将该事件传给父类处理
//    QLineEdit::mousePressEvent(event);
}

//void newQLineEdit::mouseDoubleClickEvent(QMouseEvent *event)
//{
//    //如果单击了就触发clicked信号
//    if (event->button() == Qt::LeftButton)
//    {
//        //触发clicked信号
//        emit clicked();
//    }
////    //将该事件传给父类处理
////    QLineEdit::mousePressEvent(event);
//}

void newQLineEdit::handleClicked()
{
    if(clickedCheckFlag == false)
    {
        int ret = MMessageBox::information(0,tr("提示"),tr("是否为字节流"),tr("字节流"),tr("字符串"),tr("取消"));
        if(ret == 0)
        {
            this->setToolTip(tr("字节流数据"));
//            if(hexStringShowWidget != NULL)
//            {
//                delete(hexStringShowWidget);
//                hexStringShowWidget = NULL;
//            }
            if(!this->text().isEmpty())
            {

                if(this->text().mid(0,2).compare("\\x") != 0)
                {
                    MMessageBox::critical(0,tr("错误"),tr("导入的数据为非16进制数据"),tr("确认"));
                    return;
                }
                QRegExp valueRegExp("[a-fA-F0-9]");
                valueRegExp.setCaseSensitivity(Qt::CaseInsensitive);
                QString hexString = this->text().mid(2);
                bool ret = hexString.contains(valueRegExp);
                if(ret == false)
                {
                    MMessageBox::critical(0,tr("错误"),tr("导入的数据为非16进制数据"),tr("确认"));
                    return;
                }

                hexStringShowWidget = new hexStringShow(NULL,hexString);

            }
            else
            {
                hexStringShowWidget = new hexStringShow();
            }
//            hexStringShowWidget->setAttribute(Qt::WA_QuitOnClose,false);
//            //强制英文输入法
//            hCurKL = GetKeyboardLayout(0);
//            LoadKeyboardLayout((LPCWSTR)QString("0x0804").utf16(), KLF_ACTIVATE);
            hexStringShowWidget->setAttribute(Qt::WA_QuitOnClose, false);
            connect(hexStringShowWidget,SIGNAL(dataChangeForHexWiget(QByteArray,bool)),this,SLOT(recvDataForHexWiget(QByteArray,bool)));
            hexStringShowWidget->show();
            clickedCheckFlag = true;
        }
        else if(ret == 1)
        {
            this->setToolTip(tr("字符串数据"));
            if(stringShowWidget != NULL)
            {
                delete(stringShowWidget);
                stringShowWidget = NULL;
            }
            if(!this->text().isEmpty())
            {
                if(this->text().startsWith("\\\\x"))
                    stringShowWidget = new stringShow(NULL,this->text().mid(1));
                else
                    stringShowWidget = new stringShow(NULL,this->text());
            }
            else
                stringShowWidget = new stringShow();
            stringShowWidget->setAttribute(Qt::WA_QuitOnClose,false);
            connect(stringShowWidget,SIGNAL(dataChangeForStringWiget(QString,bool)),this,SLOT(recvDataForStringWiget(QString,bool)));
            stringShowWidget->show();
            clickedCheckFlag = true;
        }
        else
            this->clearFocus();
    }
    else
    {
        MMessageBox::critical(this,tr("错误"),tr("不能同时打开字符串和字节流界面"),tr("确定"));
    }
}
/* recvMode 1 : 正常方式*/
/* recvMode 0 : 异常方式*/
void newQLineEdit::recvDataForHexWiget(QByteArray data,bool recvMode)
{
    this->setObjectName("byte");
    if(recvMode && !data.isEmpty())
    {
        QString dstData;
        dstData.append("\\x");
        dstData.append(QString(data).toUpper());
        this->setText(dstData);
    }
    else if(recvMode)
    {
        this->setText("");
    }
    hexStringShowWidget->disconnect(hexStringShowWidget,SIGNAL(dataChangeForHexWiget(QByteArray,bool)),this,SLOT(recvDataForHexWiget(QByteArray,bool)));
    hexStringShowWidget->close();
//    ActivateKeyboardLayout(hCurKL, KLF_SUBSTITUTE_OK); //还原输入法
    clickedCheckFlag = false;
}

void newQLineEdit::recvDataForStringWiget(QString data,bool recvMode)
{
    this->setObjectName("string");
    if(true == recvMode && !data.isEmpty())
    {
        this->setText(data);
    }
    stringShowWidget->disconnect(stringShowWidget,SIGNAL(dataChangeForStringWiget(QString,bool)),this,SLOT(recvDataForStringWiget(QString,bool)));
    stringShowWidget->close();
    clickedCheckFlag = false;
}

