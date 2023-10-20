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
#include "itembrokerhex.h"
#include <QDebug>
/**************************************************************************
** 功能	@brief :   item输入数据过滤
** 输入	@param :
** 输出	@retval:
***************************************************************************/
itemBrokerHex::itemBrokerHex(QObject *parent, QString regExp):QItemDelegate(parent)
{
    currentRegExp = new QRegExp();
    RegExpString.append(regExp);

}

itemBrokerHex::~itemBrokerHex()
{

}

QWidget *itemBrokerHex::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    //添加代理控件
    QLineEdit *editor =  new  QLineEdit(parent);
    if(RegExpString.isEmpty())
    {
        QRegExp reg("^[a-fA-F0-9]{0,2}$");
        currentRegExp->swap(reg);
    }
    editor->setValidator( new QRegExpValidator(*currentRegExp, parent));
    return  editor;
}

void itemBrokerHex::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //将单元格中的文本写入代理控件
    QString text = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit *lineEdit =  static_cast <QLineEdit*>(editor);
    lineEdit->setStyleSheet("color:blue;");
    lineEdit->setText(text);
}

void itemBrokerHex::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    //将代理控件的文本写入单元格
    QLineEdit *lineEdit =  static_cast <QLineEdit*>(editor);
    QString text = lineEdit->text();
    model->setData(index, text, Qt::EditRole);
}
