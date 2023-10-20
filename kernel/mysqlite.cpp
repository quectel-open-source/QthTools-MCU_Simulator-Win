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
#include "mysqlite.h"
#include <QStandardPaths>
#include <QDebug>
#include <QSqlRecord>
#include<QDir>
#include "userconfigure.h"
mySqlite::mySqlite()
{

}

mySqlite::~mySqlite()
{

}

void mySqlite::createSqlDataBase(QString name,QString path)
{
    // 指定的连接名称
    QString dstPath = path;
    if (QSqlDatabase::contains(name))
    {
        database = QSqlDatabase::database(name);
    }
    else
    {
        qDebug()<<"dstPath:"<<dstPath;
        // 添加数据库
        database = QSqlDatabase::addDatabase("QSQLITE",name);
        // 数据库文件名:如果这个数据库不存在，则会在后续操作时自动创建；如果已经存在，则后续的操作会在已有的数据库上进行
        if(!dstPath.isEmpty())
        {
            database.setDatabaseName(vitalFilePath+dstPath+".db");
        }
    }
}

bool mySqlite::setSqlPath(QString path)
{
    bool result = false;
    if(database.isOpen())
    {
        qDebug()<<"set path success";
        database.setDatabaseName(path);
        result = true;
    }
    return result;
}
/**************************************************************************
** 功能	@brief : 更新sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note: 存在表则切换，不存在则创建
**      @example: create table student (id int primary key, name text(30), age int)
***************************************************************************/
void mySqlite::createSqlQuery(QString tableName, QString data)
{
    // 打开数据库
    if(!database.open())
    {
        qDebug()<<"open error";
        return;
    }
    QStringList tableNameList = database.tables();
    bool isFind = false;
    qDebug()<<tableNameList;
    foreach (QString name, tableNameList)
    {
        qDebug()<<"name:"<<name;
        // sqlite_sequence 排除
        if ( QString("sqlite_sequence") == name)
        {
            continue;
        }
        if(0 == tableName.compare(name))
        {
            isFind = true;
            break;
        }
    }
    if(false == isFind)
    {
//        QSqlQuery sql_query = QSqlQuery(database);
//        sql_query.exec(data);
//        query = sql_query;
        query = QSqlQuery(database);
        if(false == query.prepare(data))
        {
            qDebug()<<"error table"<<query.lastError();
        }
        if(!query.exec(data))
        {
            qDebug()<<"query->lastError():"<<query.lastError();
        }
    }
    else
    {
//        QSqlQuery sql_query = QSqlQuery(database);
//        query = sql_query;
        query = QSqlQuery(database);
//        if(!query.exec(data))
//        {
//            qDebug()<<"query->lastError():"<<query.lastError();
//        }
    }
}
/**************************************************************************
** 功能	@brief : 插入sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example: insert into student value(3, \"Li\", 23)
***************************************************************************/
bool mySqlite::insertSqlData(QString data)
{
    // 打开数据库
    if(!database.isOpen())
    {
        qDebug()<<"open error";
        return false;
    }
    if(!query.exec(data))
    {
        qDebug() << query.lastError();
        return false;
    }
    return true;
}
/**************************************************************************
** 功能	@brief : 查找sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example: select id, name from student"; select * from student select max(id) from student
***************************************************************************/
QStringList mySqlite::findSqlDataList(QString data)
{
    QStringList result;
    // 打开数据库
    if(!database.isOpen())
    {
        qDebug()<<"open error";
        return result;
    }
    if(!query.exec(data))
    {
        qDebug() << query.lastError();
        return result;
    }
    while(query.next())
    {
        result.append(query.value(0).toString());
    }
    return result;
}

QList<QStringList> mySqlite::findSqlDataListEx(QString data)
{
    QList<QStringList> result;
    // 打开数据库
    if(!database.isOpen())
    {
        qDebug()<<"open error";
        return result;
    }
    if(!query.exec(data))
    {
        qDebug() << query.lastError();
        return result;
    }
    while(query.next())
    {
        QStringList list;
        for(int i = 0;i<query.record().count();i++)
            list.append(query.value(i).toString());
        result.append(list);
    }
    return result;
}

