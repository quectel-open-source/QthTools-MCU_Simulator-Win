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
#ifndef CONFIGPROCESS_H
#define CONFIGPROCESS_H

#include <QObject>
#include <QTableWidget>
#include <QPushButton>
#include <QStandardItemModel>
#include "kernel/jsonFile.h"

typedef struct
{
    QString pk;
    QString ps;
    QString model;
    QString server;
	QStringList serverList;
    QString protocol;
    QString buffer;
    QString lifetime;
    QString act;
    QString filePath;
    QString format;
    QString sessionMode;
    QString note;
    QString dk;
    QString ds;
}configInfo_t;

enum
{
    CONFIG_TABLE_PK,
    CONFIG_TABLE_PS,
    CONFIG_TABLE_JSON,
    CONFIG_TABLE_NOTE,
};

class configProcess : public QObject
{
    Q_OBJECT
public:
    explicit configProcess();
    ~configProcess();
    void configInfoSave(QTableWidget *configTable,configInfo_t *currentConfigInfo);
    void configInfoRead(QTableWidget *configTable,configInfo_t *currentConfigInfo);
    void historySave(QString key,QStandardItemModel &value);
    void historySave(QString key,QStringList value);
    void historyRead(QString key,QStandardItemModel* value);
    QStringList historyRead(QString key);
    void uartConfigSave(QString key,QString value);
    QString uartConfigRead(QString key);
    Q_INVOKABLE void test(QString key);
private slots:

private:
    QTableWidget *configTable;
    jsonFile *configFile;
    QButtonGroup* checkGroup;
    void configFileRead();
};

#endif // CONFIGPROCESS_H
