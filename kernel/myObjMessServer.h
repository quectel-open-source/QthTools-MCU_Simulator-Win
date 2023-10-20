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
#ifndef GLOBALOBSERVER_H
#define GLOBALOBSERVER_H

#include <QObject>
#include <QVariant>
#include "myObjMessApater.h"

struct relationData
{
    QString type;
    QObject *reciver;
    const char *method;
};

class myObjMessServer : public QObject
{
    Q_OBJECT
public:
    static myObjMessServer* getObjMessServer();
    static void release();
    static myObjMessServer *m_pInst;

    void attach(const QString type, QObject *reciver, const char *method);
    void detach(const QString type, const QObject* reciver);

    void notify(const QString type,
                QGenericArgument val0 = QGenericArgument(nullptr),
                QGenericArgument val1 = QGenericArgument(),
                QGenericArgument val2 = QGenericArgument(),
                QGenericArgument val3 = QGenericArgument(),
                QGenericArgument val4 = QGenericArgument(),
                QGenericArgument val5 = QGenericArgument(),
                QGenericArgument val6 = QGenericArgument(),
                QGenericArgument val7 = QGenericArgument(),
                QGenericArgument val8 = QGenericArgument(),
                QGenericArgument val9 = QGenericArgument());

    void notify(const QString type,
                QGenericReturnArgument ret,
                QGenericArgument val0= QGenericArgument(nullptr),
                QGenericArgument val1= QGenericArgument(),
                QGenericArgument val2= QGenericArgument(),
                QGenericArgument val3= QGenericArgument(),
                QGenericArgument val4= QGenericArgument(),
                QGenericArgument val5= QGenericArgument(),
                QGenericArgument val6= QGenericArgument(),
                QGenericArgument val7= QGenericArgument(),
                QGenericArgument val8= QGenericArgument(),
                QGenericArgument val9= QGenericArgument());
signals:

private:
    explicit myObjMessServer(QObject *parent = 0);
    ~myObjMessServer();

private:
    QList<relationData*> m_oRelationList;
};

#endif // GLOBALOBSERVER_H
