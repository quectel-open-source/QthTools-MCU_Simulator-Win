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
#ifndef TTLVANYLSIS_H
#define TTLVANYLSIS_H
#include <QWidget>
#include <QVariant>
#include<QString>
#include <QMessageBox>
#include <qDebug>
#include "QJsonDocument"
#include "QJsonParseError"
#include "QJsonObject"
#include "QJsonArray"
#include "QJsonValue"

enum
{
    DP_TTLV_TYPE_BOOL_FALSE = 0, /* 布尔值false */
    DP_TTLV_TYPE_BOOL_TRUE,      /* 布尔值true */
    DP_TTLV_TYPE_ENUM_NUM,       /* 枚举和数值 */
    DP_TTLV_TYPE_BINARY,         /* 二进制数据 */
    DP_TTLV_TYPE_STRUCT,         /* 结构体 */
};


enum TTLV_DATA_TYPE
{
    TYPE_NONE_PROTOCOL_HEAD = 0,
    TYPE_HAVE_PROTOCOL_HEAD,
};


enum CMD_LIST
{
    CMD_TTLV_STATUS_GET         = 0x0011,
    CMD_TTLV_STATUS_ACK         = 0x0012,
    CMD_TTLV_DATA_GET           = 0x0013,
    CMD_TTLV_DATA_REPORT        = 0x0014,

    CMD_DATA_GET                = 0x0023,
    CMD_DATA_REPORT             = 0x0024,

    CMD_DEV_STATUS_GET          = 0x0031,
    CMD_DEV_STATUS_ACK          = 0x0032,
    CMD_DEV_STATUS_REPORT       = 0x0034,

    CMD_DEV_INFO_GET            = 0x0041,
    CMD_DEV_INFO_ACK            = 0x0042,
    CMD_DEV_INFO_REPORT         = 0x0044,

    CMD_ERROR_GET               = 0x00A3,
    CMD_ERROR_REPORT            = 0x00A4,

    CMD_DEV_MANAGE              = 0x00B3,
    CMD_DEV_CONFIGURE           = 0x00B4,
    CMD_DEV_REPORT_MANAGER      = 0x00B5,
    CMD_DEV_REPORT_MANAGER_ACK  = 0x00B6,
    CMD_DEV_CONFIGURE_REPORT    = 0x00B7,

    CMD_SUB_DEV_AUTH            = 0x00C0,
    CMD_SUB_DEV_AUTH_ACK        = 0x00C1,
    CMD_SUB_DEV_LOGIN           = 0x00C2,
    CMD_SUB_DEV_LOGIN_ACK       = 0x00C3,
    CMD_SUB_DEV_LOGOFF          = 0x00C4,
    CMD_SUB_DEV_LOGOFF_ACK      = 0x00C5,
    CMD_SUB_DEV_LOGOUT          = 0x00C6,
    CMD_SUB_DEV_LOGOUT_ACK      = 0x00C7,
    CMD_SUB_DEV_LOGOFF_NOTE     = 0x00C8,
    CMD_SUB_DEV_RESET           = 0x00C9,
    CMD_SUB_DEV_RESET_ACK       = 0x00CA,
    CMD_SUB_DEV_VERIFY          = 0x00CB,

    CMD_OTA_PLAN                = 0x0111,
    CMD_OTA_PLAN_ACK            = 0x0112,
    CMD_OTA_URL                 = 0x0113,
    CMD_OTA_STATUS_REPORT       = 0x0114,
    CMD_OTA_REQUEST             = 0x0115,

    CMD_LOCAL_REPORT_TYPE       = 0x0121,
    CMD_LOCAL_REPORT_DATA       = 0x0122,
    CMD_LOCAL_GET_TYPE          = 0x0123,
    CMD_LOCAL_GET_TYPE_ACK      = 0x0124,
};



typedef struct
{
    int idex;
    QString type;
    QVariant data;
    QVariant externMess;
}newModelStruct;

typedef struct newModeInfo
{
    int level;
    int parentId;
    struct newModeInfo *head;
    QList<newModelStruct>modeInfo;
}newModelListStruct;

class ttlvDataAnysis:public QObject
{
    Q_OBJECT
public:
    ttlvDataAnysis();
    bool analyzeTtlvDataNode(QByteArray data,int *offset,QJsonObject &jsonNode);
    void handleAnalyzeResult(newModelListStruct *list);
    void anlyzeTtlvData(QByteArray data);
    int anlyzeTtlvType(QByteArray data);

    char intGetChar(uint64_t Number,int position,int NumSize);
    QByteArray IntToByte(uint64_t Number, int NumSize);
    int getDoubleAccuracy(double tempDouble);
    int getArrayMax(int data[],int count);
    uint8_t dataSum(QByteArray data);
    QString getMapString(int cmd);
signals:
    void sendTJResult(QString mess);
private:
    void initMapList();
private:
    newModelListStruct newModeInfoList;
    QMap<int,QString>cmdMap;

private slots:
    void anlyzeStruct(newModelListStruct modeInfoList);
};

#endif // TTLVANYLSIS_H
