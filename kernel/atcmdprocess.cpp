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
#include "atcmdprocess.h"
#include <QDebug>
#include <QDateTime>
#include "userconfigure.h"
atcmdProcess::atcmdProcess(QObject *parent) : QObject(parent)
{
    sendMutexFlag = false;
    timeOutFlag = false;
//    dataRecvTimer = new QTimer();
//    connect(dataRecvTimer, SIGNAL(timeout()), this, SLOT(dataRecvTimeout()));
//    dataRecvTimer->setSingleShot(true);

    sendDataHexFlag = false;
//    runNextTimer = new QTimer(this);
//    connect(runNextTimer, SIGNAL(timeout()), this, SLOT(runNextFunc()));
//    runNextTimer->setSingleShot(true);
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “AT+QIOTREG  配置平台连接模式”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTREG_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    qDebug()<<"atcmd_QIOTREG_process"<<type<<result<<sendArgs<<recvArgs;
    if(type == ATCMD_GET && result)
    {
        emit atEventSend(ATCMD_EVENT_REG_GET,recvArgs);
    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “ AT+QIOTSEND  发送透传数据”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTSEND_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “ AT+QIOTRD  读取透传数据”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTRD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__<<"result:"<<result;
	Q_UNUSED(sendArgs);
    if(type == ATCMD_SET && result)
    {
        emit atEventSend(ATCMD_EVENT_PASSDATA_READ,recvArgs);
    }
    else if(type == ATCMD_TEST && result)
    {
        emit atEventSend(ATCMD_EVENT_PASSDATA_TEST,recvArgs);
    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “ AT+QIOTCFG  配置可选参数”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTCFG_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    qDebug()<<"atcmd_QIOTCFG_process"<<type<<result<<sendArgs<<recvArgs;
    if(type == ATCMD_SET && result) // 不存在GET，这里需要修改整个逻辑
    {
        emit atEventSend(ATCMD_EVENT_CONFIG_GET,recvArgs);
    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “ AT+QIOTMCUVER  配置MCU版本号”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTMCUVER_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    qDebug()<<"type:"<<type;
    Q_UNUSED(sendArgs);
    if(type == ATCMD_GET && result)
    {
        emit atEventSend(ATCMD_EVENT_MCUVER_GET,recvArgs);
    }
    else if(type == ATCMD_SET && result)
    {
        emit atEventSend(ATCMD_EVENT_MCUVER_SET,recvArgs);
    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “ AT+QIOTUPDATE  配置OTA升级行为”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTUPDATE_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “AT+QIOTINFO  请求信息上报”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTINFO_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    Q_UNUSED(sendArgs);
    if(type == ATCMD_GET && result)
    {
        emit atEventSend(ATCMD_EVENT_INFO_GET,recvArgs);
    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “AT+QIOTOTARD  分片读取SOTA数据”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTOTARD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    Q_UNUSED(sendArgs);
    if(type == ATCMD_SET && result)
    {
        emit atEventSend(ATCMD_EVENT_OTAFILE_GET,recvArgs);
    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “AT+QIOTSTATE  查询平台连接状态”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTSTATE_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    Q_UNUSED(sendArgs);
    if(type == ATCMD_GET && result)
    {
        emit atEventSend(ATCMD_EVENT_STATUS,recvArgs);
    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “AT+QIOTMODELTD  发送物模型数据”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTMODELTD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);

//    if(result)
//    {
//        if(recvArgs[3].toInt() == 10210)
//        {
//            emit atEventSend(ATCMD_EVENT_MODEL_SEND,QList<QByteArray>()<<recvArgs.at(3));
//        }
//    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “AT+QIOTMODELRD  读取物模型数据”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTMODELRD_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__<<"type:"<<type;
    Q_UNUSED(sendArgs);
    if(type == ATCMD_SET && result)
    {
        if(recvArgs.count() == 4)
        {
            emit atEventSend(ATCMD_EVENT_MODEL_READ,QList<QByteArray>()<<recvArgs.at(1)<<recvArgs.at(2)<<recvArgs.at(3));
        }
        else
        {
            emit atEventSend(ATCMD_EVENT_MODEL_READ,QList<QByteArray>()<<recvArgs.at(1)<<recvArgs.at(2));
        }

    }
    else if(type == ATCMD_TEST && result)
    {
        emit atEventSend(ATCMD_EVENT_MODEL_TEST,recvArgs);
    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “AT+QIOTLOCIN  获取模组内置定位功能数据”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTLOCIN_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    if(ATCMD_TEST == type)
        return;
    else if(result)
    {
        if((ATCMD_SET == type && sendArgs.at(0).toInt() == 1) || ATCMD_GET == type)
        {
            emit atEventSend(ATCMD_EVENT_QIOTLOCIN,recvArgs);
        }
    }

}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “AT+QIOTLOCEXT  上报外置定位数据”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTLOCEXT_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    Q_UNUSED(sendArgs);
    if(ATCMD_TEST == type)
        return;
    else
    if(result)
    {
        emit atEventSend(ATCMD_EVENT_QIOTLOCEXT,recvArgs);
    }
}
/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “未知数据”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_UNKNOWN_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__;
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

void atcmdProcess::atcmd_QIOTOTAREQ_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

void atcmdProcess::atcmd_QFOTAUP_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

void atcmdProcess::atcmd_QFOTACFG_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

/************************************* 网关子设备 **************************************************/
void atcmdProcess::atcmd_QIOTSUBCONN_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

void atcmdProcess::atcmd_QIOTSUBDISCONN_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

void atcmdProcess::atcmd_QIOTSUBSEND_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

void atcmdProcess::atcmd_QIOTSUBRD_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(sendArgs);
    if(type == ATCMD_TEST && result)
    {
        emit atEventSend(ATCMD_EVENT_QIOTSUBRD_TEST,recvArgs);
    }
    else
    {
        emit atEventSend(ATCMD_EVENT_QIOTSUBRD,recvArgs);
    }
}

void atcmdProcess::atcmd_QIOTSUBTSLTD_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

void atcmdProcess::atcmd_QIOTSUBTSLRD_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    if(result)
    {
        emit atEventSend(ATCMD_EVENT_QIOTSUBTSLRD,recvArgs);
    }

}

void atcmdProcess::atcmd_QIOTSUBINFO_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

void atcmdProcess::atcmd_QIOTSUBHTB_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    Q_UNUSED(recvArgs);
}

void atcmdProcess::atcmd_QIOTSUBEVT_process(atcmd_type_e type, bool result, QList<QByteArray> sendArgs, QList<QByteArray> recvArgs)
{
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    qInfo()<<__FUNCTION__;
    emit atEventSend(ATCMD_EVENT_QIOTSUBEVT,recvArgs);
}


/************************************* 网关子设备 **************************************************/

/**************************************************************************
** 功能	@brief : 对解析后的数据进行处理 “设备接入设备管理平台相关事件”
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::atcmd_QIOTEVT_process(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs)
{
    qInfo()<<__FUNCTION__<<recvArgs;
    Q_UNUSED(type);
    Q_UNUSED(result);
    Q_UNUSED(sendArgs);
    if(recvArgs.isEmpty())
    {
        return;
    }
    if(recvArgs.at(0) == "1")
    {
        if(recvArgs.at(1) == "10200")
        {
            emit atEventSend(ATCMD_EVENT_STATUS,QList<QByteArray>()<<"3");
        }
        else
        {
            emit atEventSend(ATCMD_EVENT_STATUS,QList<QByteArray>()<<"4");
        }
    }
    else if(recvArgs.at(0) == "2")
    {
        if(recvArgs.at(1) == "10200")
        {
            emit atEventSend(ATCMD_EVENT_STATUS,QList<QByteArray>()<<"6");
        }
        else
        {
            emit atEventSend(ATCMD_EVENT_STATUS,QList<QByteArray>()<<"7");
        }
    }
    else if(recvArgs.at(0) == "3")
    {
        if(recvArgs.at(1) == "10200")
        {
            emit atEventSend(ATCMD_EVENT_STATUS,QList<QByteArray>()<<"8");
        }
        else
        {
            emit atEventSend(ATCMD_EVENT_STATUS,QList<QByteArray>()<<"9");
        }
    }
    else if(recvArgs.at(0) == "4")
    {

    }
    else if(recvArgs.at(0) == "5")
    {
        if(recvArgs.at(1) == "10200")
        {
            if(recvArgs.count() == 4)
            {
                emit atEventSend(ATCMD_EVENT_PASSDATA_ISSUE,QList<QByteArray>()<<recvArgs.at(3));
            }
            else if(recvArgs.count() == 2)
            {
                emit atEventSend(ATCMD_EVENT_PASSDATA_ISSUE,QList<QByteArray>());
            }
        }
        else if(recvArgs.at(1) == "10210")
        {
            if(recvArgs.count() == 4)
            {
                emit atEventSend(ATCMD_EVENT_MODEL_ISSUE,QList<QByteArray>()<<recvArgs.at(3));
            }
            else if(recvArgs.count() == 2)
            {
                emit atEventSend(ATCMD_EVENT_MODEL_ISSUE,QList<QByteArray>());
            }
        }
        else if(recvArgs.at(1) == "10211")
        {
            emit atEventSend(ATCMD_EVENT_MODEL_GET,recvArgs.mid(2));
        }
    }
    else if(recvArgs.at(0) == "6")
    {
        if(recvArgs.at(1) == "10200")
        {
            emit atEventSend(ATCMD_EVENT_STATUS,QList<QByteArray>()<<"11");
        }
        else
        {
            emit atEventSend(ATCMD_EVENT_STATUS,QList<QByteArray>()<<"12");
        }
    }
    else if(recvArgs.at(0) == "7" )
    {
        emit atEventSend(ATCMD_EVENT_OTA,recvArgs.mid(1));
    }

}
class atcmdList_type
{
public:
    atcmdList_type(QString str,bool urc,int pos,void (atcmdProcess::*cFun)(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs) )
    {
        key=str;
        isUrc=urc;
        hexLenPos=pos;
        fun=cFun;
    }
    QString key;
    bool isUrc;
    int hexLenPos;
    void (atcmdProcess::*fun)(atcmd_type_e type,bool result,QList<QByteArray> sendArgs,QList<QByteArray> recvArgs);
};

QList<atcmdList_type> ql_ATCMDList =
{
    {"+QIOTREG"     ,0 ,0 ,&atcmdProcess::atcmd_QIOTREG_process      },
    {"+QIOTSEND"    ,0 ,0 ,&atcmdProcess::atcmd_QIOTSEND_process     },
    {"+QIOTRD"      ,0 ,1 ,&atcmdProcess::atcmd_QIOTRD_process       },
    {"+QIOTCFG"     ,0 ,0 ,&atcmdProcess::atcmd_QIOTCFG_process      },
    {"+QIOTMCUVER"  ,0 ,0 ,&atcmdProcess::atcmd_QIOTMCUVER_process   },
    {"+QIOTUPDATE"  ,0 ,0 ,&atcmdProcess::atcmd_QIOTUPDATE_process   },
    {"+QIOTINFO"    ,0 ,0 ,&atcmdProcess::atcmd_QIOTINFO_process     },
    {"+QIOTOTARD"   ,0 ,2 ,&atcmdProcess::atcmd_QIOTOTARD_process    },
    {"+QIOTSTATE"   ,0 ,0 ,&atcmdProcess::atcmd_QIOTSTATE_process    },
    {"+QIOTMODELRD" ,0 ,1 ,&atcmdProcess::atcmd_QIOTMODELRD_process  },
    {"+QIOTMODELTD" ,0 ,0 ,&atcmdProcess::atcmd_QIOTMODELTD_process  },
    {"+QIOTLOCIN"   ,0 ,0 ,&atcmdProcess::atcmd_QIOTLOCIN_process    },
    {"+QIOTLOCEXT"  ,0 ,0 ,&atcmdProcess::atcmd_QIOTLOCEXT_process   },
    {"+QIOTEVT"     ,1 ,3 ,&atcmdProcess::atcmd_QIOTEVT_process      },
    {"+QIOTOTAREQ"   ,0 ,0 ,&atcmdProcess::atcmd_QIOTOTAREQ_process   },
    {"+QFOTAUP"      ,0 ,0 ,&atcmdProcess::atcmd_QFOTAUP_process      },
    {"+QFOTACFG"     ,1 ,0 ,&atcmdProcess::atcmd_QFOTACFG_process     },
    {"+QIOTSUBCONN"  ,0 ,0 ,&atcmdProcess::atcmd_QIOTSUBCONN_process  },
    {"+QIOTSUBDISCONN",0 ,0 ,&atcmdProcess::atcmd_QIOTSUBDISCONN_process  },
    {"+QIOTSUBSEND"  ,0 ,0 ,&atcmdProcess::atcmd_QIOTSUBSEND_process  },
    {"+QIOTSUBRD"    ,0 ,3 ,&atcmdProcess::atcmd_QIOTSUBRD_process  },
    {"+QIOTSUBTSLTD" ,0 ,0 ,&atcmdProcess::atcmd_QIOTSUBTSLTD_process  },
    {"+QIOTSUBTSLRD" ,0 ,3 ,&atcmdProcess::atcmd_QIOTSUBTSLRD_process  },
    {"+QIOTSUBINFO"  ,0 ,0 ,&atcmdProcess::atcmd_QIOTSUBINFO_process  },
    {"+QIOTSUBHEBT"  ,0 ,0 ,&atcmdProcess::atcmd_QIOTSUBHTB_process  },
    {"+QIOTSUBEVT"  ,1 ,5 ,&atcmdProcess::atcmd_QIOTSUBEVT_process  },
};

/**************************************************************************
** 功能	@brief : 对AT命令以"\r\n"结束进行分割
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QByteArray atEngine_LineExtract(QByteArray data,int *outLen)
{
    qInfo()<<__FUNCTION__;
    QByteArray outData;
    int pEnd = 0;
    int offset = 0;
    if((pEnd =data.indexOf("\r\n"))>= 0)
    {
        offset = 2;
    }
    else if((pEnd =data.indexOf("\r")>= 0))
    {
        offset = 0;
    }
    else if((pEnd =data.indexOf("\n")>= 0))
    {
        offset = 0;
    }
    if((pEnd < 0))
    {
        return NULL;
    }
//    qDebug()<<"pEnd:"<<pEnd<<"offset:"<<offset;
    *outLen = pEnd+offset;
    for (int i = 0; i < *outLen; i++)
    {
        outData.append(data.at(i));
    }
    return outData;
}
/**************************************************************************
** 功能	@brief : 匹配已知的AT指令
** 输入	@param :
** 输出	@retval:
***************************************************************************/
atcmdList_type *atcmdFindList(QString data)
{
    qInfo()<<__FUNCTION__;
    for (int i=0;  i < ql_ATCMDList.count(); i++)
    {
        if(data.indexOf(ql_ATCMDList[i].key) >= 0)
        {
            return &ql_ATCMDList[i];
        }
    }
    return NULL;
}
/**************************************************************************
** 功能	@brief : 匹配已知的AT指令类型
** 输入	@param :
** 输出	@retval:
***************************************************************************/
atcmd_type_e atTypeFindList(QString key,QByteArray data)
{
    qInfo()<<__FUNCTION__;
    atcmd_type_e type;
    int keyLen = data.indexOf(key)+key.length();
    if(data.count() <= keyLen)
    {
        return ATCMD_ERROR;
    }
    if(data.at(keyLen) == '=' && (data.count()>=keyLen+2 && data.at(keyLen+1) == '?'))
    {
        type = ATCMD_TEST;
    }
    else if(data.at(keyLen) == '=' ) // 这里存在AT+QIOTCFG="server"的情况
    {
        type = ATCMD_SET;
    }
    else if(data.at(keyLen) == '?' )
    {
        type = ATCMD_GET;
    }
    else
    {
        type = ATCMD_EXEC;
    }
    return type;
}
/**************************************************************************
** 功能	@brief : 获取参数
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QList<QByteArray> atArgsExtract(QString key,QByteArray data)
{
    qInfo()<<__FUNCTION__;
    QList<QByteArray> argList;
    QList<QByteArray> argList2;
    int start = data.indexOf(key);
    // qDebug()<<key.length();
    if(data.count() <= start+key.length())
    {
        return argList;
    }
    if(start >= 0)
    {
        start += key.length();
        if(data.at(start) == '"' && data.at(data.length()-1) == '"')
        {
            QByteArray payload = data.mid(start+1,data.length()-start-2);
            qDebug()<<"都是双引号"<<payload;
            if(payload.indexOf('"') < 0)
            {
                qDebug()<<"作为一个参数处理";
                return QList<QByteArray>()<<payload;
            }

        }

        argList = data.mid(start).split(',');
    }
    foreach (QByteArray arg, argList)
    {
start:
        if(arg.front() == '"' && arg.back() == '"')
        {
            argList2.append(arg.mid(1,arg.length()-2));
        }
        else if(arg.indexOf('\\') == 0 && arg.lastIndexOf('"') == 0)
        {
            argList2.append(arg.mid(2,arg.length()-4));
        }
        else if(!arg.isEmpty())
        {
            // qDebug()<<__FUNCTION__<<"arg:"<<arg;
            int place = arg.indexOf("\r\n");
            if(place > 0)
            {
                arg = arg.mid(0,place);
                goto start;
            }
            else
            {
                argList2.append(arg);
            }
        }
    }
    return argList2;
}
/**************************************************************************
** 功能	@brief : 获取AT指令中的命令
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QString atKeyExtract(QByteArray data)
{
    qInfo()<<__FUNCTION__;
    QString key;
    int start = data.indexOf("+");
    int end = data.indexOf(":");
    if(start >= 0 && end >= 0)
    {
        key = data.mid(start,end-start);
    }
    return key;
}
/**************************************************************************
** 功能	@brief : 发送AT指令的解释
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool atcmdProcess::atcmdSendAnalysis(QByteArray data)
{
    qInfo()<<__FUNCTION__;
    atcmdList_type *cmdInfo = atcmdFindList(data);

    if(cmdInfo)
    {
        sendInfo.type = atTypeFindList(cmdInfo->key,data);
        if(sendInfo.type == ATCMD_SET)
        {
            sendInfo.sendArgs = atArgsExtract("=",data);
            if(sendInfo.sendArgs.count() == 0)
            {
                qCritical()<<"errno sendArgs";
                return false;
            }
        }
        else if(sendInfo.type == ATCMD_ERROR)
        {
            qCritical()<<"errno type";
            return false;
        }
        sendInfo.key = cmdInfo->key;
        sendInfo.enable = true;
    }
    return true;
}
/**************************************************************************
** 功能	@brief :
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::dataRecvTimeout()
{
//    if(sendMutexFlag != false)
//        sendMutexFlag = false;
//    qDebug()<<"end:" <<QDateTime::currentDateTime();
//    dataRecvTimer->stop();

}
/**************************************************************************
** 功能	@brief : 数据分析
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int atcmdProcess::atcmdRecvAnalysis(QByteArray data,QByteArray &hexData)
{
    qInfo()<<__FUNCTION__;
    if(data.size() <=0)
        return 0;
    qDebug()<<"data:"<<data;
    int dataPos = 0;
    atcmdList_type *cmdInfo = NULL;
    atcmdList_type *newCmdInfo = NULL;
    bool handleDataFlag = false;
//    qDebug()<<"sendMutexFlag:"<<sendMutexFlag;
//    qDebug()<<"data.length() :"<<data.length() ;

    // 判断是否为 正确的 ‘>’
    if( sendMutexFlag == true && data.length() < 10 && data.indexOf(">")>=0)
    {
        bool dataFlag = false;
        foreach(char c,data)
        {
            if(c != '\r' || c != '\n' || c!= '>' || c!= ' ')
                dataFlag = true;
        }
        if(dataFlag == true)
        {
            if(!sendDataHex.isEmpty())
            {
                sendDataHexFunc();
            }
            else
                sendDataHexFlag = true;
            return data.length();
        }

    }
    // 判断是否为 正确的 命令
    if((newCmdInfo = atcmdFindList(data)) != NULL)
    {
        QList<QByteArray> recvArgs;
        do
        {
            int lineLen = 0;
            QByteArray lineData = atEngine_LineExtract(data.mid(dataPos),&lineLen);
//            qDebug()<<"lineData:"<<lineData;
//            qDebug()<<"dataPos:"<<dataPos<<",lineLen:"<<lineLen;
            if(lineLen == 0)
            {
                if(sendMutexFlag && data.mid(dataPos).startsWith(">"))
                {
                    if(!sendDataHex.isEmpty() && (data.mid(dataPos).indexOf("OK")==-1 && data.mid(dataPos).indexOf("ERROR")==-1))
                    {
                        sendDataHexFunc();
                    }
                    else
                        sendDataHexFlag = true;
                    return data.length();
                }
                return 0;
            }
            if(sendMutexFlag && data.mid(dataPos).startsWith(">"))
            {
                if(!sendDataHex.isEmpty() && (data.mid(dataPos).indexOf("OK")==-1 && data.mid(dataPos).indexOf("ERROR")==-1))
                {
                    sendDataHexFunc();
                }
                else
                    sendDataHexFlag = true;
                return data.length();
            }
            dataPos += lineLen;
            if((newCmdInfo =atcmdFindList(lineData))!= NULL )
            {
                cmdInfo = newCmdInfo;
                if(lineData.startsWith(":") || lineData.indexOf((cmdInfo->key+":")) >=0 )
                {

                    // 获取参数
                    recvArgs.append(atArgsExtract(": ",lineData));
                     qDebug()<<"recvArgs:"<<recvArgs;
                    // 有字节流
                    if(cmdInfo->hexLenPos > 0 && recvArgs.count() >= cmdInfo->hexLenPos)
                    {
                        int hexLen = recvArgs.at(cmdInfo->hexLenPos-1).toInt();
                        qDebug()<<"hexLen:"<<hexLen;
                        // 物模型 无缓存模式 无法正常，特此备注以下代码
                        // 貌似具备冲突，后续再改进
                        if((cmdInfo->isUrc && (cmdInfo->hexLenPos != 3)) && !(cmdInfo->key.contains("+QIOTSUBEVT")))
                            hexLen= 0;
                        // qDebug()<<"hexLen:"<<hexLen<<"hexLen+dataPos:"<<hexLen+dataPos<<"data.length() :"<<data.length() ;
                        if(hexLen > 0 && data.length() > hexLen+dataPos && (data.at(hexLen+dataPos) == '\r' || data.at(hexLen+dataPos) == '\n'))
                        {
                            hexData.append(data.mid(dataPos,hexLen));
                            recvArgs.append(data.mid(dataPos,hexLen));
                            dataPos += hexLen;
                        }
                        else if(hexLen > 0 && data.length() <= hexLen+dataPos && sendMutexFlag)
                        {
                            return 0;
                        }
                        else if(hexLen > 0 &&data.length() > hexLen+dataPos && sendMutexFlag)
                        {
                            dataPos += hexLen;
                        }
                    }
                    qDebug()<<"after recvArgs:"<<recvArgs;
//                    qDebug()<<"cmdInfo_key:"<<cmdInfo->key<<"isUrc:"<<cmdInfo->isUrc;
                    if(cmdInfo->isUrc)
                    {
//                        qDebug()<<"urc:"<<"recvArgs:"<<recvArgs;
//                        sendInfo.type = ATCMD_SET;
                        setNextFuncRun(cmdInfo->key,sendInfo,false,recvArgs);
                        cmdInfo = NULL;
                        recvArgs.clear();
                        qDebug()<<"next";
                        continue;
//                        break;
                    }

                    if(sendMutexFlag && handleDataFlag == false )
                    {
                        QByteArray array = data.mid(dataPos);
                        if(array.indexOf("ERROR") == -1 && array.indexOf("OK")== -1)
                        {
                            return 0;
                        }
                    }

                }
            }
            else if(lineData.startsWith("OK") || lineData.startsWith("ERROR"))
            {
                handleDataFlag = true;
                if(lineData.startsWith("OK"))
                {
                    atHubInstance.recvAtSendAck();
                }
            }
            else if(sendMutexFlag && data.mid(dataPos).startsWith(">") && (data.mid(dataPos).indexOf("OK")==-1 && data.mid(dataPos).indexOf("ERROR")==-1))
            {
                if(!sendDataHex.isEmpty())
                {
                    sendDataHexFunc();
                }
                else
                    sendDataHexFlag = true;
                return 0;
            }
            if(handleDataFlag)
            {
                int mode = -1;
                if(lineData.startsWith("ERROR"))
                {
                    mode = 0;
                }else if(lineData.startsWith("OK"))
                {
                    mode = 1;
                }
                if(mode == 1 || mode == 0)
                {
                    if(1 == mode)
                    {
                        atHubInstance.recvAtSendAck();
                    }
                    if(sendInfo.enable)
                    {
                        if(cmdInfo != NULL)
                        {
                            if(cmdInfo->key.count() != 0)
                            {
                                setNextFuncRun(cmdInfo->key,sendInfo,mode,recvArgs);
                                if(cmdInfo->key == sendInfo.key)
                                    sendInfo.enable = false;
                                cmdInfo = NULL;
                            }
                            else
                            {
                                setNextFuncRun(sendInfo.key,sendInfo,mode,recvArgs);
                                sendInfo.enable = false;
                            }
                        }
                        else
                            sendInfo.enable = false;
                    }
                    handleDataFlag = true;
                    break;
                }

            }

         }while(dataPos<data.length());
    }
    else if(sendMutexFlag)
    {
        int mode = -1;
        if(data.indexOf("ERROR") >=0)
        {
            mode = 0;
        }else if(data.indexOf("OK") >=0)
        {
            mode = 1;
        }
        if(mode == 1 || mode == 0)
        {
            if(1 == mode)
            {
                atHubInstance.recvAtSendAck();
            }
            if(sendInfo.enable)
            {
                if(cmdInfo == NULL)
                {
                    cmdInfo = atcmdFindList(sendInfo.key);
                    if(cmdInfo == NULL)
                        return data.count();
                }
                QList<QByteArray> recvArgs2;
                setNextFuncRun(cmdInfo->key,sendInfo,mode,recvArgs2);
                cmdInfo = NULL;
                sendInfo.enable = false;
            }
            handleDataFlag = true;
         }
    }
    else
    {
        sendMutexFlag = false;
        return data.count();
    }
    qDebug()<<"sendMutexFlag:"<<sendMutexFlag<<"handleDataFlag:"<<handleDataFlag;
    if(sendMutexFlag == true && handleDataFlag == true)
    {
        sendMutexFlag = false;
    }
    qDebug()<<"dataPos:"<<dataPos;
    if(dataPos > 0)
        return dataPos;
    else
        return data.count();
}
/**************************************************************************
** 功能	@brief : 设置待执行的函数
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::setNextFuncRun(QString key, sendInfo_t sinfo, bool result, QList<QByteArray> recvData)
{
    qInfo()<<__FUNCTION__;
    eventStruct currentEvent;
    currentEvent.key = key;
    currentEvent.sinfo = sinfo;
    currentEvent.result = result;
    currentEvent.recvData = recvData;
    currentEventList.append(currentEvent);
}

/**************************************************************************
** 功能	@brief : 运行待执行的函数
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::runNextFunc()
{
    qInfo()<<__FUNCTION__;

    if(currentEventList.count() >0)
    {
        for(int i=0;i<currentEventList.count();i++)
        {
            eventStruct currentEvent = currentEventList[i];
            if(!currentEvent.key.isEmpty())
            {

                atcmdList_type *cmdInfo = atcmdFindList(currentEvent.key);
                (this->*(cmdInfo->fun))(currentEvent.sinfo.type,currentEvent.result,currentEvent.sinfo.sendArgs,currentEvent.recvData);
                currentEvent.key.clear();
            }
            else
                qDebug()<<"error";
        }
        currentEventList.clear();
    }

}

void atcmdProcess::runNextFuncAgain()
{
    runNextFunc();
}
/**************************************************************************
** 功能	@brief :发送字节流数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdProcess::sendDataHexFunc()
{
    qDebug()<<"sendDataHexFlag:"<<sendDataHexFlag;
    if(sendDataHexFlag && !sendDataHex.isEmpty())
    {
        emit atEventSend(ATCMD_EVENT_SENDHEX,QList<QByteArray>()<<this->sendDataHex);
        sendDataHex.clear();
    }
}



