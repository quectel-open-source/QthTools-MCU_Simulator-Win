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
#ifndef JSONLIST_H
#define JSONLIST_H

#include "QJsonDocument"
#include "QJsonParseError"
#include "QJsonObject"
#include "QJsonValue"
#include <QMap>
#include <QVariant>

typedef struct
{
//    QList<QMap<QString,QVariant>>input;
//    QList<QMap<QString,QVariant>>output;
    QMap<QString,QVariant>input;
    QMap<QString,QVariant>output;
    QString name;
}info_type;

class mapList
{
public:
    mapList();

//    // 建立表
//    bool buildJsonTable();

//    // 增加
//    bool addJsonToTable(QJsonObject data,QString key,bool level);

//    // 删除
//    bool deleteJsonFromTable(QJsonObject data,int level);


//    // 查找表 ，可循环嵌套
////    bool findValueJsonFromTable(QJsonObject objHead ,QString key, QJsonValue *data,int level);
//    bool findValueJsonFromTable(QString key, QJsonValue *data,int level);

//    // 映射
//    bool findJsonMapFromTable(QString key,QString *data);


//    // 修改
//    bool modifyJsonFromTable(QJsonObject data);

//    // 保存记录
//    bool saveJsonData();

/*******************************************************************************************************************/

    // 增加
    bool addMapToTable(info_type data);

    // 删除
    bool deleteMapFromTable(info_type data);


    // 查找表 ，可循环嵌套
//    bool findValueJsonFromTable(QJsonObject objHead ,QString key, QJsonValue *data,int level);
    bool findValueMapFromTable(info_type *data);

    // 修改
    bool modifyMapFromTable(info_type data);

    //

private:
//    QJsonDocument *g_docu;
//    QJsonObject *g_obj;
    QList<info_type>infoList;
};

#endif // JSONLIST_H
