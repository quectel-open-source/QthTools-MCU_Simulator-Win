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
#ifndef OBESRVERAPATER_H
#define OBESRVERAPATER_H

#include <QObject>
class myObjMessApater;

//工厂，方便构造对象
class objserverApaterFactory
{
public:
    static objserverApaterFactory *getInst();
    static void realese();
    static objserverApaterFactory* inst;

    myObjMessApater* createObesrverApater();

private:
    objserverApaterFactory()
    {}
};

//中间层，用来连接信号槽
class myObjMessApater : public QObject
{
    Q_OBJECT
public:
    explicit myObjMessApater(QObject *parent = 0);

signals:
    void notify();
};

#endif // OBESRVERAPATER_H
