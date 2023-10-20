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
#ifndef ITEMBROKERHEX_H
#define ITEMBROKERHEX_H
#include <QWidget>
#include <QItemDelegate>
#include <QLineEdit>
#include <QRegExp>
#include <QString>
class itemBrokerHex: public QItemDelegate
{
    Q_OBJECT
public:
    itemBrokerHex(QObject *parent = 0,QString regExp = NULL);
    ~itemBrokerHex(void);
public:
     QWidget *createEditor ( QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
     void setEditorData ( QWidget * editor, const QModelIndex & index ) const ;
     void setModelData ( QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const ;
private:
    QRegExp *currentRegExp;
    QString RegExpString;
};

#endif // ITEMBROKERHEX_H
