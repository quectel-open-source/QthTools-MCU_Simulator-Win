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
#ifndef MODELWINDOW_H
#define MODELWINDOW_H

#include <QWidget>
#include "mainwindow.h"
#include "passwindow.h"
#include <QListWidget>
namespace Ui {
class modelWindow;
}

class modelWindow : public QWidget
{
    Q_OBJECT

public:
    explicit modelWindow(int mode = 0,QWidget *parent = nullptr);
    ~modelWindow();
    void setParm(void *father);
    void csdkModelIssue(QByteArray data);
    void csdkModelRead(QString readByte,QString readCount,QByteArray data);
    void csdkModelGet(int pkgId,QList<QByteArray> idList);
    void uiAttributeSet();
    void resetUIValue();
    int getReadCurrentCount();
    void listWidgetShow(QStringList data, QRect showPoint);
    void listWidgetCheckClose();
    void setModelReadMaxLen(int len);
	void setLabelShowTls(bool mode);
    bool getTlsStatus();
    bool retranslationUi();

private slots:
    void on_pushButton_sendTtlv_clicked();
    void on_pushButton_sendAck_clicked();
    void on_pushButton_readTtlv_clicked();
    void reportModelTimeout();
    void on_checkBox_timer_toggled(bool checked);
//    void slotCheckStatusChange(bool checked);
    void on_spinBox_timerMs_valueChanged(int arg1);
//    void on_pushButton_recvModeSelect_clicked();
    void itemClickedHandle(QListWidgetItem *item);
//    void on_pushButton_sendModeSelect_clicked();
    void mousePressEvent(QMouseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void leaveEvent(QEvent *e);
	void recvAtAck(QString cmdData);
    void on_pushButton_properties_clicked();
    void on_pushButton_services_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_readMaxTtlv_clicked();

public slots:
    void modelTableUpdate(QString filePath);

private:
    void sendDataToQuec(QByteArray data,bool externFlag);
    void sendDataToQuec_Pkgid(QByteArray data,bool externFlag);

private:
    Ui::modelWindow *ui;
    MainWindow *father;
    QTimer reportTimer;
    modelProcess *modelprocess;
    QByteArray dataCache;
    int userMode;
    newListWiget *listWidget = NULL;
    passWindow *passwindow = NULL;
};

#endif // MODELWINDOW_H
