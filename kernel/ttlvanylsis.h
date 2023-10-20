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
enum
{
    DP_TTLV_TYPE_BOOL_FALSE = 0, /* 布尔值false */
    DP_TTLV_TYPE_BOOL_TRUE,      /* 布尔值true */
    DP_TTLV_TYPE_ENUM_NUM,       /* 枚举和数值 */
    DP_TTLV_TYPE_BINARY,         /* 二进制数据 */
    DP_TTLV_TYPE_STRUCT,         /* 结构体 */
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
    struct newModeInfo *head;
    QList<newModelStruct>modeInfo;
}newModelListStruct;



char intGetChar(uint64_t Number,int position,int NumSize);
QByteArray IntToByte(uint64_t Number, int NumSize);
int getDoubleAccuracy(double tempDouble);
int getArrayMax(int data[],int count);

class ttlvAnylsis
{
public:
    ttlvAnylsis();
    bool analyzeTtlvDataNode(QByteArray data, int *offset,newModelListStruct *list);
    void handleAnalyzeResult(newModelListStruct *list);
    void anlyzeTtlvData(QByteArray data);
private:
    newModelListStruct newModeInfoList;
};

#endif // TTLVANYLSIS_H
