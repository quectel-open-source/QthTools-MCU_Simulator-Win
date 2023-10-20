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
#include "atcmdanalysis.h"
#include <QTextBlock>
#include <QDebug>
atcmdAnalysis::atcmdAnalysis()
{

}

class anlysisAtcmdList_type
{
public:
    anlysisAtcmdList_type(QString str,void (atcmdAnalysis::*cFun)(QString data) )
    {
        key=str;
        fun=cFun;
    }
    QString key;
    void (atcmdAnalysis::*fun)(QString data);
};

QList<anlysisAtcmdList_type> ql_AnalysisATCMDList =
{
    {"QIOTREG"     ,&atcmdAnalysis::analysis_QIOTREG      },
    {"QIOTSEND"    ,&atcmdAnalysis::analysis_QIOTSEND     },
    {"QIOTRD"      ,&atcmdAnalysis::analysis_QIOTRD       },
    {"QIOTCFG"     ,&atcmdAnalysis::analysis_QIOTCFG      },
    {"QIOTMCUVER"  ,&atcmdAnalysis::analysis_QIOTMCUVER   },
    {"QIOTUPDATE"  ,&atcmdAnalysis::analysis_QIOTUPDATE   },
    {"QIOTINFO"    ,&atcmdAnalysis::analysis_QIOTINFO     },
    {"QIOTOTARD"   ,&atcmdAnalysis::analysis_QIOTOTARD    },
    {"QIOTSTATE"   ,&atcmdAnalysis::analysis_QIOTSTATE    },
    {"QIOTMODELRD" ,&atcmdAnalysis::analysis_QIOTMODELRD  },
    {"QIOTMODELTD" ,&atcmdAnalysis::analysis_QIOTMODELTD  },
    {"QIOTLOCIN"   ,&atcmdAnalysis::analysis_QIOTLOCIN    },
    {"QIOTLOCEXT"  ,&atcmdAnalysis::analysis_QIOTLOCEXT   },
    {"QIOTEVT"     ,&atcmdAnalysis::analysis_QIOTEVT      },
    {"QIOTOTAREQ"  ,&atcmdAnalysis::analysis_QIOTOTAREQ   },
    {"QFOTAUP"     ,&atcmdAnalysis::analysis_QFOTAUP      },
    {"QFOTACFG"    ,&atcmdAnalysis::analysis_QFOTACFG     },
    {"+QIOTSUBCONN"     ,&atcmdAnalysis::analysis_QIOTSUBCONN  },
    {"+QIOTSUBDISCONN"  ,&atcmdAnalysis::analysis_QIOTSUBDISCONN  },
    {"+QIOTSUBSEND"     ,&atcmdAnalysis::analysis_QIOTSUBSEND  },
    {"+QIOTSUBRD"       ,&atcmdAnalysis::analysis_QIOTSUBRD  },
    {"+QIOTSUBTSLTD"    ,&atcmdAnalysis::analysis_QIOTSUBTSLTD  },
    {"+QIOTSUBTSLRD"    ,&atcmdAnalysis::analysis_QIOTSUBTSLRD  },
    {"+QIOTSUBINFO"     ,&atcmdAnalysis::analysis_QIOTSUBINFO  },
    {"+QIOTSUBHBT"     ,&atcmdAnalysis::analysis_QIOTSUBHBT  },
    {"+QIOTSUBEVT"      ,&atcmdAnalysis::analysis_QIOTSUBEVT  }
};

