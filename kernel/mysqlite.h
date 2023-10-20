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
#ifndef MYSQLITE_H
#define MYSQLITE_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "subwindow/subdevcreate.h"

class mySqlite: public QObject
{
    Q_OBJECT
public:
    mySqlite();
    ~mySqlite();
    void createSqlDataBase(QString name,QString path);
    bool setSqlPath(QString path);
    void createSqlQuery(QString tableName,QString data);
    bool insertSqlData(QString data);
    QStringList findSqlDataList(QString data);
    QList<QStringList> findSqlDataListEx(QString data);
    QString findSqlDataString(QString data);
    QVariant findSqlData(QString data,int mode);
    QVector<subDevConfigInfo_t> findSqlAllList(QString data);
    bool delSqlData(QString data);
    bool modifySqlData(QString data);
    void closeSqlDataBase();
    QStringList execSqlDataList(QString data);
    QString getDataBaseName();
    bool isMathQuery(QString queryName);
private:
    QSqlDatabase database;
    QSqlQuery query;
};

#endif // MYSQLITE_H
