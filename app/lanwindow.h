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
#ifndef LANWINDOW_H
#define LANWINDOW_H

#include <QWidget>
#include "mainwindow.h"

typedef struct
{
    QString name;
    QString password;
}wifiInfo;

typedef struct
{
    wifiInfo g_wifi;
    wifiInfo g_ap;
    QString bindcode;
}lanInfo;

namespace Ui {
class lanwindow;
}

class lanwindow : public QWidget
{
    Q_OBJECT

public:
    explicit lanwindow(QWidget *parent = nullptr);
    ~lanwindow();
    void setParm(void *father);
    void resetUIValue();
    bool retranslationUi();

private slots:
    bool eventFilter(QObject *obj, QEvent *event);
    void on_pushButton_close_clicked();
    void on_pushButton_open_clicked();
    void on_pushButton_set_AP_clicked();
    void on_pushButton_set_WIFI_clicked();
    void on_pushButton_set_bindcode_clicked();
    void on_pushButton_set_clear_clicked();
    void on_pushButton_set_reset_clicked();

private:
    Ui::lanwindow *ui;
    MainWindow *father;
    lanInfo g_lan;
};

#endif // WIFIWINDOW_H
