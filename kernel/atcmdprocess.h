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
#ifndef ATCMDPROCESS_H
#define ATCMDPROCESS_H

#include <QObject>
#include <QTimer>
typedef enum
{
    ATCMD_SET = 0,
    ATCMD_GET = 1,
    ATCMD_TEST = 2,
    ATCMD_EXEC = 3,
    ATCMD_ERROR = 4,
}atcmd_type_e;

typedef enum
{
    ATCMD_EVENT_SEND,
    ATCMD_EVENT_SENDHEX,
    ATCMD_EVENT_STATUS,
    ATCMD_EVENT_REG_GET,
    ATCMD_EVENT_PASSDATA_ISSUE,
    ATCMD_EVENT_PASSDATA_READ,
    ATCMD_EVENT_PASSDATA_TEST,
    ATCMD_EVENT_MODEL_TEST,
    ATCMD_EVENT_MODEL_ISSUE,
    ATCMD_EVENT_MODEL_GET,
    ATCMD_EVENT_MODEL_READ,
    ATCMD_EVENT_MODEL_SEND,
    ATCMD_EVENT_INFO_GET,
    ATCMD_EVENT_MCUVER_GET,
    ATCMD_EVENT_MCUVER_SET,
    ATCMD_EVENT_OTA,
    ATCMD_EVENT_OTAFILE_GET,
    ATCMD_EVENT_QIOTLOCIN,
    ATCMD_EVENT_QIOTLOCEXT,
/************ 网关子设备 **************/
    ATCMD_EVENT_QIOTSUBCONN,
    ATCMD_EVENT_QIOTSUBDISCONN,
    ATCMD_EVENT_QIOTSUBSEND,
    ATCMD_EVENT_QIOTSUBRD,
    ATCMD_EVENT_QIOTSUBRD_TEST,
    ATCMD_EVENT_QIOTSUBTSLTD,
    ATCMD_EVENT_QIOTSUBTSLRD,
    ATCMD_EVENT_QIOTSUBINFO,
    ATCMD_EVENT_QIOTSUBHTB,
    ATCMD_EVENT_QIOTSUBEVT,
/************ 配置信息获取 **************/
    ATCMD_EVENT_CONFIG_GET,
}atcmd_event_e;

typedef struct
{
    atcmd_type_e  type;
    QList<QByteArray> sendArgs;
    QString key;
    bool enable;
}sendInfo_t;


typedef void (*atRecvcb)(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
typedef struct EventInfo
{
    QString key;
    sendInfo_t sinfo;
    bool result;
    QList<QByteArray> recvData;
}eventStruct;


class atcmdProcess : public QObject
{
    Q_OBJECT
public:
    explicit atcmdProcess(QObject *parent = nullptr);
    QByteArray sendDataHex;
    bool sendDataHexFlag;
    bool sendMutexFlag;
    bool timeOutFlag;
    bool atcmdSendAnalysis(QByteArray data);
    int atcmdRecvAnalysis(QByteArray data,QByteArray &hexData);
    void atcmd_process(QString atcmd,atcmd_type_e type,QList<QByteArray> paraList,bool result);
    void atcmd_QIOTREG_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSEND_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTRD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTCFG_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTMCUVER_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTUPDATE_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTINFO_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTOTARD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSTATE_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTEVT_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTMODELTD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTMODELRD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTLOCIN_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTLOCEXT_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_UNKNOWN_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTOTAREQ_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QFOTAUP_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QFOTACFG_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    /************************************* 网关子设备 **************************************************/
    void atcmd_QIOTSUBCONN_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSUBDISCONN_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSUBSEND_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSUBRD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSUBTSLTD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSUBTSLRD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSUBINFO_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSUBHTB_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    void atcmd_QIOTSUBEVT_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
    /************************************* 网关子设备 **************************************************/
    void setNextFuncRun(QString key,sendInfo_t sinfo,bool result,QList<QByteArray> recvData);
    void getNextFuncRun(eventStruct *event);
    void runNextFunc(void);
    void runNextFuncAgain(void);
    void sendDataHexFunc();
//    void setSendDataHexFlag();
private slots:
    void dataRecvTimeout();
signals:
    void atEventSend(atcmd_event_e event,QList<QByteArray> paraList);
    void sendATDataEmit(QString data);
private:
    sendInfo_t sendInfo;
//    QTimer *dataRecvTimer;
//    QTimer *runNextTimer;

    QList<eventStruct>currentEventList;
};


#endif // ATCMDPROCESS_H