/**************************************************************************
** 功能	@brief : 匹配已知的AT指令
** 输入	@param :
** 输出	@retval:
***************************************************************************/
anlysisAtcmdList_type* analysis_atcmdMatchList(QString data)
{
    qInfo()<<__FUNCTION__<<data;
    for (int i=0;  i < ql_AnalysisATCMDList.count(); i++)
    {
        if(data.indexOf(ql_AnalysisATCMDList[i].key) >= 0)
        {
            return &ql_AnalysisATCMDList[i];
        }
    }
    return NULL;
}
/**************************************************************************
** 功能	@brief : 调用已匹配的AT指令对应的回调
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::handle_atcmdMatchList(QString data)
{
    anlysisAtcmdList_type *info = analysis_atcmdMatchList(data);
    if(NULL != info)
    {
        (this->*(info->fun))(data);
    }

}
/**************************************************************************
** 功能	@brief : 解析QIOTREG
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTREG(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("平台自动连接模式命令")));
    if(data.indexOf("AT+QIOTREG=") >= 0)
    {
        if(data.contains("AT+QIOTREG=?"))
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
        }
        else
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
            currentParaList.append(qMakePair(QObject::tr("连接模式"),QString(data.split("=").at(1))));
        }
    }
    else if(data.indexOf("AT+QIOTREG?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTREG:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
        QStringList list = data.split(":");
        if(list.count() >= 2 )
        {
            currentParaList.append(qMakePair(QObject::tr("连接模式"),removeDoubleQuotationMarks(list.at(1))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTSTATE
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSTATE(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("平台连接状态查询")));
    if(data.indexOf("AT+QIOTSTATE?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTSTATE:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
        QStringList list = data.split(":");

        if(list.count() < 2)
        {
            return;
        }
        bool isNumber = false;
        int number = QString(list.at(1)).toInt(&isNumber);
        if(isNumber)
        {
            switch (number)
            {
            case 0:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("未初始化")));
                break;
            case 1:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("已初始化")));
                break;
            case 2:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("正在认证")));
                break;
            case 3:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("认证成功")));
                break;
            case 4:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("认证失败")));
                break;
            case 5:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("正在注册")));
                break;
            case 6:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("注册成功，等待订阅")));
                break;
            case 7:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("注册失败")));
                break;
            case 8:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("已订阅，数据可发送")));
                break;
            case 9:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("订阅失败")));
                break;
            case 10:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("正在注销")));
                break;
            case 11:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("注销成功")));
                break;
            case 12:
                currentParaList.append(qMakePair(QObject::tr("模组注册状态"),QObject::tr("注销失败")));
                break;
            default:
                break;
            }
        }

    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTSEND
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSEND(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("发送透传数据")));
    if(data.indexOf("AT+QIOTSEND=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
    }
    else if(data.indexOf("AT+QIOTSEND=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
        QStringList textValue = QString(data.split("=").at(1)).split(",");
        currentParaList.append(qMakePair(QObject::tr("发送模式"),QString(textValue.at(0))));
        currentParaList.append(qMakePair(QObject::tr("发送数据字节数"),QString(textValue.at(1))));
        if(textValue.count() == 3)
        {
            currentParaList.append(qMakePair(QObject::tr("发送数据"),QString(textValue.at(2)).replace("\"","")));
        }
    }


}
/**************************************************************************
** 功能	@brief : 解析QIOTRD
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTRD(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("读取透传数据")));
    if(data.indexOf("AT+QIOTRD=") >= 0)
    {
        if(data.contains("AT+QIOTRD=?"))
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
        }
        else
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
            currentParaList.append(qMakePair(QObject::tr("读取字节数"),QString(data.split("=").at(1))));
        }
    }
    else if(data.indexOf("AT+QIOTRD?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTRD:") >= 0)
    {
        QStringList dataList = data.split(":");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        if(textValue.count() == 3)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置响应")));
            currentParaList.append(qMakePair(QObject::tr("返回字节数"),QString(textValue.at(0))));
            currentParaList.append(qMakePair(QObject::tr("当前数据包剩余字节数"),QString(textValue.at(1))));
            currentParaList.append(qMakePair(QObject::tr("剩余数据包个数"),QString(textValue.at(2))));
        }
        else if(textValue.count() == 1)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
            currentParaList.append(qMakePair(QObject::tr("剩余数据包个数"),QString(textValue.at(0))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTCFG
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTCFG(QString data)
{
    if(data.contains("AT+QIOTCFG=?"))
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
        return;
    }
    if(data.indexOf("productinfo") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置产品信息")));
        if(data.indexOf("AT+QIOTCFG=") >= 0)
        {
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 1)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
            }
            else if(textValue.count() == 3)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
                currentParaList.append(qMakePair(QObject::tr("产品号"),QString(textValue.at(1)).replace("\"","")));
                currentParaList.append(qMakePair(QObject::tr("产品秘钥"),QString(textValue.at(2)).replace("\"","")));
            }

        }
        else if(data.indexOf("+QIOTCFG:") >= 0)
        {
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 4)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
                currentParaList.append(qMakePair(QObject::tr("产品号"),QString(textValue.at(1)).replace("\"","")));
                currentParaList.append(qMakePair(QObject::tr("产品秘钥"),QString(textValue.at(2)).replace("\"","")));
                currentParaList.append(qMakePair(QObject::tr("设备认证协议版本号"),QString(textValue.at(3)).replace("\"","")));
            }
        }
    }
    else if(data.indexOf("server") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置服务器信息")));
        if(data.indexOf("AT+QIOTCFG=") >= 0)
        {
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 1)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
            }
            else if(textValue.count() == 3)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
                if(textValue.at(1) == "0")
                {
                    currentParaList.append(qMakePair(QObject::tr("协议类型"),QObject::tr("LwM2M协议")));
                }
                else if(textValue.at(1) == "1")
                {
                    currentParaList.append(qMakePair(QObject::tr("协议类型"),QObject::tr("MQTT协议")));
                }
                currentParaList.append(qMakePair(QObject::tr("认证/引导服务器"),QString(textValue.at(2)).replace("\"","")));
            }
        }
        else if(data.indexOf("+QIOTCFG: ") >= 0)
        {
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 3)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
                if(textValue.at(1) == "0")
                {
                    currentParaList.append(qMakePair(QObject::tr("协议类型"),QObject::tr("LwM2M协议")));
                }
                else if(textValue.at(1) == "1")
                {
                    currentParaList.append(qMakePair(QObject::tr("协议类型"),QObject::tr("MQTT协议")));
                }
                currentParaList.append(qMakePair(QObject::tr("认证/引导服务器"),QString(textValue.at(2)).replace("\"","")));
            }
        }
    }
    else if(data.indexOf("lifetime") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置心跳值")));
        if(data.indexOf("AT+QIOTCFG=") >= 0)
        {
            QStringList textValue = QString(data.split("=").at(1)).split(",");
            if(textValue.count() == 1)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
            }
            else if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
                currentParaList.append(qMakePair(QObject::tr("设备生命周期"),QString(textValue.at(1))));
            }
        }
        else if(data.indexOf("+QIOTCFG: ") >= 0)
        {

            QStringList textValue = QString(data.split(": ").at(1)).split(",");
            if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
                currentParaList.append(qMakePair(QObject::tr("设备生命周期"),QString(textValue.at(1))));
            }
        }
    }
    else if(data.indexOf("buffer") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置下行数据缓存模式")));
        if(data.indexOf("AT+QIOTCFG=") >= 0)
        {
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 1)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
            }
            else if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
                if(textValue.at(1) == "0")
                {
                    currentParaList.append(qMakePair(QObject::tr("下行数据缓存模式"),QObject::tr("关闭缓存模式")));
                }
                else if(textValue.at(1) == "1")
                {
                    currentParaList.append(qMakePair(QObject::tr("下行数据缓存模式"),QObject::tr("开启缓存模式")));
                }
            }
        }
        else if(data.indexOf("+QIOTCFG: ") >= 0)
        {
            QStringList dataList = data.split(": ");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
                if(textValue.at(1) == "0")
                {
                    currentParaList.append(qMakePair(QObject::tr("下行数据缓存模式"),QObject::tr("关闭缓存模式")));
                }
                else if(textValue.at(1) == "1")
                {
                    currentParaList.append(qMakePair(QObject::tr("下行数据缓存模式"),QObject::tr("开启缓存模式")));
                }
            }
        }
    }
    else if(data.indexOf("act") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置PDP contextID")));

        if(data.indexOf("AT+QIOTCFG=") >= 0)
        {
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 1)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
            }
            else if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
                currentParaList.append(qMakePair(QObject::tr("场景"),QString(textValue.at(1))));
            }
        }
        else if(data.indexOf("+QIOTCFG: ") >= 0)
        {
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
                currentParaList.append(qMakePair(QObject::tr("场景"),QString(textValue.at(1))));
            }
        }
    }
    else if(data.indexOf("tsl") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置通信模式")));
        if(data.indexOf("AT+QIOTCFG=") >= 0)
        {
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 1)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
            }
            else if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
                if(textValue.at(1) == "0")
                {
                    currentParaList.append(qMakePair(QObject::tr("通信模式"),QObject::tr("TTLV格式通信模式")));
                }
                else if(textValue.at(1) == "1")
                {
                    currentParaList.append(qMakePair(QObject::tr("通信模式"),QObject::tr("JSON格式通信模式")));
                }
            }
        }
        else if(data.indexOf("+QIOTCFG: ") >= 0)
        {
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
                currentParaList.append(qMakePair(QObject::tr("当前配置通信模式"),QString(QString(textValue.at(1)).toInt() == 0 ? QObject::tr("TTLV格式通信模式"):QObject::tr("JSON格式通信模式"))));
            }
        }
    }
    else if(data.indexOf("session_mode") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置通信加密模式")));
        if(data.indexOf("AT+QIOTCFG=") >= 0)
        {
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 1)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
            }
            else if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
                currentParaList.append(qMakePair(QObject::tr("加密模式"),QString(QString(textValue.at(1)).toInt() == 0 ? QObject::tr("关闭加密模式"):QObject::tr("开启加密模式"))));
            }
        }
        else if(data.indexOf("+QIOTCFG: ") >= 0)
        {
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
                currentParaList.append(qMakePair(QObject::tr("当前加密模式"),QString(QString(textValue.at(1)).toInt() == 0 ? QObject::tr("关闭加密模式"):QObject::tr("开启加密模式"))));
            }
        }
    }
    else if(data.indexOf("dk_ds") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置设备名称和密码")));
        if(data.indexOf("AT+QIOTCFG=") >= 0)
        {
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 1)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
            }
            else if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
                currentParaList.append(qMakePair(QObject::tr("设备名称"),QString(textValue.at(1))));
            }
            else if(textValue.count() == 3)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
                currentParaList.append(qMakePair(QObject::tr("设备名称"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
                currentParaList.append(qMakePair(QObject::tr("设备密钥"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            }

        }
        else if(data.indexOf("+QIOTCFG: ") >= 0)
        {
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            if(textValue.count() == 2)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
                currentParaList.append(qMakePair(QObject::tr("当前设备号"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
            }
            else if(textValue.count() == 3)
            {
                currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
                currentParaList.append(qMakePair(QObject::tr("当前设备号"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
                currentParaList.append(qMakePair(QObject::tr("当前设备秘钥"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            }
        }
    }
    else
    {
        currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置命令")));
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTMCUVER
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTMCUVER(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置MCU版本号")));
    if(data.contains("AT+QIOTMCUVER=?"))
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
    }
    else if(data.indexOf("AT+QIOTMCUVER=") >= 0)
    {
        QStringList dataList = data.split("=");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
        currentParaList.append(qMakePair(QObject::tr("MCU编号"),QString(textValue.at(0)).replace("\"","")));
        if(textValue.count() == 2)
        {
            currentParaList.append(qMakePair(QObject::tr("MCU对应的版本"),QString(textValue.at(1)).replace("\"","")));
        }
    }
    else if(data.indexOf("AT+QIOTMCUVER?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTMCUVER:") >= 0)
    {
        QStringList dataList = data.split(":");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        if(textValue.count() == 2)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置响应")));
            currentParaList.append(qMakePair(QObject::tr("MCU编号"),QString(textValue.at(0)).replace("\"","")));
            currentParaList.append(qMakePair(QObject::tr("MCU对应的版本"),QString(textValue.at(1)).replace("\"","")));
        }
        else
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
            QStringList infoList = QString(dataList.at(1)).replace("\"","").split(";");
            for(int i=0;i<infoList.count();i++)
            {
                currentParaList.append(qMakePair(QString(infoList.at(i)).split(":").at(0),QString(infoList.at(i)).split(":").at(1)));
            }
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTINFO
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTINFO(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("设备信息上报")));
    if(data.contains("AT+QIOTINFO=?"))
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
    }
    else if(data.indexOf("AT+QIOTINFO=\"up\"") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
    }
    else if(data.indexOf("AT+QIOTINFO?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTINFO:") >= 0)
    {
        QStringList dataList = data.split(":");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList infoList = QString(dataList.at(1)).replace("\"","").split(",");
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
        currentParaList.append(qMakePair(QString(infoList.at(0)),QString(removeDoubleQuotationMarks(infoList.at(1)))));
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTUPDATE
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTUPDATE(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("OTA确认")));
    if(data.contains("AT+QIOTUPDATE=?"))
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
    }
    else if(data.indexOf("AT+QIOTUPDATE=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
        if(data.split("=").at(1) == "0")
        {
            currentParaList.append(qMakePair(QObject::tr("OTA确认升级行为"),QObject::tr("拒绝升级")));
        }
        else if(data.split("=").at(1) == "1")
        {
            currentParaList.append(qMakePair(QObject::tr("OTA确认升级行为"),QObject::tr("确认升级")));
        }
        else if(data.split("=").at(1) == "2")
        {
            currentParaList.append(qMakePair(QObject::tr("OTA升级行为"),QObject::tr("MCU请下载下一块数据")));
        }
        else if(data.split("=").at(1) == "3")
        {
            currentParaList.append(qMakePair(QObject::tr("OTA升级行为"),QObject::tr("MCU上报更新中状态")));
        }

    }
    else if(data.indexOf("AT+QIOTUPDATE?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTUPDATE:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询响应")));
        if(data.split(": ").at(1) == "0")
        {
            currentParaList.append(qMakePair(QObject::tr("OTA确认升级行为"),QObject::tr("拒绝升级")));
        }
        else if(data.split(": ").at(1) == "1")
        {
            currentParaList.append(qMakePair(QObject::tr("OTA确认升级行为"),QObject::tr("确认升级")));
        }
        else if(data.split(": ").at(1) == "2")
        {
            currentParaList.append(qMakePair(QObject::tr("OTA升级行为"),QObject::tr("MCU请下载下一块数据")));
        }
        else if(data.split(": ").at(1) == "3")
        {
            currentParaList.append(qMakePair(QObject::tr("OTA确认升级行为"),QObject::tr("MCU上报更新中状态")));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTOTARD
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTOTARD(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("读取SOTA数据")));
    if(data.contains("AT+QIOTOTARD=?"))
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
    }
    else if(data.indexOf("AT+QIOTOTARD=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
        QStringList dataList = data.split("=");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        if(textValue.count() == 2)
        {
            currentParaList.append(qMakePair(QObject::tr("读取数据开始位置"),QString(textValue.at(0))));
            currentParaList.append(qMakePair(QObject::tr("读取长度"),QString(textValue.at(1))));
        }
    }
    else if(data.indexOf("AT+QIOTOTARD?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTOTARD:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置响应")));
        QStringList dataList = data.split(":");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        if(textValue.count() == 2)
        {
            currentParaList.append(qMakePair(QObject::tr("读取数据开始位置"),QString(textValue.at(0))));
            currentParaList.append(qMakePair(QObject::tr("实际返回的字节数"),QString(textValue.at(1))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTEVT
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTEVT(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("事件通知")));
    if(!data.contains(":") || !data.contains(","))
    {
        return;
    }
    QStringList dataList = data.split(":");
    if(dataList.count() < 2 )
    {
        return;
    }
    QStringList textValue = QString(dataList.at(1)).split(",");
    int event_type = QString(textValue.at(0)).toInt();
    int event_code = QString(textValue.at(1)).toInt();
    if(textValue.count() <= 2)
    {
        textValue.append(QObject::tr("当前为缓存模式"));
    }
    qDebug()<<"event_type:"<<event_type<<"event_code:"<<event_code;
    switch (event_type)
    {
    case 1:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("引导认证操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备认证成功")));
            break;
        case 10500:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备认证失败（系统发生未知异常）")));
            break;
        case 10404:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("平台内部接口调用失败")));
            break;
        /*   2.11.0 已删除
        case 10420:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("请求数据错误（连接失败）")));
            break;
       */
        case 10422:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备已认证（连接失败）")));
            break;
        case 10423:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("查询产品信息失败（连接失败）")));
            break;
        case 10424:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("PAYLOAD 解密失败（连接失败）")));
            break;
        case 10425:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("签名验证未通过（连接失败）")));
            break;
