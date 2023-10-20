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
#ifndef STRINGSHOW_H
#define STRINGSHOW_H

#include <QWidget>

namespace Ui {
class stringShow;
}

class stringShow : public QWidget
{
    Q_OBJECT

public:
    explicit stringShow(QWidget *parent = nullptr,QString data = NULL);
    ~stringShow();
signals:
    void dataChangeForStringWiget(QString,bool);
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();

private:
    Ui::stringShow *ui;
    bool m_bDrag = false;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;
};

#endif // STRINGSHOW_H
