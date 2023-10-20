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
#include "jsonFile.h"
#include "QFileInfo"
#include "QFile"
#include "QDir"

jsonFile::jsonFile(QString fileUrl)
{
    qInfo()<<"jsonfileUrl:"<<fileUrl;
    if(!fileUrl.isEmpty())
    {
        this->fileUrl = fileUrl;
        QFileInfo fileInfo(fileUrl);
        QString path = fileInfo.path();
        QDir dir;
        if(path.isEmpty() || dir.exists(path)==false) dir.mkdir(path);
    }
}

jsonFile::~jsonFile()
{

}

/**************************************************************************
** 功能	@brief : 读取配置文件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool jsonFile::readFile(void)
{
    qInfo()<<__FUNCTION__;
    QJsonObject obj;
    QFile file(this->fileUrl);
    if(true == file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray readData = file.readAll();
        file.close();
        return isJsonObject(readData);
    }
    qCritical()<<"read <"<<file.fileName()<<"> fail";
    return false;
}


bool jsonFile::isJsonObject(QByteArray readData)
{
    if(readData.isEmpty())
        return false;
    QJsonParseError jsonErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(readData,&jsonErr);
    if(jsonErr.error == QJsonParseError::NoError)
    {
        if(jsonDoc.isObject())
        {
            this->jsonObj = jsonDoc.object();
            return true;
        }
    }
    return false;
}

/**************************************************************************
** 功能	@brief : 写配置文件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool jsonFile::writeFile(void)
{
    qInfo()<<__FUNCTION__;
    QFile file(fileUrl);
    if(true == file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
    {
        QJsonDocument rootDoc;
        rootDoc.setObject(this->jsonObj);
        QByteArray writeData = rootDoc.toJson();
        file.write(writeData);
        file.close();
        return true;
    }
    qCritical()<<"write <"<<file.fileName()<<"> fail";
    return false;
}


/**************************************************************************
** 功能	@brief : 读json值
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool jsonFile::readJsonKeyValue(QString keyUrl,QJsonValue *value)
{
//    qInfo()<<__FUNCTION__;
    QStringList keyList = keyUrl.split("/",QString::SplitBehavior::SkipEmptyParts);
    QJsonObject obj = this->jsonObj;
    for(int i=0;i<keyList.size();i++)
    {
        if(i+1 == keyList.size())
        {
            if(obj.contains(keyList.at(i)))
            {
                *value = obj.value(keyList.at(i));
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(obj.contains(keyList.at(i)) && obj.value(keyList.at(i)).isObject())
        {
            obj = obj.value(keyList.at(i)).toObject();
        }
        else
        {
            break;
        }
    }
    qDebug()<<"find no value";
    return false;
}
/**************************************************************************
** 功能	@brief : 写json值
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool jsonFile::writeJsonKeyValue(QString keyUrl,QJsonValue value)
{
//    qInfo()<<__FUNCTION__;
    QStringList keyList = keyUrl.split("/",QString::SplitBehavior::SkipEmptyParts);
    QList<QJsonObject> objList;
    QJsonObject obj = this->jsonObj;
    for(int32_t i=0;i<keyList.size()-1;i++)
    {
        if(obj.contains(keyList.at(i)))
        {
            obj = obj.value(keyList.at(i)).toObject();
        }
        else
        {
            QJsonObject tempObj;
            obj =tempObj;
        }
        objList.append(obj);
    }

    for(int i = objList.size();i>0;i--)
    {
        objList.last().insert(keyList.at(i),value);
        value = objList.last();
        objList.removeLast();
    }
    this->jsonObj.insert(keyList.first(),value);
    return true;
}

