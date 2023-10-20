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
#include "ttlvDataAnysis.h"
#include "kernel/mmessagebox.h"
#include <QWidget>

ttlvDataAnysis::ttlvDataAnysis()
{
    initMapList();
}


/**************************************************************************
** 功能	@brief : int 转 char
** 输入	@param :
** 输出	@retval:
***************************************************************************/
char ttlvDataAnysis::intGetChar(uint64_t Number,int position,int NumSize)
{
    uint64_t highByte = ((uint64_t)0xff << (NumSize-position)*8)&Number;
    return (uchar)(highByte>>((NumSize-position)*8));
}
/**************************************************************************
** 功能	@brief : int 转 byte
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QByteArray ttlvDataAnysis::IntToByte(uint64_t Number, int NumSize)
{
    QByteArray abyte0;
    for (int i=1;i<=NumSize;i++)
    {
        abyte0.append(intGetChar(Number,i,NumSize));
    }
    return abyte0;
}

/**************************************************************************
** 功能	@brief : 获取double精度
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int ttlvDataAnysis::getDoubleAccuracy(double tempDouble)
{
    QString doubleString;
    if(QString("%1").arg(tempDouble).indexOf("e") >= 0 || QString("%1").arg(tempDouble).indexOf("E") >= 0)
    {
        doubleString = QString::number(tempDouble,'f',15);
    }
    else
    {
        doubleString = QString("%1").arg(tempDouble);
    }
    QStringList doubleStringList = doubleString.split(".");
    if(doubleStringList.count() == 2)
    {
        for (int i = doubleStringList.at(1).length()-1; i >= 0 ; i--)
        {
            if(doubleStringList.at(1)[i] != "0")
            {
                return i+1;
            }
        }
    }
    return 0;
}
/**************************************************************************
** 功能	@brief : 获取数组长度
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int ttlvDataAnysis::getArrayMax(int data[],int count)
{
    int maxInt = 0;
    for(int i=0;i<count;i++)
    {
        if(maxInt < data[i])
        {
            maxInt = data[i];
        }
    }
    return maxInt;
}

uint8_t ttlvDataAnysis::dataSum(QByteArray data)
{
    uint8_t dataSum = 0;
    for(int i = 0;i<data.size();i++)
    {
        dataSum += data.at(i);
    }
    return dataSum;
}


void ttlvDataAnysis::initMapList()
{
    cmdMap.insert(CMD_TTLV_STATUS_GET,tr("云端获取设备的物模型状态"));
    cmdMap.insert(CMD_TTLV_STATUS_ACK,tr("设备回复物模型状态到云端"));
    cmdMap.insert(CMD_TTLV_DATA_GET,tr("云端下发物模型数据到设备"));
    cmdMap.insert(CMD_TTLV_DATA_REPORT,tr("设备回复物模型数据到云端"));
    cmdMap.insert(CMD_DATA_GET,tr("云端透传数据下发"));
    cmdMap.insert(CMD_DATA_REPORT,tr("设备透传数据上报"));


    cmdMap.insert(CMD_DEV_STATUS_GET,tr("云端获取设备的状态"));
    cmdMap.insert(CMD_DEV_STATUS_ACK,tr("获取设备状态回复至云端"));
    cmdMap.insert(CMD_DEV_STATUS_REPORT,tr("设备状态上报至云端"));
    cmdMap.insert(CMD_DEV_INFO_GET,tr("云端获取设备信息"));
    cmdMap.insert(CMD_DEV_INFO_ACK,tr("设备信息回复至平台"));
    cmdMap.insert(CMD_DEV_INFO_REPORT,tr("设备信息上报至平台"));

    cmdMap.insert(CMD_ERROR_GET,tr("云端下发异常通知"));
    cmdMap.insert(CMD_ERROR_REPORT,tr("设备上报异常通知"));

    cmdMap.insert(CMD_DEV_MANAGE,tr("云端下发设备管理指令"));
    cmdMap.insert(CMD_DEV_CONFIGURE,tr("云端下发设备配置指令"));
    cmdMap.insert(CMD_DEV_REPORT_MANAGER,tr("设备上报管理信息"));
    cmdMap.insert(CMD_DEV_REPORT_MANAGER_ACK,tr("云端回复设备上报管理信息"));
    cmdMap.insert(CMD_DEV_CONFIGURE_REPORT,tr("设备回复云端配置指令"));

    cmdMap.insert(CMD_SUB_DEV_AUTH,tr("设备向云端发起子设备认证"));
    cmdMap.insert(CMD_SUB_DEV_AUTH_ACK,tr("云端回复设备的子设备认证"));
    cmdMap.insert(CMD_SUB_DEV_LOGIN,tr("设备向云端发起子设备登录"));
    cmdMap.insert(CMD_SUB_DEV_LOGIN_ACK,tr("云端回复设备的子设备登录"));
    cmdMap.insert(CMD_SUB_DEV_LOGOFF,tr("设备向云端发起子设备登出"));
    cmdMap.insert(CMD_SUB_DEV_LOGOFF_ACK,tr("云端回复设备的子设备登出"));
    cmdMap.insert(CMD_SUB_DEV_LOGOUT,tr("设备向云端发起子设备注销"));
    cmdMap.insert(CMD_SUB_DEV_LOGOUT_ACK,tr("云端回复设备的子设备注销"));
    cmdMap.insert(CMD_SUB_DEV_LOGOFF_NOTE,tr("云端下发通知设备的子设备已下线"));
    cmdMap.insert(CMD_SUB_DEV_RESET,tr("设备向云端发起子设备重置指令"));
    cmdMap.insert(CMD_SUB_DEV_RESET_ACK,tr("云端回复设备的子设备重置指令"));
    cmdMap.insert(CMD_SUB_DEV_VERIFY,tr("设备回复云端"));

    cmdMap.insert(CMD_OTA_PLAN,tr("云端下发OTA升级版本至设备"));
    cmdMap.insert(CMD_OTA_PLAN_ACK,tr("设备对云端下发ota升级版本的回复"));
    cmdMap.insert(CMD_OTA_URL,tr("云端下发url至设备"));
    cmdMap.insert(CMD_OTA_STATUS_REPORT,tr("设备上报OTA升级状态"));
    cmdMap.insert(CMD_OTA_REQUEST,tr("设备向云端请求OTA升级计划"));

    cmdMap.insert(CMD_LOCAL_REPORT_DATA,tr("设备上报定位信息至平台"));
    cmdMap.insert(CMD_LOCAL_GET_TYPE,tr("云端获取设备的定位信息"));
    cmdMap.insert(CMD_LOCAL_GET_TYPE_ACK,tr("设备回复云端获取定位信息"));
}


QString ttlvDataAnysis::getMapString(int cmd)
{
    return cmdMap[cmd];
}




bool ttlvDataAnysis::analyzeTtlvDataNode(QByteArray data,int *offset,QJsonObject &jsonNode)
{
    bool analysisFlag = true;
    bool ok;
    if(*offset >= data.size() || *offset+2 > data.size())
    {
        return false;
    }
    int id = data.mid(*offset,2).toHex().toInt(&ok,16);
    int type = id&0x07;
    QJsonValue jsonValue;
    *offset += 2;
    id>>=3;
    qDebug()<<"ID:"<<QString::number(id);
    qDebug()<<"type:"<<QString::number(type);

    switch (type)
    {
        case DP_TTLV_TYPE_BOOL_FALSE:
        case DP_TTLV_TYPE_BOOL_TRUE:
        {
            qDebug()<<"bool";
#if 0
            newModelStruct dataInfo;
            dataInfo.idex = id;
            dataInfo.type = "bool";
            dataInfo.data = QVariant((type == 1?true:false));
            dataInfo.externMess = NULL;
            list->modeInfo.append(dataInfo);
#else

//          jsonNode["id"] = id;
//          jsonNode["type"] = "bool";
//          jsonNode["value"] = type == 1?true:false;
            jsonNode.insert(QString::number(id),type == 1?true:false);

#endif
            break;
        }
        case DP_TTLV_TYPE_ENUM_NUM:
        {
            //0042 02 01e240

            qDebug()<<"num" ;
            if(data.length() <= *offset+1)
            {
                return false;
            }
            bool negative = (data.at(*offset) >> 7) ? true : false;
            int amp = (data.at(*offset) >> 3) & 0x0F;
            int tmpLen = (data.at(*offset) & 0x07) + 1;
            int tmpOffset = 0;
            quint64 value_int = 0;
            *offset += 1;
            if(tmpLen+(*offset) > data.size())
            {
                return false;
            }
            for (tmpOffset = 0; tmpOffset < tmpLen; tmpOffset++)
            {
                value_int <<= 8;
                value_int |= (unsigned char)(data.at(tmpOffset + *offset));
            }
            *offset += tmpOffset;
            double vaule_double = (double)value_int;
            if(amp > 0)
            {
                int times = amp;
                while (times--)
                {
                    vaule_double /= 10.0;
                }
            }
            if (negative)
            {
                vaule_double = 0 - vaule_double;
            }
#if 0
            newModelStruct dataInfo;
            dataInfo.idex = id;
            if(amp > 0)
            {
                dataInfo.type = "double";
                dataInfo.data = QVariant(vaule_double);
                dataInfo.externMess = QVariant(amp);
            }
            else
            {
                dataInfo.type = "int";
                dataInfo.data = QVariant(int(vaule_double));
                dataInfo.externMess = NULL;
            }
            qDebug()<<"dataInfo.data:"<<dataInfo.data;
            list->modeInfo.append(dataInfo);
#else
          qDebug()<<"id:"<<id<<"amp:"<<amp;
          if(amp > 0)
          {
//               jsonNode["type"] = "double";
               jsonValue = vaule_double;

          }
          else
          {
//              jsonNode["type"] = "int";
             jsonValue =int(vaule_double);
          }
          jsonNode.insert(QString::number(id),jsonValue);
#endif
            break;
        }
        case DP_TTLV_TYPE_BINARY:
        {
            qDebug()<<"bin";
            if(data.length() <= *offset+2)
            {
                return false;
            }
            int dataLen = data.mid(*offset,2).toHex().toInt(&ok,16);
#if 0
            newModelStruct dataInfo;
            dataInfo.idex = id;
            dataInfo.type = "QByteArray";
            dataInfo.data = QVariant(data.mid(*offset+2,dataLen));
            dataInfo.externMess = NULL;
            list->modeInfo.append(dataInfo);
#else

//          jsonNode["type"] = "text";
          jsonValue = QString(data.mid(*offset+2,dataLen).data());
          jsonNode.insert(QString::number(id),jsonValue);
#endif
          if(data.length() < *offset+dataLen+2)
          {
              return false;
          }
            *offset += 2+dataLen;
            break;
        }
        case DP_TTLV_TYPE_STRUCT:
        {
            qDebug()<<"struct";
            if(data.length() <= *offset+2)
            {
                return false;
            }
            int count = data.mid(*offset,2).toHex().toInt(&ok,16);
            *offset += 2;
//            jsonNode["id"] = id;
//            jsonNode["type"] = "struct";
            if(data.length() < *offset+count)
            {
                return false;
            }
            QJsonArray array;
            for(int i = 0;i<count;i++)
            {
#if 0
                newModelListStruct nextList;
                nextList.head = NULL;
                nextList.level = ++(list->level);
                nextList.parentId = id;
                list->head = &nextList;
                analyzeTtlvDataNode(data,offset,&nextList);
#else
                QJsonObject sonJsonNode;
                if(true == analyzeTtlvDataNode(data,offset,sonJsonNode))
                {
                    array.append(sonJsonNode);
//                    QStringList keyList = sonJsonNode.keys();
//                    for(int i = 0;i<keyList.count();i++)
//                    {
//                        QString key = keyList[i];
//                        array[key] = sonJsonNode[key];
//                    }
                }else
                {
                    return false;
                }

#endif
            }
            jsonValue = array;
            jsonNode.insert(QString::number(id),jsonValue);
            break;
        }
        default:
            qDebug()<<"false";
            analysisFlag = false;
            break;
    }
    return analysisFlag;
}

void ttlvDataAnysis::handleAnalyzeResult(newModelListStruct *list)
{
    newModelListStruct **node = &list;
    QJsonObject jsonHead;
    do
    {
        qDebug()<<"level:"<<(*node)->level;
        for(int i= 0;i<(*node)->modeInfo.size();i++)
        {
            qDebug()<<"id:"<<(*node)->modeInfo.at(i).idex;
            if(0 == (*node)->modeInfo.at(i).type.compare("bool"))
            {
                qDebug()<<"value:"<<(*node)->modeInfo.at(i).data.toBool();
            }
            else if(0 == (*node)->modeInfo.at(i).type.compare("int"))
            {
                qDebug()<<"value:"<<(*node)->modeInfo.at(i).data.toInt();
            }
            else if(0 == (*node)->modeInfo.at(i).type.compare("double"))
            {
                qDebug()<<"value:"<<(*node)->modeInfo.at(i).data.toDouble();
            }
            else if(0 == (*node)->modeInfo.at(i).type.compare("QByteArray"))
            {
                qDebug()<<"value:"<<(*node)->modeInfo.at(i).data.toByteArray();
            }
        }
        node = &((*node)->head);
    }
    while((*node)->head != NULL);
}

void ttlvDataAnysis::anlyzeTtlvData(QByteArray data)
{
    int offset = 0;
    int rowCount = 0;
    int ret;
    bool analysisResult = true;
    QJsonArray array;
    QString sendStr;
    qDebug()<<"data:"<<data;
    qDebug()<<"data.toHex():"<<data.toHex();
    if(TYPE_HAVE_PROTOCOL_HEAD == anlyzeTtlvType(data))
    {
        bool ok;
        sendStr.append(tr("协议头：0xAAAA \n"));
        sendStr.append(tr("数据域长度：%1 \n").arg((data.mid(2,1).toHex().toInt(&ok,16)<<8) +data.mid(3,1).toHex().toInt(&ok,16)));
        sendStr.append(tr("校验和：%1 \n").arg(data.mid(4,1).toHex().toInt(&ok,16)));
        sendStr.append(tr("包Id：%1 \n").arg((data.mid(5,1).toHex().toInt(&ok,16)<<8) +data.mid(6,1).toHex().toInt(&ok,16)));
        sendStr.append(tr("命令字：%1 \n").arg((data.mid(7,1).toHex().toInt(&ok,16)<<8) +data.mid(8,1).toHex().toInt(&ok,16)));
        data = data.mid(9);
    }


    do
    {
        QJsonObject obj;
        ret = analyzeTtlvDataNode(data,&offset,obj);
        qDebug()<<"offset:"<<offset;
        if(ret == false)
        {
            MMessageBox::critical(NULL,tr("错误"),tr("ttlv数据解析错误"),tr("确认"));
            analysisResult = false;
            break;
        }
        else
        {
            rowCount++;
#if 0
            anlyzeStruct(newModeInfoList);
            newModeInfoList.head = NULL;
            newModeInfoList.level = 0;
            newModeInfoList.parentId = -1;
            newModeInfoList.modeInfo.clear();
#else
           qDebug()<<"obj:"<<obj;
//           QStringList keyList = obj.keys();
//           for(int i = 0;i<keyList.count();i++)
//           {
//               array[keyList[i]] = obj[keyList[i]];
//           }
           array.append(obj);
#endif
        }

//        else if(ret == -1)
//            return ret;
    }while(offset<data.length());
    #if 0
        memset(&newModeInfoList,0,sizeof(newModeInfoList));
    #else
        if(analysisResult)
        {
            QJsonDocument document;
            if(array.size()>1)
                document.setArray(array);
            else
                document.setObject(array[0].toObject());
            QByteArray byteArray = document.toJson(QJsonDocument::Indented);
            sendStr =  sendStr+ QString(byteArray);
            qDebug()<<"sendStr:"<<sendStr;
            emit sendTJResult(sendStr);
        }
    #endif

}

int ttlvDataAnysis::anlyzeTtlvType(QByteArray data)
{
    qDebug()<<"data.size():"<<data.size();
    if(data.size() > 10 && "aa" == data.mid(0,1).toHex() && "aa" == data.mid(1,1).toHex())
    {
        qDebug()<<"TYPE_HAVE_PROTOCOL_HEAD";
        return TYPE_HAVE_PROTOCOL_HEAD;
    }
    return TYPE_NONE_PROTOCOL_HEAD;
}

void ttlvDataAnysis::anlyzeStruct(newModelListStruct modeInfoList)
{
    handleAnalyzeResult(&modeInfoList);
    // emit sendTJResult();
}




