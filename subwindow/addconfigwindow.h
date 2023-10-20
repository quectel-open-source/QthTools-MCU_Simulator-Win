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
#ifndef ADDCONFIGWINDOW_H
#define ADDCONFIGWINDOW_H

#include <QWidget>
#include <QAbstractButton>
#include "kernel/configprocess.h"
#include "kernel/httpclient.h"

namespace Ui {
class addConfigWindow;
}

class addConfigWindow : public QWidget
{
    Q_OBJECT

public:
    explicit addConfigWindow(QWidget *parent = nullptr);
    ~addConfigWindow();
    void retranslationUi();
private slots:
//    void on_buttonBox_clicked(QAbstractButton *button);
    void on_pushButton_file_clicked();
    void on_comboBox_data_currentIndexChanged(int index);
    void on_pushButton_OK_clicked();
    void on_pushButton_Cancel_clicked();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void on_pushButton_getFile_clicked();
    void on_checkBox_automatic_clicked(bool checked);

    void on_comboBox_environment_currentIndexChanged(int index);

private:
    Ui::addConfigWindow *ui;
    void closeEvent(QCloseEvent *event);
    bool m_bDrag = false;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;
    HttpClient *httpClient = NULL;

signals:
    void addConfigSignal(configInfo_t configInfo);
    void sendCloseEvent(QCloseEvent *event);
};

#endif // ADDCONFIGWINDOW_H
