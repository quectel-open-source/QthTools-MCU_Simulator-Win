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
#include "configprocess.h"

#include <QJsonArray>
#include "baseQTConfigure.h"
#include "kernel_config.h"
#include "userconfigure.h"

configProcess::configProcess()
{
    /* 配置文件 */
    QString configfile = vitalFilePath+".config.ini";
    if(configfile.isEmpty())
        qCritical()<<"config.ini is Empty";
    this->configFile = new jsonFile(configfile);
    this->configFile->readFile();

    myObjMessServer::getObjMessServer()->attach("configprocess_historyRead", this, "historyRead");
}

configProcess::~configProcess()
{

}
/**************************************************************************
** 功能	@brief : 用户配置信息保存
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configProcess::configInfoSave(QTableWidget *configTable,configInfo_t *currentConfigInfo)
{
    qInfo()<<__FUNCTION__;
    QJsonArray configArray;
    for(int i=0;i<configTable->rowCount();i++)
    {
        QJsonObject configObj;
        configObj.insert("pk",configTable->item(i,CONFIG_TABLE_PK)->text());
        configObj.insert("ps",configTable->item(i,CONFIG_TABLE_PS)->text());
        configObj.insert("filePath",configTable->item(i,CONFIG_TABLE_JSON)->toolTip());
        if (configTable->item(i,CONFIG_TABLE_JSON)->text() == "Transparent")
        {
            configObj.insert("format","透传数据");
        }
        else if (configTable->item(i,CONFIG_TABLE_JSON)->text() == "TSL")
        {
            configObj.insert("format","物模型");
        }
        else
        {
            configObj.insert("format",configTable->item(i,CONFIG_TABLE_JSON)->text());
        }
        configObj.insert("note",configTable->item(i,CONFIG_TABLE_NOTE)->text());
        configArray.append(configObj);
    }
    this->configFile->writeJsonKeyValue("config",configArray);
    this->configFile->writeJsonKeyValue("server",currentConfigInfo->server);
    this->configFile->writeJsonKeyValue("buffe",currentConfigInfo->buffer);
    this->configFile->writeJsonKeyValue("protocol",currentConfigInfo->protocol);
    this->configFile->writeJsonKeyValue("lifetime",currentConfigInfo->lifetime);
    this->configFile->writeJsonKeyValue("contextId",currentConfigInfo->act);
    this->configFile->writeJsonKeyValue("tsl",currentConfigInfo->format);
    this->configFile->writeJsonKeyValue("sessionMode",currentConfigInfo->sessionMode);
    this->configFile->writeJsonKeyValue("index",configTable->currentRow());
    QJsonArray serverList;
    for(int i=0;i<currentConfigInfo->serverList.count();i++)
    {
        serverList.append(currentConfigInfo->serverList[i]);
    }
    this->configFile->writeJsonKeyValue("serverList",serverList);
    this->configFile->writeFile();
}

/**************************************************************************
** 功能	@brief : 用户配置信息读取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configProcess::configInfoRead(QTableWidget *configTable,configInfo_t *currentConfigInfo)
{
    qInfo()<<__FUNCTION__;
    QJsonValue jsonvalue;
    this->configFile->readJsonKeyValue("index",&jsonvalue);
    int index = jsonvalue.toInt();
    this->configFile->readJsonKeyValue("server",&jsonvalue);
    currentConfigInfo->server = jsonvalue.toString();
    this->configFile->readJsonKeyValue("buffe",&jsonvalue);
    currentConfigInfo->buffer = jsonvalue.toString();
    this->configFile->readJsonKeyValue("protocol",&jsonvalue);
    currentConfigInfo->protocol = jsonvalue.toString();
    this->configFile->readJsonKeyValue("lifetime",&jsonvalue);
    currentConfigInfo->lifetime = jsonvalue.toString();
    this->configFile->readJsonKeyValue("contextId",&jsonvalue);
    currentConfigInfo->act = jsonvalue.toString();
    this->configFile->readJsonKeyValue("tsl",&jsonvalue);
    currentConfigInfo->format = jsonvalue.toString();
    this->configFile->readJsonKeyValue("sessionMode",&jsonvalue);
    currentConfigInfo->sessionMode = jsonvalue.toString();
    this->configFile->readJsonKeyValue("config",&jsonvalue);
    QJsonArray configArray = jsonvalue.toArray();
    configTable->setRowCount(configArray.count());
    for (int i = 0; i < configArray.count(); i++)
    {
        QJsonObject configObj = configArray.at(i).toObject();
        configTable->setItem(i,CONFIG_TABLE_PK,new QTableWidgetItem(configObj.value("pk").toString()));
        configTable->setItem(i,CONFIG_TABLE_PS,new QTableWidgetItem(configObj.value("ps").toString()));
//        if (configObj.value("format").toString() == "透传数据")
//        {
//            configTable->setItem(i,CONFIG_TABLE_JSON,new QTableWidgetItem(tr("透传数据")));
//        }
//        else if (configObj.value("format").toString() == "物模型")
//        {
//            configTable->setItem(i,CONFIG_TABLE_JSON,new QTableWidgetItem(tr("物模型")));
//        }
        configTable->setItem(i,CONFIG_TABLE_JSON,new QTableWidgetItem(configObj.value("format").toString()));

        configTable->setItem(i,CONFIG_TABLE_NOTE,new QTableWidgetItem(configObj.value("note").toString()));
        configTable->item(i,CONFIG_TABLE_JSON)->setToolTip(configObj.value("filePath").toString());
        if(i == index)
        {
            currentConfigInfo->pk = configObj.value("pk").toString();
            currentConfigInfo->ps = configObj.value("ps").toString();
            currentConfigInfo->filePath = configObj.value("filePath").toString();
            currentConfigInfo->note = configObj.value("note").toString();
            currentConfigInfo->model = configObj.value("format").toString(); //数据类型
        }
    }
    this->configFile->readJsonKeyValue("serverList",&jsonvalue);
    QJsonArray serverList = jsonvalue.toArray();
    for (int i = 0; i < currentConfigInfo->serverList.count(); i++)
    {
        currentConfigInfo->serverList.append(currentConfigInfo->serverList[i]);
    }
    if(index == -1)
    {
        index = 0;
    }
    configTable->setCurrentCell(index,0);
}

/**************************************************************************
** 功能	@brief : 信息保存
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configProcess::historySave(QString key,QStandardItemModel &value)
{
    QJsonArray keyArray;
    for(int i=0;i<value.rowCount();i++)
    {
        keyArray.append(value.index(i,0).data().toString());
    }
    this->configFile->writeJsonKeyValue(key,keyArray);
    this->configFile->writeFile();
}

void configProcess::historySave(QString key,QStringList value)
{
    QJsonArray keyArray;
    for(int i=0;i<value.size();i++)
    {
        keyArray.append(value.at(i));
    }
    this->configFile->writeJsonKeyValue(key,keyArray);
    this->configFile->writeFile();
}
/**************************************************************************
** 功能	@brief : 信息读取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configProcess::historyRead(QString key,QStandardItemModel *value)
{
    qInfo()<<__FUNCTION__;
    QJsonValue jsonvalue;
    this->configFile->readJsonKeyValue(key,&jsonvalue);
    QJsonArray keyArray = jsonvalue.toArray();
    value->clear();
    for (int i = 0; i < keyArray.size(); i++)
    {
        value->appendRow(new QStandardItem(keyArray.at(i).toString()));
    }
}

QStringList configProcess::historyRead(QString key)
{
    QStringList value;
    QJsonValue jsonvalue;
    if(this->configFile->readJsonKeyValue(key,&jsonvalue))
    {
        QJsonArray keyArray = jsonvalue.toArray();
        for (int i = 0; i < keyArray.size(); i++)
        {
            value.append(keyArray.at(i).toString());
        }
    }
    return value;
}

void configProcess::test(QString data)
{
    qInfo()<<__FUNCTION__;
    qDebug()<<"data:"<<data;
}

/**************************************************************************
** 功能	@brief : socket配置信息保存
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void configProcess::uartConfigSave(QString key,QString value)
{
    qInfo()<<__FUNCTION__;
    this->configFile->writeJsonKeyValue(key,value);
    this->configFile->writeFile();
}
/**************************************************************************
** 功能	@brief : socket配置信息读取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QString configProcess::uartConfigRead(QString key)
{
//    qInfo()<<__FUNCTION__;
    QJsonValue jsonvalue;
    this->configFile->readJsonKeyValue(key,&jsonvalue);
    return jsonvalue.toString();
}
