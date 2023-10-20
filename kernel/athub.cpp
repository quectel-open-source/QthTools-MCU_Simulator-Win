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
#include "athub.h"
#include <QDebug>

QStringList g_atCmdList =
{
    "QIOTREG",
    "QIOTSTATE",
    "QIOTSEND",
    "QIOTRD",
    "QIOTMODELTD",
    "QIOTMODELRD",
    "QIOTCFG",
    "QIOTMCUVER",
    "QIOTUPDATE",
    "QIOTBINDCODE",
    "QIOTINFO",
    "QIOTOTARD",
    "QIOTLOCIN",
    "QIOTLOCEXT",
    "QIOTOTAREQ",
    "QHOTAREQ",
    "QIOTSUBCONN",
    "QIOTSUBDISCONN",
    "QIOTSUBSEND",
    "QIOTSUBRD",
    "QIOTSUBTSLRD",
    "QIOTSUBTSLTD",
    "QIOTSUBHTB"
};

/**************************************************************************
** 功能	@brief :实例化
** 输入	@param :
** 输出	@retval:
***************************************************************************/
ATHub::ATHub(QObject *parent) : QObject(parent)
{
    atProgressTimer = new QTimer();
    atProgressTimer->setInterval(2000);
    connect(atProgressTimer,SIGNAL(timeout()),this,SLOT(atProgressCheckTimeout()));
}
/**************************************************************************
** 功能	@brief : 发送指令保存
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATHub::atSendLogSave(QString atCmd)
{
    userATProgress newAtProgress;
    newAtProgress.atCmd = atCmd;
    newAtProgress.sendIsOk = true;
    newAtProgress.lifeTime = 10;      // 单位毫秒
    atProgressList.append(newAtProgress);
    if(!atProgressTimer->isActive())
    {
        atProgressTimer->start();
    }
}
/**************************************************************************
** 功能	@brief :接收
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATHub::recvAtSendAck()
{
    qDebug()<<__FUNCTION__;
    QMutexLocker locker(&dataHandleMutex);
    if(atProgressList.count() < 0)
    {
        return;
    }
    qDebug()<<"atProgressList.count():"<<atProgressList.count();
    QList<int>recordList;
    for(int i = atProgressList.count()-1;i >= 0;i--)
    {
        for(int j =0;j<g_atCmdList.count();j++)
        {
            if(atProgressList[i].atCmd.contains(QString(g_atCmdList[j])))
            {
                emit signalAtAck(atProgressList[i].atCmd);
                recordList.append(i);
                break;
            }
        }
    }
    for(int i = 0;i<recordList.count();i++)
    {
        atProgressList.removeAt(recordList[i]);
    }
    if(0 == atProgressList.count())
    {
        atProgressTimer->stop();
    }
}
/**************************************************************************
** 功能	@brief :发送字节流数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ATHub::atProgressCheckTimeout()
{
    QMutexLocker locker(&dataHandleMutex);
    for(int i = atProgressList.count()-1;i>=0;i--)
    {
        atProgressList[i].lifeTime -= 2;
        if(atProgressList[i].lifeTime <= 0)
        {
            atProgressList.removeAt(i);
        }
    }
}
