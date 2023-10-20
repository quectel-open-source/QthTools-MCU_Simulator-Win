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
#ifndef USERCONFIGSAVE_H
#define USERCONFIGSAVE_H
#include "jsonFile.h"

class userConfigSave
{
public:
    userConfigSave(QString pathName);
    void addJsonToFile(QString keyUrl, QJsonValue *value);
    QJsonValue readJsonFile(QString keyUrl);
private:
    jsonFile *userConfig;
//    static userConfigSave *m_pInstance;

};

class userConfigSaveInstance
{
public:
    static userConfigSaveInstance *globalInstance();
    userConfigSaveInstance();
    void init(QString pathName);
    void addJsonToFile(QString keyUrl, QJsonValue *value);
    QJsonValue readJsonFile(QString keyUrl);
private:
    userConfigSave *userConfigSaveDemo;
};


#endif // USERCONFIGSAVE_H
