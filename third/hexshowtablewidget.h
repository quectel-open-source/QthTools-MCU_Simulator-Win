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
#ifndef HEXSHOWTABLEWIDGET_H
#define HEXSHOWTABLEWIDGET_H

#include <QTableWidget>
#include <QKeyEvent>
#include <QTimer>
class hexShowTableWidget:public QTableWidget
{
    Q_OBJECT
public:
    explicit hexShowTableWidget(QWidget *parent = 0);
    void keyReleaseEvent(QKeyEvent  *event);
    void EnterNextEvent(bool flag=false);
private slots:
    void showData(QTableWidgetItem *item);
private:
    void setAppearance();
    int columSum;
    int keyInputSum;
};

#endif // HEXSHOWTABLEWIDGET_H
