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
#ifndef ATCMDWINDOW_H
#define ATCMDWINDOW_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class ATcmdWindow;
}

class ATcmdWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ATcmdWindow(int mode = 0,QWidget *parent = nullptr);
    ~ATcmdWindow();
    void setParm(void *father);
    void handleConfigGetEvent(QList<QByteArray> paraList);
    void getTslValue();
    bool retranslationUi();

private slots:
    bool eventFilter(QObject *obj, QEvent *event);
    void on_pushButton_server_clicked();
    void on_pushButton_server_get_clicked();
    void on_pushButton_buffer_clicked();
    void on_pushButton_lifetime_clicked();
    void on_pushButton_contextId_clicked();
    void on_pushButton_format_clicked();
    void on_pushButton_sessionMode_clicked();
    void on_pushButton_dkds_clicked();
    void on_pushButton_gwMode_clicked();
    void on_pushButton_psk_clicked();
    void on_pushButton_txidMode_clicked();
    void on_pushButton_buffer_get_clicked();
    void on_pushButton_lifetime_get_clicked();
    void on_pushButton_contextId_get_clicked();
    void on_pushButton_format_get_clicked();
    void on_pushButton_sessionMode_get_clicked();
    void on_pushButton_dkds_get_clicked();
    void on_pushButton_psk_get_clicked();
    void on_pushButton_txidMode_get_clicked();
    void on_pushButton_gwMode_get_clicked();
    void on_comboBox_server_currentIndexChanged(int index);
    void handleAtAck(QString cmdData);
    void on_comboBox_protocol_activated(const QString &arg1);

private:
    Ui::ATcmdWindow *ui;
    MainWindow *father;
    int userMode;
    configInfo_t currentConfigInfo;

};

#endif // ATCMDWINDOW_H
