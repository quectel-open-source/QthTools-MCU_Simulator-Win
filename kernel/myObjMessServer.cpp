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
#include "myObjMessServer.h"

myObjMessServer *myObjMessServer::m_pInst = NULL;
myObjMessServer *myObjMessServer::getObjMessServer()
{
    if (m_pInst == NULL)
    {
        m_pInst = new myObjMessServer();
    }
    return m_pInst;
}

void myObjMessServer::release()
{
    if (m_pInst != NULL)
    {
        delete m_pInst;
        m_pInst = NULL;
    }
}

void myObjMessServer::attach(const QString type, QObject *reciver, const char *method)
{
    relationData *data = new relationData();
    data->type = type;
    data->reciver = reciver;
    data->method = method;
    m_oRelationList.append(data);
}

void myObjMessServer::detach(const QString type, const QObject *reciver)
{
    QList<relationData*>::iterator iter = m_oRelationList.begin();

    while (iter != m_oRelationList.end())
    {
        if ((*iter)->type.compare(type) == 0 && (*iter)->reciver == reciver)
        {
            relationData *data = *iter;
            m_oRelationList.removeOne((*iter));
            delete data;
            return;
        }
        iter++;
    }
}

void myObjMessServer::notify(const QString type, QGenericArgument val0, QGenericArgument val1, QGenericArgument val2,
                            QGenericArgument val3, QGenericArgument val4, QGenericArgument val5, QGenericArgument val6,
                            QGenericArgument val7, QGenericArgument val8, QGenericArgument val9)
{
    QList<relationData*>::iterator iter = m_oRelationList.begin();
    while (iter != m_oRelationList.end())
    {
        if ((*iter)->type.compare(type) == 0)
        {
            QMetaObject::invokeMethod((*iter)->reciver, (*iter)->method, Qt::AutoConnection, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);
        }
        iter++;
    }
}

void myObjMessServer::notify(const QString type, QGenericReturnArgument ret,QGenericArgument val0, QGenericArgument val1, QGenericArgument val2,
                            QGenericArgument val3, QGenericArgument val4, QGenericArgument val5, QGenericArgument val6,
                            QGenericArgument val7, QGenericArgument val8, QGenericArgument val9)
{
    QList<relationData*>::iterator iter = m_oRelationList.begin();
    while (iter != m_oRelationList.end())
    {
        if ((*iter)->type.compare(type) == 0)
        {
            QMetaObject::invokeMethod((*iter)->reciver, (*iter)->method, Qt::AutoConnection,ret, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);
        }
        iter++;
    }
}


myObjMessServer::myObjMessServer(QObject *parent) : QObject(parent)
{

}

myObjMessServer::~myObjMessServer()
{
    //释放列表数据
    QList<relationData*>::iterator iter = m_oRelationList.begin();

    while (iter != m_oRelationList.end())
    {
        delete *iter;
        iter++;
    }

}