//        case 10426:
//            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("认证版本错误（连接失败）")));
//            break;
        case 10427:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("散列信息不合法（连接失败）")));
            break;
       /*   2.11.0 已删除
        case 10430:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("产品号发生改变（连接失败）")));
            break;
        */
        case 10431:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("DK不合法（连接失败）")));
            break;
        /*   2.11.0 已删除
        case 10432:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("产品号与认证版本不匹配（连接失败）")));
            break;
        */
        case 10433:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("Flag 不合法")));
            break;
        case 10434:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("ClientID与passwd中ClientID不匹配")));
            break;
        case 10450:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备内部错误（连接失败）")));
            break;
        /*
        case 10466:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("引导服务器地址未找到（连接失败）")));
            break;
        */
        case 10300:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("其他错误")));
            break;
        default:
            break;
        }
    }
        break;
    case 2:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("接入操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("注册成功")));
            break;
        case 10500:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("注册失败（系统发生未知异常）")));
            break;
        /*
        case 10420:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("请求数据错误（连接失败）")));
            break;
        */
        case 10404:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("平台内部接口调用错误")));
            break;
        case 10430:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备密钥不正确（连接失败）")));
            break;
        /*
        case 10431:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备被禁用（连接失败）")));
            break;
        */
        case 10433:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("Flag不符合规则；仅可为0000与0001")));
            break;
        case 10434:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("ClientID与password中的ClientID不匹配")));
            break;
        case 10437:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备秘钥错误")));
            break;
        case 10450:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备内部错误（连接失败）")));
            break;
        case 10471:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("实现方案版本不支持(连接失败)")));
            break;
        case 10473:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("接入心跳异常（连接超时）")));
            break;
        case 10474:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("网络异常（连接超时）")));
            break;
        case 10475:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("服务器发生改变")));
            break;
        case 10476:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("连接AP异常")));
            break;
        case 10477:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备被禁用（连接失败）")));
            break;
        case 10300:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("其他错误")));
            break;
        default:
            break;
        }
    }
        break;
    case 3:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("订阅操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("订阅成功")));
            break;
        case 10300:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("订阅失败")));
            break;
        default:
            break;
        }
    }
        break;
    case 4:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("发送数据操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("透传数据发送成功")));
            break;
        case 10210:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("物模型数据发送成功")));
            break;
        case 10220:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("定位数据发送成功")));
            break;
        case 10230:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备状态数据发送成功")));
            break;
        case 10240:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备信息发送成功")));
            break;
        case 10300:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("透传数据发送失败")));
            break;
        case 10310:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("物模型数据发送失败")));
            break;
        case 10320:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("定位数据发送失败")));
            break;
        case 10330:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备状态数据发送失败")));
            break;
        case 10340:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备信息发送失败")));
            break;
        default:
            break;
        }
    }
        break;
    case 5:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("接收数据操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到透传数据")));
            currentParaList.append(qMakePair(QObject::tr("接收数据长度"),QString(textValue.at(2))));
            break;
        case 10210:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到物模型下发数据")));
            currentParaList.append(qMakePair(QObject::tr("接收数据长度"),QString(textValue.at(2))));
            break;
        case 10211:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到物模型查询命令")));
            currentParaList.append(qMakePair(QObject::tr("请求包ID"),QString(textValue.at(2))));
            for(int i=3;i<textValue.count()-3;i++)
            {
                currentParaList.append(qMakePair(QObject::tr("物模型ID"),QString(textValue.at(i))));
            }
            break;
        case 10220:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到子设备状态请求数据")));
            /* 时间短暂，暂不处理
            currentParaList.append(qMakePair(QObject::tr("请求包ID"),QString(textValue.at(2))));
            for(int i=3;i<textValue.count()-3;i++)
            {
                currentParaList.append(qMakePair(QObject::tr("物模型ID"),QString(textValue.at(i))));
            }
            */
            break;
        case 10230:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到子设备信息请求数据")));
            /* 时间短暂，暂不处理
            currentParaList.append(qMakePair(QObject::tr("请求包ID"),QString(textValue.at(2))));
            for(int i=3;i<textValue.count()-3;i++)
            {
                currentParaList.append(qMakePair(QObject::tr("物模型ID"),QString(textValue.at(i))));
            }
            */
            break;
        case 10473:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到数据但长度超过模组buffer限制")));
            break;
        case 10428:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备接收缓存过多导致限流")));
            break;
        default:
            break;
        }
    }
        break;
    case 6:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("设备登出事件")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("注销成功（断开连接成功）")));
            break;
        default:
            break;
        }
    }
        break;
    case 7:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("FOTA操作")));
        switch (event_code)
        {
        case 10700:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("有升级任务")));
            if(textValue.count() >= 8)
            {
                currentParaList.append(qMakePair(QObject::tr("标识符"),QString(textValue.at(2)).replace("\"","")));
                currentParaList.append(qMakePair(QObject::tr("源版本"),QString(textValue.at(3)).replace("\"","")));
                currentParaList.append(qMakePair(QObject::tr("目标版本"),QString(textValue.at(4)).replace("\"","")));
                currentParaList.append(qMakePair(QObject::tr("最小电量值"),QString(textValue.at(5))));
                currentParaList.append(qMakePair(QObject::tr("最低信号值"),QString(textValue.at(6))));
                currentParaList.append(qMakePair(QObject::tr("需要的内存大小"),QString(textValue.at(7))));
            }
            break;
        case 10701:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("模组开始下载")));
            if(textValue.count() >= 5)
            {
                currentParaList.append(qMakePair(QObject::tr("标识符"),QString(textValue.at(2)).replace("\"","")));
                currentParaList.append(qMakePair(QObject::tr("数据长度"),QString(textValue.at(3))));
                currentParaList.append(qMakePair(QObject::tr("MD5校验值"),QString(textValue.at(4)).replace("\"","")));
            }
            break;
        case 10702:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("包下载中")));
            break;
        case 10703:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("包下载完成")));
            if(textValue.count() >= 6)
            {
                currentParaList.append(qMakePair(QObject::tr("标识符"),QString(textValue.at(2)).replace("\"","")));
                currentParaList.append(qMakePair(QObject::tr("数据长度"),QString(textValue.at(3))));
                currentParaList.append(qMakePair(QObject::tr("起始地址"),QString(textValue.at(4))));
                currentParaList.append(qMakePair(QObject::tr("当前块大小"),QString(textValue.at(5))));
            }
            break;
        case 10704:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("包更新中")));
            break;
        case 10705:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("更新固件完成")));
            break;
        case 10706:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("更新固件失败")));
            break;
        case 10707:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("升级操作通知")));

            break;
        default:
            break;
        }
    }
        break;
    case 8:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("平台事件")));
        switch (event_code)
        {
        case 10428:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备高频消息导致限流")));
            break;
        case 10429:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("超过单设备数量或者每日请求数导致限流")));
            break;
        default:
            break;
        }
    }
    break;
    case 10:
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("子设备注销事件")));
        /* 时间短暂，暂不处理 */
        break;
    default:
        break;
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTMODELTD
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTMODELTD(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("发送物模型数据")));
    if(data.contains("AT+QIOTMODELTD=?"))
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
    }
    else if(data.indexOf("AT+QIOTMODELTD=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
        QStringList dataList = data.split("=");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        if(textValue.count() > 0)
        {
            currentParaList.append(qMakePair(QObject::tr("发送模式"),QString(textValue.at(0))));
        }
        if(textValue.count() > 1)
        {
            currentParaList.append(qMakePair(QObject::tr("发送数据长度"),QString(textValue.at(1))));
        }
        if(textValue.count() == 3)
        {
            currentParaList.append(qMakePair(QObject::tr("请求包ID"),QString(textValue.at(2))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTMODELRD
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTMODELRD(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("读取物模型数据")));
    if(data.contains("AT+QIOTMODELRD=?"))
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取AT指令的使用方式/范围")));
    }
    else if(data.indexOf("AT+QIOTMODELRD=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
        currentParaList.append(qMakePair(QObject::tr("读取字节数"),QString(data.split("=").at(1))));
    }
    else if(data.indexOf("AT+QIOTMODELRD?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTMODELRD:") >= 0)
    {
        QStringList textValue;
        if(textValue.contains(","))
        {
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
           textValue = QString(dataList.at(1)).split(",");
           currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置响应")));
           if(textValue.count() == 3)
           {
                currentParaList.append(qMakePair(QObject::tr("返回字节数"),QString(textValue.at(0))));
                currentParaList.append(qMakePair(QObject::tr("当前数据包剩余字节数"),QString(textValue.at(1))));
                currentParaList.append(qMakePair(QObject::tr("剩余数据包个数"),QString(textValue.at(2))));
           }
        }
        else if(textValue.contains("-") && textValue.contains("("))
        {
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            QString payload = QString(dataList.at(1));
            payload.replace("(","");
            payload.replace(")","");
            textValue = payload.split("-");
            if(textValue.count() == 2)
            {
                 currentParaList.append(qMakePair(QObject::tr("最小长度"),QString(textValue.at(0))));
                 currentParaList.append(qMakePair(QObject::tr("最大长度"),QString(textValue.at(1))));
            }
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTLOCIN
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTLOCIN(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("配置内部定位信息报告类型")));
    if(data.indexOf("AT+QIOTLOCIN=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTLOCIN=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("获取")));
        currentParaList.append(qMakePair(QObject::tr("内部定位类型"),QString(data.split("=").at(1))));
    }
    else if(data.indexOf("AT+QIOTLOCIN?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("AT+QIOTLOCIN:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("执行")));
        currentParaList.append(qMakePair(QObject::tr("内部定位类型"),QString(data.split(": ").at(1))));
    }
    else if(data.indexOf("+QIOTLOCIN:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
        currentParaList.append(qMakePair(QObject::tr("内部定位数据"),QString(data.split(": ").at(1))));
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTLOCEXT
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTLOCEXT(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("获取外部定位信息")));
    if(data.indexOf("AT+QIOTLOCEXT=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTLOCEXT=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("上报")));
        currentParaList.append(qMakePair(QObject::tr("外部定位类型"),QString(data.split("=").at(1))));
    }
    else if(data.indexOf("AT+QIOTLOCEXT?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("AT+QIOTLOCEXT:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("执行")));
        currentParaList.append(qMakePair(QObject::tr("外部定位类型"),QString(data.split(": ").at(1))));
    }
    else if(data.indexOf("+QIOTLOCEXT:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
        currentParaList.append(qMakePair(QObject::tr("外部定位类型"),QString(data.split(": ").at(1))));
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTOTAREQ
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTOTAREQ(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("OTA请求")));
    if(data.indexOf("AT+QIOTOTAREQ=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTOTAREQ=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("请求")));
        currentParaList.append(qMakePair(QObject::tr("MCU标识"),QString(data.split("=").at(1))));
    }
    else if(data.indexOf("AT+QIOTOTAREQ?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTOTAREQ:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
        QStringList textValue = QString(data.split(":").at(1)).split(",");
        if(QString(textValue.at(0)).toInt() == 0)
        {
            currentParaList.append(qMakePair(QObject::tr("额外信息"),QObject::tr("无")));
        }
        else if(QString(textValue.at(0)).toInt() == 1)
        {
            currentParaList.append(qMakePair(QObject::tr("额外信息"),QObject::tr("sha256")));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QFOTAUP
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QFOTAUP(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("HTTP-OTA请求")));
    if(data.indexOf("AT+QFOTAUP=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QFOTAUP=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("请求")));
        QStringList dataList = data.split("=");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        if(textValue.count() < 2 )
        {
            return;
        }
        currentParaList.append(qMakePair(QObject::tr("电量"),QString(textValue.at(0))));
        currentParaList.append(qMakePair(QObject::tr("连接模式"),QString(textValue.at(1))));
        if(textValue.count() == 3)
        {
            currentParaList.append(qMakePair(QObject::tr("服务器地址"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
        }
    }
    else if(data.indexOf("AT+QIOTOTARE?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("+QIOTOTARE:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
        QStringList dataList = data.split(":");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        currentParaList.append(qMakePair(QObject::tr("电量"),QString(textValue.at(0))));
        currentParaList.append(qMakePair(QObject::tr("连接模式"),QString(textValue.at(1))));
        if(textValue.count() == 3)
        {
            currentParaList.append(qMakePair(QObject::tr("服务器地址"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QFOTACFG
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QFOTACFG(QString data)
{
    int offset = 0;
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("HTTP-OTA信息设置")));
    if(data.indexOf("AT+QFOTACFG=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QFOTACFG?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("PK") >= 0)
    {
        if(data.indexOf("AT+QFOTACFG=") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("请求")));
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(",");
            currentParaList.append(qMakePair(QObject::tr("产品号"),QString(textValue.at(0))));
            currentParaList.append(qMakePair(QObject::tr("产品密钥"),QString(textValue.at(1))));
        }
        else if((offset = data.indexOf("+QFOTACFG:")) >= 0)
        {
            QString dataFork = data.mid(offset+strlen("+QFOTACFG:"));
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
            QStringList textValue = dataFork.split(",");
            currentParaList.append(qMakePair(QObject::tr("产品信息"),QString(removeDoubleQuotationMarks(textValue.at(0)))));
            if(textValue.count() >= 2)
            {
                currentParaList.append(qMakePair(QObject::tr("产品密钥"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
            }
            if(textValue.count() >= 3)
            {
                currentParaList.append(qMakePair(QObject::tr("产品号"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            }
        }
    }
    else if(data.indexOf("server")>= 0)
    {
        if(data.indexOf("AT+QFOTACFG=") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("请求")));
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            QStringList textValue = QString(dataList.at(1)).split(":");
            currentParaList.append(qMakePair(QObject::tr("服务器ip"),QString(removeDoubleQuotationMarks(textValue.at(0)))));
            currentParaList.append(qMakePair(QObject::tr("服务器端口号"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
        }
        else if((offset = data.indexOf("+QFOTACFG:")) >= 0)
        {
            QString dataFork = data.mid(offset+strlen("+QFOTACFG:"));
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
            QStringList textValue = dataFork.split(",");
            currentParaList.append(qMakePair(QObject::tr("服务器信息"),textValue.at(0)));
            QStringList textValueFork = dataFork.mid(QString(textValue.at(0)).length()).split(":");
            if(textValueFork.count() > 0)
                currentParaList.append(qMakePair(QObject::tr("服务器地址"),QString(removeDoubleQuotationMarks(textValueFork.at(0)))));
            if(textValueFork.count() > 1)
                currentParaList.append(qMakePair(QObject::tr("服务器端口号"),QString(removeDoubleQuotationMarks(textValueFork.at(1)))));
        }
    }
    else if(data.indexOf("tls")>= 0)
    {
        if(data.indexOf("AT+QFOTACFG=") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("请求")));
            currentParaList.append(qMakePair(QObject::tr("加密模式"),QString(data.split("=").at(1))));
        }
        else if(data.indexOf("+QFOTACFG:") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
            currentParaList.append(qMakePair(QObject::tr("加密模式"),QString(data.split(":").at(1))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTSUBCONN
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSUBCONN(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("子设备连接")));
    if(data.indexOf("AT+QIOTSUBCONN=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTSUBCONN?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("AT+QIOTSUBCONN=") >= 0 || data.indexOf("+QIOTSUBCONN") >= 0)
    {

        QStringList textValue;
        if(data.indexOf("AT+QIOTSUBCONN=") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
        }
        else
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
        }
        if(textValue.count() >= 5)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备交互数据加密方式"),QString(textValue.at(0))));
            currentParaList.append(qMakePair(QObject::tr("子设备心跳时间"),QString(textValue.at(1))));
            currentParaList.append(qMakePair(QObject::tr("子设备产品名"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            currentParaList.append(qMakePair(QObject::tr("子设备产品秘钥"),QString(removeDoubleQuotationMarks(textValue.at(3)))));
            currentParaList.append(qMakePair(QObject::tr("子设备设备名"),QString(removeDoubleQuotationMarks(textValue.at(4)))));
        }
        if(textValue.count() == 6)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备设备秘钥"),QString(removeDoubleQuotationMarks(textValue.at(5)))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTSUBDISCONN
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSUBDISCONN(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("断开/注销子设备与网关的连接")));
    if(data.indexOf("AT+QIOTSUBDISCONN=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTSUBDISCONN?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("AT+QIOTSUBDISCONN=") >= 0 || data.indexOf("+QIOTSUBDISCONN：") >= 0)
    {


        QStringList textValue;
        if(data.indexOf("AT+QIOTSUBDISCONN=") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
        }
        else
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
        }
        if(textValue.count() >= 2)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备产品名"),QString(removeDoubleQuotationMarks(textValue.at(0)))));
            currentParaList.append(qMakePair(QObject::tr("子设备设备名"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
        }
        if(textValue.count() >= 4)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备产品秘钥"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            currentParaList.append(qMakePair(QObject::tr("子设备设备秘钥"),QString(removeDoubleQuotationMarks(textValue.at(3)))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析QIOTSUBSEND
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSUBSEND(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("发送子设备透传数据至平台")));
    if(data.indexOf("AT+QIOTSUBSEND=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTSUBSEND?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("AT+QIOTSUBSEND=") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
        QStringList dataList = data.split("=");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        if(textValue.count() == 4)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备产品名"),QString(removeDoubleQuotationMarks(textValue.at(0)))));
            currentParaList.append(qMakePair(QObject::tr("子设备设备名"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
            currentParaList.append(qMakePair(QObject::tr("数据长度"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            currentParaList.append(qMakePair(QObject::tr("数据"),QString(removeDoubleQuotationMarks(textValue.at(3)))));
        }
    }
    else if(data.indexOf("+QIOTSUBSEND:") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
        QStringList dataList = data.split("=");
        if(dataList.count() < 2 )
        {
            return;
        }
        QStringList textValue = QString(dataList.at(1)).split(",");
        if(textValue.count() >= 3)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备产品名"),QString(removeDoubleQuotationMarks(textValue.at(0)))));
            currentParaList.append(qMakePair(QObject::tr("子设备设备名"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
            currentParaList.append(qMakePair(QObject::tr("数据长度"),QString(removeDoubleQuotationMarks(textValue.at(2)))));

        }
        if(textValue.count() == 4)
        {
            currentParaList.append(qMakePair(QObject::tr("数据"),QString(removeDoubleQuotationMarks(textValue.at(3)))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析 QIOTSUBRD
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSUBRD(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("读取平台下发的透传数据")));
    if(data.indexOf("AT+QIOTSUBRD=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTSUBRD?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("AT+QIOTSUBRD=") >= 0 || data.indexOf("+QIOTSUBRD:") >= 0)
    {
        QStringList textValue;
        if(data.indexOf("AT+QIOTSUBRD=") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
        }
        else
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
        }
        if(textValue.count() >= 2)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备产品名"),QString(removeDoubleQuotationMarks(textValue.at(0)))));
            currentParaList.append(qMakePair(QObject::tr("子设备设备名"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
        }
        if(textValue.count() == 3)
        {
            currentParaList.append(qMakePair(QObject::tr("读取的数据长度"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
        }
        else if(textValue.count() == 5)
        {
            currentParaList.append(qMakePair(QObject::tr("实际读取的数据长度"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            currentParaList.append(qMakePair(QObject::tr("当前数据包剩余未读的数据长度"),QString(removeDoubleQuotationMarks(textValue.at(3)))));
            currentParaList.append(qMakePair(QObject::tr("剩余数据包个数"),QString(removeDoubleQuotationMarks(textValue.at(4)))));
        }

    }
}
/**************************************************************************
** 功能	@brief : 解析 QIOTSUBTSLTD
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSUBTSLTD(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("发送子设备物模型数据至平台")));
    if(data.indexOf("AT+QIOTSUBTSLTD=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTSUBTSLTD?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("AT+QIOTSUBTSLTD=") >= 0 || data.indexOf("+QIOTSUBTSLTD:") >= 0)
    {
        QStringList textValue;
        if(data.indexOf("AT+QIOTSUBTSLTD=") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
        }
        else
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
        }
        if(textValue.count() >= 3)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备产品名"),QString(removeDoubleQuotationMarks(textValue.at(0)))));
            currentParaList.append(qMakePair(QObject::tr("子设备设备名"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
            currentParaList.append(qMakePair(QObject::tr("待发送数据长度"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
        }
        if(textValue.count() == 4)
        {
            currentParaList.append(qMakePair(QObject::tr("请求包 ID"),QString(removeDoubleQuotationMarks(textValue.at(3)))));
        }

    }
}
/**************************************************************************
** 功能	@brief : 解析 QIOTSUBTSLRD
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSUBTSLRD(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("发送子设备物模型数据至平台")));
    if(data.indexOf("AT+QIOTSUBTSLRD=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTSUBTSLRD?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("AT+QIOTSUBTSLRD=") >= 0 || data.indexOf("+QIOTSUBTSLRD:") >= 0)
    {
        QStringList textValue;
        bool mode = true;
        if(data.indexOf("AT+QIOTSUBTSLRD=") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
            textValue = QString(data.split("=").at(1)).split(",");
        }
        else
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
            textValue = QString(data.split(":").at(1)).split(",");
            mode = false;
        }
        if(textValue.count() >= 2)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备产品名"),QString(removeDoubleQuotationMarks(textValue.at(0)))));
            currentParaList.append(qMakePair(QObject::tr("子设备设备名"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
        }
        if(textValue.count() == 3)
        {
            if(mode)
            {
                currentParaList.append(qMakePair(QObject::tr("读取的数据长度"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            }
            else
            {
                currentParaList.append(qMakePair(QObject::tr("读取的数据长度/剩余数据包个数"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            }
        }
        else if(textValue.count() == 5)
        {
            currentParaList.append(qMakePair(QObject::tr("实际读取的数据长度"),QString(removeDoubleQuotationMarks(textValue.at(2)))));
            currentParaList.append(qMakePair(QObject::tr("当前数据包剩余未读的数据长度"),QString(removeDoubleQuotationMarks(textValue.at(3)))));
            currentParaList.append(qMakePair(QObject::tr("剩余数据包个数"),QString(removeDoubleQuotationMarks(textValue.at(4)))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析 QIOTSUBINFO
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSUBINFO(QString data)
{

}
/**************************************************************************
** 功能	@brief : 解析 QIOTSUBHEBT
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSUBHBT(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("子设备心跳周期")));
    if(data.indexOf("AT+QIOTSUBHEBT=?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("测试")));
    }
    else if(data.indexOf("AT+QIOTSUBHEBT?") >= 0)
    {
        currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("查询")));
    }
    else if(data.indexOf("AT+QIOTSUBHEBT=") >= 0 || data.indexOf("+QIOTSUBHEBT:") >= 0)
    {
        QStringList textValue;
        bool mode = true;
        if(data.indexOf("AT+QIOTSUBHEBT=") >= 0)
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("设置")));
            QStringList dataList = data.split("=");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
        }
        else
        {
            currentParaList.append(qMakePair(QObject::tr("命令类型"),QObject::tr("响应")));
            QStringList dataList = data.split(":");
            if(dataList.count() < 2 )
            {
                return;
            }
            textValue = QString(dataList.at(1)).split(",");
            mode = false;
        }
        if(textValue.count() == 2)
        {
            currentParaList.append(qMakePair(QObject::tr("子设备产品名"),QString(removeDoubleQuotationMarks(textValue.at(0)))));
            currentParaList.append(qMakePair(QObject::tr("子设备设备名"),QString(removeDoubleQuotationMarks(textValue.at(1)))));
        }
    }
}
/**************************************************************************
** 功能	@brief : 解析 QIOTSUBEVT
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void atcmdAnalysis::analysis_QIOTSUBEVT(QString data)
{
    currentParaList.append(qMakePair(QObject::tr("命令描述"),QObject::tr("子设备事件通知")));
    if(!data.contains(":") || !data.contains(","))
    {
        return;
    }
    QStringList dataList = data.split(":");
    if(dataList.count() < 2 )
    {
        return;
    }
    QStringList textValue = QString(dataList.at(1)).split(",");
    if(textValue.count() < 4)
    {
        return;
    }
    currentParaList.append(qMakePair(QObject::tr("设备名称"),QString(textValue.at(0))));
    currentParaList.append(qMakePair(QObject::tr("设备秘钥"),QString(textValue.at(1))));
    int event_type = QString(textValue.at(2)).toInt();
    int event_code = QString(textValue.at(3)).toInt();
    qDebug()<<"event_type:"<<event_type<<"event_code:"<<event_code;
    switch (event_type)
    {
    case 1:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("认证操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备认证成功")));
            break;
        case 10404:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("平台内部接口调用错误")));
            break;
        case 10422:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备已认证（连接失败）")));
            break;
        case 10423:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("查询产品信息失败（连接失败）")));
            break;
        case 10425:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("签名验证未通过（连接失败）")));
            break;
        case 10427:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("散列信息不合法（连接失败）")));
            break;
        case 10431:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("DK不合法（连接失败）")));
            break;
        case 10500:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备认证失败（系统发生未知异常）")));
            break;
        case 10440:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("网关与子设备无关联关系")));
            break;
        default:
            break;
        }
    }
        break;
    case 2:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("登录操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("登录成功")));
            break;
        case 10404:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("平台内部接口调用错误")));
            break;
        case 10430:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备密钥不正确（连接失败）")));
            break;
        case 10431:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备被禁用（连接失败）")));
            break;
        case 10433:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("Flag不合法")));
            break;
        case 10434:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("ClientID与password不匹配（password中包涵ClientID相关信息）")));
            break;
        case 10437:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("无当前设备信息")));
            break;
        case 10438:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("网络异常（连接超时）")));
            break;
        case 10500:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("登录失败（系统发生未知异常）")));
            break;
        case 10440:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("网关与子设备无关联关系")));
            break;
        case 10441:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备已连接（连接成功）")));
            break;
        default:
            break;
        }
    }
        break;
    case 4:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("发送数据操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备透传数据发送成功")));
            break;
        case 10210:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备物模型数据发送成功")));
            break;
        case 10300:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备透传数据发送失败")));
            break;
        case 10310:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备物模型数据发送失败")));
            break;
        default:
            break;
        }
    }
        break;
    case 5:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("接收数据操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到透传数据")));
            currentParaList.append(qMakePair(QObject::tr("接收数据长度"),QString(textValue.at(2))));
            break;
        case 10210:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到物模型下发数据")));
            currentParaList.append(qMakePair(QObject::tr("接收数据长度"),QString(textValue.at(2))));
            break;
        case 10211:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到物模型查询命令")));
            currentParaList.append(qMakePair(QObject::tr("请求包ID"),QString(textValue.at(2))));
            for(int i=3;i<textValue.count()-3;i++)
            {
                currentParaList.append(qMakePair(QObject::tr("物模型ID"),QString(textValue.at(i))));
            }
            break;
        case 10473:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("收到数据但长度超过模组buffer限制")));
            break;
        case 10428:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备接收缓存过多导致限流")));
            break;
        default:
            break;
        }
    }
        break;
    case 6:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("登出操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("登出成功")));
            break;
        default:
            break;
        }
    }
        break;
    case 8:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("平台事件")));
        switch (event_code)
        {
        case 10428:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("设备高频消息导致限流")));
            break;
        case 10429:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("超过单设备数量或者每日请求数导致限流")));
            break;
        case 10442:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("子设备没有登录")));
            break;
        default:
            break;
        }
    }
        break;
    case 10:
    {
        currentParaList.append(qMakePair(QObject::tr("事件标识符"),QObject::tr("注销操作")));
        switch (event_code)
        {
        case 10200:
            currentParaList.append(qMakePair(QObject::tr("事件返回码"),QObject::tr("注销成功")));
            break;
        default:
            break;
        }
    }
        break;
    default:
        break;
    }
}

/**************************************************************************
** 功能	@brief : 对串口数据进行解析
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QList<QPair<QString,QString>> atcmdAnalysis::atcmdTextAnalysis(QTextEdit *textEdit)
{
    qInfo()<<__FUNCTION__;
    QList<QPair<QString,QString>> paraList;
    int row = textEdit->textCursor().blockNumber();
    QString str = textEdit->document()->findBlockByLineNumber(row).text();
    str.append('\0');
    qInfo()<<"str:"<<str;
    if(str.isEmpty())
        return paraList;
    int offset = 0;
    if((offset = str.indexOf("[TX]")) >= 0)
    {
        paraList.append(qMakePair(QObject::tr("数据来源"),QObject::tr("发送")));
        str = str.mid(offset-1);
    }
    else if((offset = str.indexOf("[RX]")) >= 0)
    {
        paraList.append(qMakePair(QObject::tr("数据来源"),QObject::tr("接收")));
        str = str.mid(offset);
    }
    else
    {
        QTextCursor cursor = textEdit->textCursor();
        QTextCursor txCursor = textEdit->document()->find("TX",cursor,(QTextDocument::FindBackward|QTextDocument::FindWholeWords));
        QTextCursor rxCursor = textEdit->document()->find("RX",cursor,(QTextDocument::FindBackward|QTextDocument::FindWholeWords));
        if(txCursor.blockNumber() > rxCursor.blockNumber())
        {
            paraList.append(qMakePair(QObject::tr("数据来源"),QObject::tr("发送")));
        }
        else
        {
            paraList.append(qMakePair(QObject::tr("数据来源"),QObject::tr("接收")));
        }
    }
    if(str.contains("="))
    {
        paraList.append(qMakePair(QObject::tr("命令内容"),QString(str.mid(str.indexOf("=")+1))));
    }
    else if(str.contains(":"))
    {
        paraList.append(qMakePair(QObject::tr("命令内容"),QString(str.mid(str.indexOf(":")+1))));
    }
    else if(str.contains(">"))
    {
        paraList.append(qMakePair(QObject::tr("命令内容"),QString(str.mid(str.indexOf(">")+1))));
    }
    currentParaList = paraList;
    if(str.contains(QChar(0x00)))
    {
        str.replace(QChar(0x00),"");
    }
    handle_atcmdMatchList(str);
    return currentParaList;
}
/**************************************************************************
** 功能	@brief : 删除"
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QString atcmdAnalysis::removeDoubleQuotationMarks(QString data)
{
    QString newData = data;
    newData.replace(QRegExp("\""), "");
    return newData;
}
