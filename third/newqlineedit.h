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
#ifndef NEWQLINEEDIT_H
#define NEWQLINEEDIT_H
#include <QLineEdit>
#include "subwindow/hexstringshow.h"
#include "subwindow/stringshow.h"

class newQLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit newQLineEdit(QWidget *parent = 0);
protected:
    //重写mousePressEvent事件
//    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    //自定义clicked()信号,在mousePressEvent事件发生时触发
    void clicked();

private slots:
    void handleClicked();
    void recvDataForHexWiget(QByteArray data,bool recvMode);
    void recvDataForStringWiget(QString data,bool recvMode);
private:
    bool clickedCheckFlag;
    hexStringShow *hexStringShowWidget;
    stringShow *stringShowWidget;
};

#endif // NEWQLINEEDIT_H
