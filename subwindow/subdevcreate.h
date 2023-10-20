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
#ifndef SUBDEVCREATE_H
#define SUBDEVCREATE_H

#include <QWidget>
#include "kernel/httpclient.h"

typedef struct
{
    QString pk;
    QString ps;
    QString buffer;
    QString lifetime;
    QString filePath;
    QString note;
    QString dk;
    QString ds;
    QString tls;
}subDevConfigInfo_t;


namespace Ui {
class subDevCreate;
}

class subDevCreate : public QWidget
{
    Q_OBJECT

public:
    explicit subDevCreate(QWidget *parent = nullptr);
    ~subDevCreate();
    void retranslationUi();
signals:
//    void sendSubDevData(QStringList data);
//    void addsubDevConfigSignal(subDevConfigInfo_t subDevInfo);
    void sendSubDevCreateInfo(subDevConfigInfo_t subDevInfo);
    void closeWidgetEvent();

private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void on_pushButton_subDev_ok_clicked();
    void on_pushButton_subDev_cancel_clicked();
    void on_comboBox_productMode_currentIndexChanged(int index);

    void on_pushButton_file_clicked();

    void on_checkBox_automatic_clicked(bool checked);

    void on_pushButton_getFile_clicked();

private:
    Ui::subDevCreate *ui;
    bool m_bDrag = false;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;
    void closeEvent(QCloseEvent *event);
    HttpClient *httpClient = NULL;
};

#endif // SUBDEVCREATE_H
