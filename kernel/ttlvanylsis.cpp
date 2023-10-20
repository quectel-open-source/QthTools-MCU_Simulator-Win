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
#include "ttlvanylsis.h"
#include <QWidget>
#include "kernel/mmessagebox.h"
#include <QDebug>

ttlvAnylsis::ttlvAnylsis()
{

}


/**************************************************************************
** 功能	@brief : int 转 char
** 输入	@param :
** 输出	@retval:
***************************************************************************/
char intGetChar(uint64_t Number,int position,int NumSize)
{
    uint64_t highByte = ((uint64_t)0xff << (NumSize-position)*8)&Number;
    return (uchar)(highByte>>((NumSize-position)*8));
}
/**************************************************************************
** 功能	@brief : int 转 byte
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QByteArray IntToByte(uint64_t Number, int NumSize)
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
int getDoubleAccuracy(double tempDouble)
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
int getArrayMax(int data[],int count)
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


bool ttlvAnylsis::analyzeTtlvDataNode(QByteArray data,int *offset,newModelListStruct *list)
{
    bool analysisFlag = true;
    bool ok;
    int id = data.mid(*offset,2).toHex().toInt(&ok,16);
    int type = id&0x07;
    switch (type)
    {
        case DP_TTLV_TYPE_BOOL_FALSE:
        case DP_TTLV_TYPE_BOOL_TRUE:
        {
            newModelStruct dataInfo;
            dataInfo.idex = id;
            dataInfo.type = "bool";
            dataInfo.data = QVariant((type == 1?true:false));
            dataInfo.externMess.clear();
            list->modeInfo.append(dataInfo);
            break;
        }
        case DP_TTLV_TYPE_ENUM_NUM:
        {
            //0042 02 01e240

            bool negative = (data.at(*offset) >> 7) ? true : false;
            int amp = (data.at(*offset) >> 3) & 0x0F;
            int tmpLen = (data.at(*offset) & 0x07) + 1;
            int tmpOffset;
            quint64 value_int = 0;
            *offset += 1;
            for (tmpOffset = 0; tmpOffset < tmpLen; tmpOffset++)
            {
                value_int <<= 8;
                value_int |= (unsigned char)data.at(tmpOffset + *offset);
            }
            *offset += tmpOffset;
            double vaule_double = (double)value_int;
            while (amp--)
            {
                vaule_double /= 10.0;
            }
            if (negative)
            {
                vaule_double = 0 - vaule_double;
            }
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
                dataInfo.data = QVariant(vaule_double);
                dataInfo.externMess.clear();
            }
            list->modeInfo.append(dataInfo);
            break;
        }
        case DP_TTLV_TYPE_BINARY:
        {
            int dataLen = data.mid(*offset,2).toHex().toInt(&ok,16);
            newModelStruct dataInfo;
            dataInfo.idex = id;
            dataInfo.type = "QByteArray";
            dataInfo.data = QVariant(data.mid(*offset+2,dataLen));
            dataInfo.externMess.clear();
            list->modeInfo.append(dataInfo);
            *offset += 2+dataLen;
            break;
        }
        case DP_TTLV_TYPE_STRUCT:
        {
            int count = data.mid(*offset,2).toHex().toInt(&ok,16);
            *offset += 2;
            for(int i = 0;i<count;i++)
            {
                newModelListStruct nextList;
                nextList.head = NULL;
                nextList.level = ++(list->level);
                list->head = &nextList;

                analyzeTtlvDataNode(data,offset,&nextList);
            }
            break;
        }
        default:
            analysisFlag = false;
            break;
    }
    return analysisFlag;
}

void ttlvAnylsis::handleAnalyzeResult(newModelListStruct *list)
{
    newModelListStruct **node = &list;
    while((*node)->head != NULL)
    {
        qDebug()<<"level:"<<(*node)->level;
        for(int i= 0;i<(*node)->modeInfo.size();i++)
        {
            qDebug()<<"idex:"<<(*node)->modeInfo.at(i).idex;
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
    }
}

void ttlvAnylsis::anlyzeTtlvData(QByteArray data)
{
    qDebug()<<"data:"<<data;
    int offset = 0;
    int rowCount = 0;
    int ret;
    do
    {
        ret = analyzeTtlvDataNode(data,&offset,&newModeInfoList);
        if(ret == false)
        {
            MMessageBox::critical(NULL,QObject::tr("错误"),QObject::tr("物模型数据解析错误"),QObject::tr("确认"));
            break;
        }
        else
        {
            rowCount++;
        }
//        else if(ret == -1)
//            return ret;
    }while(offset<data.length());
    qDebug()<< offset;
}




