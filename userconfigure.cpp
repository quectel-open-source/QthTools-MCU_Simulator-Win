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
#include "userconfigure.h"
/**************************************************************************
** 功能	@brief : 用户配置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
userConfigure::userConfigure()
{
    userInfo.insert(SOCKETSTATUS,false);
    userInfo.insert(MQTTSTATUS,false);
    userInfo.insert(CACHEMODE,false);
    userInfo.insert(SENDMMODELMODE,TTLVMODESNED);
}
/**************************************************************************
** 功能	@brief :  获取全局值
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool userConfigure::getValue(int key,bool &error)
{
    bool value = false;
    switch(key)
    {
        case SOCKETSTATUS:
            value = userInfo[SOCKETSTATUS];
            break;
        case MQTTSTATUS:
            value = userInfo[MQTTSTATUS];
            break;
        case CACHEMODE:
            value = userInfo[CACHEMODE];
            break;
        case SENDMMODELMODE:
            value = userInfo[SENDMMODELMODE];
            break;
        default:
            error = true;
            break;
    }
    return value;
}
/**************************************************************************
** 功能	@brief :  设置全局值
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool userConfigure::setValue(int key, bool value,bool &error)
{
    switch(key)
    {
        case SOCKETSTATUS:
            userInfo[SOCKETSTATUS] = value;
            break;
        case MQTTSTATUS:
            userInfo[MQTTSTATUS] = value;
            break;
        case CACHEMODE:
            userInfo[CACHEMODE] = value;
            break;
        case SENDMMODELMODE:
            userInfo[SENDMMODELMODE] = value;
            break;
        default:
            break;
    }
    return error;
}
