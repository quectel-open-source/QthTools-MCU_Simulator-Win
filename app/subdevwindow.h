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
#ifndef SUBDEVWINDOW_H
#define SUBDEVWINDOW_H
#include "mainwindow.h"
#include "subwindow/subdevcreate.h"
namespace Ui {
class subDevwindow;
}

typedef struct
{
    QString PK;
    QString DK;
    int currentLifeTime;
    int lifeTime;
    bool status;
}liveSendDataStruct;

class subDevwindow : public QWidget
{
    Q_OBJECT

public:
    explicit subDevwindow(QWidget *parent = nullptr);
    ~subDevwindow();
    void setParm(void *father);
    void uiAttributeSet();
    void tableWidgetAddRow(subDevConfigInfo_t subDevConfigInfo,int mode);
    void handleSubDevEvent(QList<QByteArray> paraList);
    void saveSubDevDs(QList<QByteArray> paraList);
    void handleSubDevPassDataRecv(QList<QByteArray> paraList);
    void handleSubDevModelDataRecv(QList<QByteArray> paraList);
    QList<int> getCurrentCheckRow();
    void delLiveList(QString pk,QString dk);
    void setPassReadMaxCount(int value);
    void closeLiveTimer();
    bool retranslationUi();

private slots:
    void subDevConnectSlot();
    void on_pushButton_addSubDev_clicked();
    void recvSubDevCreateInfo(subDevConfigInfo_t subDevConfigInfo);
    void on_pushButton_delSubDev_clicked();
    void on_tableWidget_subDev_cellDoubleClicked(int row, int column);
    void on_pushButton_subDevConnect_clicked();
    void on_pushButton_subDevDisconnect_clicked();
    void headViewDoubleClickEvent(int idex);
    void liveListCheck();
    void closeSubWidget();
    void handleSendLiveData(QString pk,QString dk);
    void ATSendPassData(QByteArray data);
    void ATSendDirectlyData(QByteArray data,bool externFlag);
    void ATRecvPassData(int value);
    void ATSendModuleData(QByteArray data);
    void ATSendModuleData(QByteArray data,int value);
    void ATRecvModuleData(int mode=1);
    void ATSubSendData(QByteArray data,bool externFlag);
    void switchButtonLiveChange(bool checked);
    void switchButtonChange(bool checked);
    void on_pushButton_findSubDev_clicked();
    void on_pushButton_subDevCommunite_clicked();
    void on_pushButton_clearDs_clicked();

private:
    void addSendLiveDataList(QString pk,QString dk,int lifeTime,bool status);
signals:
    void sendLiveData(QString pk,QString dk);
private:
    Ui::subDevwindow *ui;
    MainWindow *father;
    bool subWidgetIsVaild;
    QWidget *commWidget = NULL;
    QWidget *createWidget = NULL;
    QList<liveSendDataStruct>sendLiveList;
    QByteArray waitSendData;
    QObject *userInfoSql;
    QTimer *liveListCheckTimer;
    int searchHistoryRow;
    QMutex liveMutex;
};

#endif // SUBDEVWINDOW_H
