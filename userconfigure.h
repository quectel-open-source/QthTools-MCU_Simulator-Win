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
#ifndef USERCONFIGURE_H
#define USERCONFIGURE_H
#include <QMap>
#include <QMutex>
#include <windows.h>
#include <QRect>
#include <QTranslator>
#include "kernel/athub.h"
#define LL_DBG  0
#define LL_INFO 1
#define LL_WARN 2
#define LL_ERR  3
#define LL_DUMP 4


#define ADDCONFIGCLASS  LL_INFO
#define CONFIGCLASS     LL_INFO
#define DEBUGCLASS      LL_INFO
#define INFOCLASS       LL_INFO
#define MAINCLASS       LL_INFO
#define MODELCLASS      LL_INFO
#define OTACLASS        LL_INFO
#define PASSCLASS       LL_INFO
#define WIFICLASS       LL_INFO


enum
{
    GETUSERINFO=0,
    SETUSERINFO,
};

enum
{
    SOCKETSTATUS=0, // socket的状态
    MQTTSTATUS,     // mqtt 的状态
    CACHEMODE,      // 是否打开缓存
    SENDMMODELMODE, // ttlv | json
};

enum
{
    TEXTEDITAPPENDMODE,
    TEXTEDITINSERTMODE,
};

#define TTLVMODESNED false
#define JSONMODESEND true

extern QByteArray *recvBuffer;
extern QMutex socketMutex;
extern int debugLevel;
//extern HKL hCurKL;
#include <QReadWriteLock>
extern QReadWriteLock readWriteLock;
extern QString vitalFilePath;
extern QRect appRect;
extern float sqliteCurVersion;
extern ATHub atHubInstance;

class userConfigure
{
public:
    userConfigure();
    bool getValue(int key,bool &error);
    bool setValue(int key,bool value,bool &error);
private:
    QMap<int,bool>userInfo;
};

extern QTranslator *m_translator;
extern QString softwareVersion;

#endif // USERCONFIGURE_H
