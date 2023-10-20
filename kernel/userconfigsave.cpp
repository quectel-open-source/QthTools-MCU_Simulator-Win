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
#include "userconfigsave.h"
#include <QFile>
#include <QStandardPaths>
#include "userconfigure.h"
userConfigSave::userConfigSave(QString pathName)
{
    QString configfile = vitalFilePath+pathName;
    this->userConfig = new jsonFile(configfile);
    this->userConfig->readFile();
}


void userConfigSave::addJsonToFile(QString keyUrl, QJsonValue *value)
{
   this->userConfig->writeJsonKeyValue(keyUrl,*value);
   this->userConfig->writeFile();
}

QJsonValue userConfigSave::readJsonFile(QString keyUrl)
{
    QJsonValue jsonvalue;
    this->userConfig->readJsonKeyValue(keyUrl,&jsonvalue);
    return jsonvalue;
}


Q_GLOBAL_STATIC(userConfigSaveInstance, getInstance)
userConfigSaveInstance *userConfigSaveInstance::globalInstance()
{
    return getInstance();
}

userConfigSaveInstance::userConfigSaveInstance()
{

}

void userConfigSaveInstance::init(QString pathName)
{
    userConfigSaveDemo = new userConfigSave(pathName);
}

void userConfigSaveInstance::addJsonToFile(QString keyUrl, QJsonValue *value)
{
    userConfigSaveDemo->addJsonToFile(keyUrl,value);
}

QJsonValue userConfigSaveInstance::readJsonFile(QString keyUrl)
{
    return userConfigSaveDemo->readJsonFile(keyUrl);
}
