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
#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H


#include <QCompleter>
#include "mainwindow.h"
#include "third/completertextedit.h"
#include "productconfig.h"
#include <QTimer>

namespace Ui {
class debugWindow;
}

class debugWindow : public QWidget
{
    Q_OBJECT

public:
    explicit debugWindow(QWidget *parent = nullptr);
    ~debugWindow();
    void setParm(void *father);
    void uiAttributeSet(void);
    void ATCMDSend(QString data);
    void ATCMDSend(QByteArray data,bool externFlag);
    void ATCMDSendHex(QByteArray hexData);
    void ATCMDSend(QString strData,QByteArray hexData);
    void csdkConnectStatus(int status);
    void localIpListUpdate(void);
    void debugfileSave(QString data,int currentEvenValue);
    bool tcpConnect();
    void resetUIValue();
    void threadRun();
    void handleTcpData(QString data,QString hexData);
    void closeSocket();
    void setDMPConnectStatus(int mode);
    bool retranslationUi();
    void openLogTime(bool isCheck);

public:
    bool uartIsOpen = false;
    bool tcpIsOpen = false;

private:
    Ui::debugWindow *ui;
    MainWindow *father;
    QStandardItemModel *model_atInput;
    QCompleter * completer_atInput;
    CompleterTextEdit *textEdit_atInput;
    qSerial *qserial;
    fileManager *logfile = NULL;
    fileManager *debugfile;
    QTimer uartTimer;
    QTcpSocket *socket;
    int MaxDebugEvenValue;
    QReadWriteLock lock;
    int reConnectSum;
    QTimer *tcpConnectTimer;
    bool tcpDataHandleFlag;
    QTimer *tcpRecvDelaytimer;
    int handleDataError;
    int oldDtaLength;
    int errorHandleRecvData;
    ProductConfig *productconfig;
    QTimer *sendSTL = NULL;

private:
    void createThread();
    void nonBlockingSleep(int sectime);
    bool checkIsConnectDev();
    QString handleDisplayData(QByteArray array);

public slots:
    void ATCMDSignalSend(QString data);

private slots:
    bool eventFilter(QObject *obj, QEvent *event);
    void portListNoticSlot(QList<QString> portList);
    void dataReadNoticSlot(QByteArray recvData);
    void uartRecvTimeout();
    void socket_Read_Data(void);
    void socket_Disconnected(void);
    void on_pushButton_uartOpen_clicked();
    void on_pushButton_runStart_clicked();
    void on_pushButton_atSend_clicked();
    void on_pushButton_logFile_clicked();
    void on_pushButton_logClear_clicked();
    void on_checkBox_logSave_toggled(bool checked);
    void on_pushButton_tcp_clicked();
    void on_pushButton_status_clicked();
    void on_checkBox_logTime_stateChanged(int arg1);
    void on_comboBox_debugSelect_currentIndexChanged(int index);
    void tcpConnectTimeout();
    void tcpRecvDataHandle();
    void on_pushButton_clearDebugFile_clicked();
    void tcpRecvTimeout();
    void on_tabWidget_currentChanged(int index);
    void closeDmpConnect();
    void on_pushButton_modeCom_clicked();
    void on_pushButton_modeTcp_clicked();
    void on_pushButton_productConfig_clicked();
    void setProductConfigSlot(QString pk, QString ps, QString model);
    void restoreProductInformationSlot(QString pk, QString ps, QString model);
    void on_pushButton_productGet_clicked();
    void on_pushButton_productSet_clicked();
    void sendSTLAt();

signals:
    void textEditNotiSignal(QString data);
    void TcpRecvData();
    void showProductConfigWindow();
    void closeProductConfigWindow();
    void productSetSignal();
    void productGetSignal();
};


#endif // DEBUGWINDOW_H
