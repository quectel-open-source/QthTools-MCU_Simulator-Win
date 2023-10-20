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
#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include "mainwindow.h"
#include "subwindow/addconfigwindow.h"

namespace Ui {
class configWindow;
}

class configWindow : public QWidget
{
    Q_OBJECT

public:
    explicit configWindow(QWidget *parent = nullptr);
    ~configWindow();
    void setParm(void *father);
    void uiAttributeSet();
    void searchPK();
    void searchNextPK();
    void handleConfigGetEvent(QList<QByteArray> paraList);
    void getTslValue();
    bool retranslationUi();
    void hideATcmdWidget();
    void restoreProductInformation();
    void getProductInformation();
    void configWindowProductGet();
    void configWindowProductSet();

private slots:
    void on_pushButton_configAdd_clicked();
    void on_pushButton_configDel_clicked();
    void on_search_pushButton_clicked();
    void CurrentConfigInfoChange();
    void addConfigSlot(configInfo_t configInfo);
    void writeItemNote(QTableWidgetItem *item);
    void addconfigwindowCloseEvent(QCloseEvent *event);
	void handleAtAck(QString cmdData);
    void showTimerTimeout();

private:
    Ui::configWindow *ui;
    MainWindow *father;
    configInfo_t currentConfigInfo;
    addConfigWindow *addconfigwindow = NULL;
    qint16 searchProductInfoHistoryRow;
    QString searchProductInfoHistoryText;
    bool isFirstTime = true;
    QTimer *showTimer = NULL;

signals:
    void configChangeSignal(QString);
    void setProductConfigSignal(QString,QString,QString);
    void restoreProductInformationSignal(QString,QString,QString);
};

#endif // CONFIGWINDOW_H
