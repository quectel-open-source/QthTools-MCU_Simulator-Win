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
#include "newcombobox.h"
#include <QListView>
//#include <QDebug>
/**************************************************************************
** 功能	@brief :   enum类型数据的key：value类型读写
** 输入	@param :
** 输出	@retval:
***************************************************************************/
newComboBox::newComboBox(QWidget *parent, int comboxValue)
{
    Q_UNUSED(parent);
    this->comboxValue = comboxValue;
    this->setView(new QListView());
    this->setStyleSheet("QComboBox{\
                        color: rgb(104, 112, 130);\
                        font-size:14px;\
                        border-style:solid;\
                        background-color: rgb(245, 245, 245);\
                        border-width:1px;\
                        border-color:rgb(229, 231, 234);\
                        border-radius:4px;\
                        padding-left:5px;\
                        }\
                        QComboBox:disabled\
                        {\
                        color: rgb(180, 180, 180);\
                        font-size:14px;\
                        border-style:solid;\
                        background-color: rgb(220, 220, 220);\
                        border-width:1px;\
                        border-color:rgb(229, 231, 234);\
                        border-radius:4px;\
                        padding-left:5px;\
                        }\
                        QComboBox::drop-down{\
                        border-style:none;\
                        width: 30px;\
                        }\
                        QComboBox::down-arrow {\
                        image: url(:/png/QComboBox_down.png);\
                        }\
                        QComboBox QAbstractItemView::item\
                        { \
                            color: rgb(54, 65, 89);\
                            height: 25px;  \
                        }");
}

int newComboBox::getComboxItemIdex(int value)
{
    int currentValue = 0;
    for(int i =0;i<itemValue.count();i++)
    {
        if(itemValue.at(i) == value)
            currentValue = i;
    }
    return currentValue;
}

int newComboBox::getComboxItemValue(int idex)
{
    if(idex >= itemValue.count())
        return 0;
    return itemValue.at(idex);
}

void newComboBox::addComboxItemValue(int value)
{
    this->itemValue.append(value);
}
