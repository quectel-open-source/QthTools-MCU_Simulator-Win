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
#include "mapList.h"
#include <QDebug>
mapList::mapList()
{
//    g_obj = new QJsonObject();
//    g_obj = new QJsonObject();
}
// 建立表
//bool jsonList::buildJsonTable()
//{
//    return true;
//}



//// 增加
//bool jsonList::addJsonToTable(QJsonObject data,QString key,bool level)
//{
//    if(g_obj->count() == 0 || !g_obj->contains(key))
//    {
//        if(level == false)
//        {
//            for(QJsonObject::iterator it = data.begin();it != data.end();it++)
//            {
//                g_obj->insert(it.key(),it.value());
//            }
//        }
//        else
//        {
//            g_obj->insert(key,data);
//        }
//    }
//    else if(g_obj->count() != 0 && g_obj->contains(key))
//    {
//        g_obj->value(key) = data.value(key);
//    }
//    else
//        return false;



//    return true;
//}
//// 删除
//bool jsonList::deleteJsonFromTable(QJsonObject data, int level)
//{
//    if(g_obj->count() > 0 &&g_obj->contains(key))
//    {
//        if(level == false)
//        {
//            for(QJsonObject::iterator it = data.begin();it != data.end();it++)
//            {
//                g_obj->take(it.key());
//            }
//        }
//        else
//        {
//            g_obj->take(key);
//        }
//    }
//    else
//        return false;
//    return true;
//}
//// 查找表 ，可循环嵌套
//bool jsonList::findValueJsonFromTable(QJsonObject objHead ,QString key, QJsonValue *data,int level)
//{
//    //    QJsonObject currentObj;
//    //    for(QJsonObject::iterator it = objHead.begin();it != objHead.end();it++)
//    //    {
//    //        if((*it).isObject() && level != 0)
//    //        {
//    //            currentObj = (*it).toObject();
//    //            level--;
//    //            findValueJsonFromTable(currentObj,key,data,level);
//    //        }
//    //        else if((*it).isObject() && level == 0)
//    //        {
//    //            continue;
//    //        }
//    //        else
//    //        {
//    //             if(! it.key() == key)
//    //             {
//    //                 if(data->isNull())
//    //                 {
//    //                    *data = it.value();
//    //                 }
//    //                 else
//    //                     (*it)[key] = *data;
//    //                 return false;
//    //             }
//    //             else
//    //                 break;
//    //        }
//    //    }
//    //    return false;
//}
// 查找表 ，可循环嵌套


//// 映射
//bool jsonList::findJsonMapFromTable(QString key, QString *data)
//{
//    return true;
//}
//// 修改
//bool jsonList::modifyJsonFromTable(QJsonObject data)
//{
//    return true;
//}
//// 保存记录
//bool jsonList::saveJsonData()
//{
//    return true;
//}

bool mapList::addMapToTable(info_type data)
{
    if(data.name.isEmpty())
        return false;
    for(QList<info_type>::iterator it = infoList.begin();it != infoList.end();it++)
    {
        if(0 == (*it).name.compare(data.name))
        {
            return false;
        }
    }
    infoList.append(data);
    return true;
}

bool mapList::deleteMapFromTable(info_type data)
{
    if(!data.name.isEmpty())
    {
        for(int i=0;i<infoList.count();i++)
        {
            if(0 == infoList[i].name.compare(data.name))
            {
                infoList.removeAt(i);
                return true;
            }
        }
        return false;
    }
    return false;
}

bool mapList::findValueMapFromTable(info_type *data)
{
    if(data->name.isEmpty())
        return false;
    else if(data->input.isEmpty() && !data->output.isEmpty())
    {
        for(QList<info_type>::iterator it = infoList.begin();it != infoList.end();it++)
        {
            if(0 == (*it).name.compare(data->name))
            {
                data->input.swap((*it).input);
                return true;
            }
        }
    }
    else if(!data->input.isEmpty() && data->output.isEmpty())
    {
        for(QList<info_type>::iterator it = infoList.begin();it != infoList.end();it++)
        {
            if(0 == (*it).name.compare(data->name))
            {
                data->output.swap((*it).output);
                return true;
            }
        }
    }
    else if(data->input.isEmpty() && data->output.isEmpty())
    {
        for(QList<info_type>::iterator it = infoList.begin();it != infoList.end();it++)
        {
            if(0 == (*it).name.compare(data->name))
            {
                data->input.swap((*it).input);
                data->output.swap((*it).output);
                return true;
            }
        }
    }
    return false;
}

bool mapList::modifyMapFromTable(info_type data)
{
    if(data.name.isEmpty())
        return false;
    else
    {
        if(data.input.isEmpty() && data.output.isEmpty())
            return false;
        for(QList<info_type>::iterator it = infoList.begin();it != infoList.end();it++)
        {
            if(0 == (*it).name.compare(data.name))
            {
                if(!data.input.isEmpty())
                {
                   for(QMap<QString,QVariant>::iterator it2 = data.input.begin();it2 != data.input.end();it2++)
                   {
                       if(!(*it).input.value(it2.key()).isNull())
                       {
                           (*it).input.insert(it2.key(),it2.value());
                       }
                   }
                }
                if(!data.output.isEmpty())
                {
                    for(QMap<QString,QVariant>::iterator it2 = data.input.begin();it2 != data.input.end();it2++)
                    {
                        if(!(*it).input.value(it2.key()).isNull())
                        {
                            (*it).input.insert(it2.key(),it2.value());
                        }
                    }
                }
                return true;
            }
        }
    }
    return true;
}


//QVariant jsonValueToVariant(QString typeString)
//{
//    if(0 == typeString.compare("BOOL"))
//    {
//        return QVariant(false);
//    }
//    else if(0 == typeString.compare("INT"))
//    {
//        return QVariant(0);
//    }
//    else if(0 == typeString.compare("FLOAT") || 0 == typeString.compare("DOUBLE"))
//    {
//        return QVariant(0.0);
//    }
//    else if
//}
