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
#ifndef HEXSTRINGSHOW_H
#define HEXSTRINGSHOW_H

#include <QWidget>

namespace Ui {
class hexStringShow;
}

class hexStringShow : public QWidget
{
    Q_OBJECT

public:
    explicit hexStringShow(QWidget *parent = nullptr,QString data = NULL);
    ~hexStringShow();
//    bool eventFilter(QObject *target,QEvent *event);
signals:
    void dataChangeForHexWiget(QByteArray data,bool changeMode);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void on_pushButton_ok_clicked();
    void on_pushButton_clearLog_clicked();
    void on_tableWidget_showHex_cellChanged(int row, int column);

    void on_pushButton_cancel_clicked();

private:
    Ui::hexStringShow *ui;
    bool m_bDrag = false;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;
    int itemsSum;
};

#endif // HEXSTRINGSHOW_H