QString mySqlite::findSqlDataString(QString data)
{
    QString result;
        // 打开数据库
    if(!database.isOpen())
    {
        qDebug()<<"open error";
        return result;
    }
    if(!query.exec(data))
    {
        qDebug() << query.lastError();
        return result;
    }
    while(query.next())
    {
        result.append(query.value(0).toString());
    }
    return result;
}

QVariant mySqlite::findSqlData(QString data, int mode)
{
    QString result = "unKnow";
    // 打开数据库
    if(!database.isOpen())
    {
        qDebug()<<"open error";
        return QVariant(result);
    }
    if(mode == 2)
    {
        QStringList result;
        if(!query.exec(data))
        {
            qDebug() << query.lastError();
            goto exit;
        }
        if(query.next())
        {
            QSqlRecord rec = query.record();
            for(int i = 0;i<rec.count();i++)
                result.append(query.value(i).toString());
        }
        goto exit;
    }
    else if(mode == 1)
    {
        int result = 0;
        if(!query.exec(data))
        {
            qDebug() << query.lastError();
            return (QVariant)result;
        }
        return (QVariant)(query.size());
    }
exit:
    return QVariant(result);
}

QVector<subDevConfigInfo_t> mySqlite::findSqlAllList(QString data)
{
    QVector<subDevConfigInfo_t> it;
    // 打开数据库
    if(!database.isOpen())
    {
        qDebug()<<"open error";
        goto exit;
    }
    if(!query.exec(data))
    {
        qDebug() << query.lastError();
        return it;
    }
    while(query.next())
    {
        subDevConfigInfo_t info;
        info.dk = query.value(0).toString();
        info.pk = query.value(1).toString();
        info.ps = query.value(2).toString();
        info.buffer = query.value(3).toString();
        info.lifetime = query.value(4).toString();
        info.filePath = query.value(5).toString();
        info.note = query.value(6).toString();
        info.ds = query.value(7).toString();
        info.tls = query.value(8).toString();
        it.append(info);
    }
exit:
    return it;
}

/**************************************************************************
** 功能	@brief : 删除sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example: delete from student where id = ?"; delete from student  delete from student where id=1 and name='xiaopi'
***************************************************************************/
bool mySqlite::delSqlData(QString data)
{
    // 打开数据库
    if(!database.isOpen())
    {
        qDebug()<<"open error";
        return false;
    }
    if(!query.exec(data))
    {
        qDebug() << query.lastError();
        return false;
    }
    return true;
}
/**************************************************************************
** 功能	@brief : 更新sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example: update student set name='小皮' where id=1
***************************************************************************/
bool mySqlite::modifySqlData(QString data)
{
    // 打开数据库
    if(!database.isOpen())
    {
        qDebug()<<"open error";
        return false;
    }
    if(!query.exec(data))
    {
        qDebug() << query.lastError();
        return false;
    }
    return true;
}
/**************************************************************************
** 功能	@brief : 关闭sql数据库
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example:
***************************************************************************/
void mySqlite::closeSqlDataBase()
{
    // 打开数据库
    if(!database.isOpen())
    {
        qDebug()<<"open error";
        return;
    }
    database.close();
}

QStringList mySqlite::execSqlDataList(QString data)
{

    QStringList result;
    if(database.isOpen())
    {
        if(query.exec(data))
        {
            while(query.next())
            {
                result.append(query.value(0).toString());
            }
        }
        else
        {
            qDebug() << query.lastError();
        }
    }
    return result;
}

QString mySqlite::getDataBaseName()
{
    QString dataBaseName;
    if(database.isOpen())
    {
        dataBaseName = database.databaseName();
    }
    return dataBaseName;
}

bool mySqlite::isMathQuery(QString queryName)
{
    bool result = false;
    if(database.isOpen())
    {
        QStringList tableNameList = database.tables();
        qDebug()<<"tableNameList:"<<tableNameList;
        foreach (QString name, tableNameList)
        {
            if (0 == queryName.compare(name))
            {
                result = true;
                break;
            }
        }
    }
    qDebug()<<"result:"<<result;
    return result;
}

