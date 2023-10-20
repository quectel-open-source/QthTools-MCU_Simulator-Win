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
#ifndef ATHUB_H
#define ATHUB_H

#include <QObject>
#include <QTimer>
#include <QMutex>

extern QStringList g_atCmdList;

class userATProgress
{
 public:
    userATProgress()
    {
        sendIsOk = false;
        recvResult = false;
        lifeTime = 0;
    }
    QString atCmd;
    bool sendIsOk;
    bool recvResult;
    int lifeTime;
};

class ATHub : public QObject
{
    Q_OBJECT
public:
    explicit ATHub(QObject *parent = nullptr);
    void atSendLogSave(QString atCmd);
    void recvAtSendAck();
signals:
    void signalAtAck(QString atCmd);
public slots:
    void atProgressCheckTimeout();
private:
    QList<userATProgress> atProgressList;
    QTimer *atProgressTimer = NULL;
    QMutex dataHandleMutex;
};

#endif // ATHUB_H
