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
#ifndef TREELISTWINDOWS_H
#define TREELISTWINDOWS_H

#include <QDialog>
#include <QTableWidget>

namespace Ui {
class treeListWindows;
}

class treeListWindows : public QDialog
{
    Q_OBJECT

public:
    explicit treeListWindows(QTableWidget *tableWidget, QWidget *parent = nullptr);
    ~treeListWindows();
    QTableWidget *getTableWidget();
    QTableWidget *TableWidget;

private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void on_pushButton_OK_clicked();

private:
    Ui::treeListWindows *ui;
    bool m_bDrag = false;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;
};

#endif // TREELISTWINDOWS_H
