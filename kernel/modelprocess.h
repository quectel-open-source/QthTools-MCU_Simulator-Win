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
#ifndef MODELPROCESS_H
#define MODELPROCESS_H

#include <QTableWidget>
#include "mapList.h"
typedef struct enumSaveInfo
{
    QList<int> value;
    int id;
}enumInfo;

class modelProcess
{
public:
    modelProcess();
    void updateModelPointList(QJsonArray modelArray,QTableWidget * tableWidget);
    int analyzeModelData(QByteArray data,QTableWidget * tableWidget,int *offset,int *arrayCount);
    int recvModelData(QByteArray data,QTableWidget * tableWidget);
    int recvModelData(QByteArray data,QTableWidget * tableWidget,int mode);
    QByteArray extractServerId(QByteArray data,int mode);
    QByteArray sendModelData(QTableWidget * tableWidget,bool mode);
    QByteArray readModelData(QList<QByteArray> idList,QTableWidget * tableWidget,bool mode);
    void readModelId(QList<QByteArray> idList,QTableWidget * tableWidget);
    bool updateReadModelPointList(QJsonObject modelArray,QTableWidget * tableWidget);
    void updateReadModelPointList(QJsonArray modelObj,QTableWidget * tableWidget);
    bool explainBaseJsonData(QJsonValue modelValue,QWidget * box);
    void setServerFlag(bool flag);
    QList<int> getListSelectedRows(QTableWidget * tableWidget);
private:
    int modelCreate(int row, int column,QJsonObject modelObj,QTableWidget * tableWidget,bool writeEnable=true);
    QByteArray getDataFormBox(int id,QWidget * box);
    QJsonObject getJsonDataFormBox(int id,QWidget * box);
    QList<enumInfo> currentModelEnumValue;
    mapList *serverMapList;
    bool serverFlag;
};

#endif // MODELPROCESS_H
