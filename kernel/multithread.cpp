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
#include "multithread.h"
#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include "userconfigure.h"
QReadWriteLock readWriteLock;

#define FILE_MAX_LENGTH 10485760


multiThread::multiThread(QObject *parent): QThread(parent)
{
    runFlag = false;
    checkDebugFileTimer = new QTimer();
    connect(checkDebugFileTimer,SIGNAL(timeout()),this,SLOT(checkDebugFileTimeOut()));
    checkDebugFileTimer->start(5*60*1000);
}

multiThread::~multiThread()
{

}

void multiThread::recvMess(QString data)
{
    sendData.append(data);
}

void multiThread::explainData(QList<QString> data)
{
    Q_UNUSED(data);
}

void multiThread::checkLogFileSize()
{
    QString configfile = vitalFilePath+".debug.ini";
    QFile file(configfile);
    if(file.size() > FILE_MAX_LENGTH)
    {
        file.seek(file.size()-FILE_MAX_LENGTH/5);
        char *data = (char *)malloc(FILE_MAX_LENGTH/5);
        file.read(data,FILE_MAX_LENGTH/5);
        file.resize(0);
        file.write(data,FILE_MAX_LENGTH/5);
        free(data);
    }
    file.close();
}




void multiThread::run()
{
    while(1)
    {
        QThread::msleep(1000);
    }
}

void multiThread::checkDebugFileTimeOut()
{
    qDebug()<<"check file size";
    checkLogFileSize();
}



