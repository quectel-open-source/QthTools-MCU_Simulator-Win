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
#ifndef PRODUCTCONFIG_H
#define PRODUCTCONFIG_H

#include <QWidget>

namespace Ui {
class ProductConfig;
}

class ProductConfig : public QWidget
{
    Q_OBJECT

public:
    explicit ProductConfig(QWidget *config,QWidget *parent = nullptr);
    ~ProductConfig();

public slots:
    void productSetSlots();
    void productGetSlots();

private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void on_pushButton_Cancel_clicked();
    void on_pushButton_OK_clicked();

private:
    Ui::ProductConfig *ui;
    bool m_bDrag = false;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;
    QWidget *configwindow;
    int selectedRow = 0;
};

#endif // PRODUCTCONFIG_H
