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
#include "modelprocess.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDoubleSpinBox>
#include <QDateTimeEdit>
#include <QHeaderView>
#include <QtCore/qmath.h>
#include <QDateTime>
#include <QBrush>
#include <QObject>

#include "baseQTConfigure.h"
#include "kernel_config.h"
#include "third/third_config.h"
#include "userconfigure.h"
#include "kernel/mmessagebox.h"
#include "subwindow/treelistwindows.h"

QJsonArray g_properties;

modelProcess::modelProcess()
{
    serverMapList = new mapList();
}

/**************************************************************************
** 功能	@brief : 创建物模型列表到qtablewidget
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int modelProcess::modelCreate(int row, int column,QJsonObject modelObj,QTableWidget * tableWidget,bool writeEnable)
{
    if(modelObj.value("dataType").toString() == "BOOL")
    {
        SwitchButton *switchButton_model = new SwitchButton();
        switchButton_model->resize(100,50);
        switchButton_model->move(350,10);
        switchButton_model->setEnable(writeEnable);
        QJsonArray sonModelArray = modelObj.value("specs").toArray();
        int modelRowCount = sonModelArray.size();
        QStringList nameList;
        for(int model_i=0;model_i<modelRowCount;model_i++)
        {
            QJsonObject Obj = sonModelArray.at(model_i).toObject();
            if(Obj.value("value").toString().contains("true") && nameList.isEmpty())
            {
                nameList.append(Obj.value("name").toString());
            }
            else
            {
                nameList.insert(0,Obj.value("name").toString());
            }
        }
        if(nameList.size() == 2)
        {
            switchButton_model->setTextColor(QColor(Qt::blue));
//            switchButton_model->setText(nameList.at(1),nameList.at(0));
        }
        tableWidget->setRowHeight(row, 40);
        tableWidget->setCellWidget(row,column,switchButton_model);
    }
    else if(modelObj.value("dataType").toString() == "INT" || modelObj.value("dataType").toString() == "FLOAT" || modelObj.value("dataType").toString() == "DOUBLE")
    {
        QDoubleSpinBox *QDoubleSpinBox_model = new QDoubleSpinBox(tableWidget);
        QDoubleSpinBox_model->setStyleSheet("QDoubleSpinBox{\
                                            color: rgb(104, 112, 130);\
                                            font-size:14px;\
                                            border-style:solid;\
                                            border-width:1px;\
                                            border-color:rgb(229, 231, 234);\
                                            border-radius:4px;\
                                            padding-left:5px;\
                                            }\
                                            QDoubleSpinBox::up-button\
                                            {\
                                            image:url(:/png/up.png);\
                                            width: 30px;\
                                            }\
                                            QDoubleSpinBox::down-button{\
                                            image:url(:/png/down.png);\
                                            width: 30px;\
                                            }");
        double min,max,step=0;
        if(modelObj.value("specs").toObject().contains("min")
                || modelObj.value("specs").toObject().contains("max")
                || modelObj.value("specs").toObject().contains("step"))
        {
            if(modelObj.value("specs").toObject().value("min").toString().isEmpty())
            {
                min = 0;
            }
            else
            {
                if(modelObj.value("specs").toObject().value("min").isString())
                {
                    min = modelObj.value("specs").toObject().value("min").toString().toDouble();
                }
                else
                {
                    min = modelObj.value("specs").toObject().value("min").toDouble();
                }
            }
            if(modelObj.value("specs").toObject().value("max").toString().isEmpty())
            {
                max = 4503599627370496;  // 9223372036854775807
            }
            else
            {
                if(modelObj.value("specs").toObject().value("max").isString())
                    max = modelObj.value("specs").toObject().value("max").toString().toDouble();
                else
                    max = modelObj.value("specs").toObject().value("max").toDouble();
            }
            if(modelObj.value("specs").toObject().value("step").toString().isEmpty())
            {
                if(modelObj.value("dataType").toString() == "INT")
                    step = 1;
                else
                    step = 0.1;
            }
            else
            {
                if(modelObj.value("specs").toObject().value("step").isString())
                    step = modelObj.value("specs").toObject().value("step").toString().toDouble();
                else
                    step = modelObj.value("specs").toObject().value("step").toDouble();
            }
        }
        else
        {
            min = 0;
            max = 4503599627370496; //9223372036854775807;
            step = 1;
        }
        int accuracyArray[3];
        accuracyArray[0] = getDoubleAccuracy(min);
        accuracyArray[1] = getDoubleAccuracy(max);
        accuracyArray[2] = getDoubleAccuracy(step);
        QDoubleSpinBox_model->setRange(min,max);
        QDoubleSpinBox_model->setDecimals(getArrayMax(accuracyArray,3));
        QDoubleSpinBox_model->setSingleStep(step);
        QDoubleSpinBox_model->setEnabled(writeEnable);
        tableWidget->setRowHeight(row, 40);
        tableWidget->setCellWidget(row,column,QDoubleSpinBox_model);
    }
    else if(modelObj.value("dataType").toString() == "ENUM")
    {
        QStringList enum_list;
        newComboBox *enum_comboBox = new newComboBox(tableWidget);
        QJsonArray enumArray = modelObj.value("specs").toArray();

        for (int enum_i = 0; enum_i < enumArray.size(); enum_i++)
        {
            enum_list.append(enumArray.at(enum_i).toObject().value("name").toString());
            QString valueString = enumArray.at(enum_i).toObject().value("value").toString();
            enum_comboBox->addComboxItemValue(valueString.toInt());
        }
        enum_comboBox->addItems(enum_list);
        tableWidget->setRowHeight(row, 40);
        tableWidget->setCellWidget(row,column,enum_comboBox);
    }
    else if(modelObj.value("dataType").toString() == "TEXT")
    {
        newQLineEdit *QLineEdit_model = new newQLineEdit(tableWidget);

        int length = 0;
        if(modelObj.value("specs").toObject().contains("length"))
        {
            if(modelObj.value("specs").toObject().value("length").isString())
            {
                length = modelObj.value("specs").toObject().value("length").toString().toInt();
            }
            else
            {
                length = modelObj.value("specs").toObject().value("length").toInt();
            }
        }
        else
        {
            length = 999999999;
        }
        QLineEdit_model->setMaxLength(length);
        tableWidget->setRowHeight(row, 40);
        tableWidget->setCellWidget(row,column,QLineEdit_model);
    }
    else if(modelObj.value("dataType").toString() == "DATE")
    {
        //UTC毫秒时间戳 13个字节
        QDateTimeEdit *QDateTimeEdit_model = new QDateTimeEdit(QDateTime::currentDateTime());
        QDateTimeEdit_model->setStyleSheet("QDateTimeEdit{\
                                           color: rgb(104, 112, 130);\
                                           font-size:14px;\
                                           border-style:solid;\
                                           background-color: rgb(245, 245, 245);\
                                           border-width:1px;\
                                           border-color:rgb(229, 231, 234);\
                                           border-radius:4px;\
                                           padding-left:5px;\
                                           }\
                                           QDateTimeEdit::drop-down{\
                                           border-style:none;\
                                           width: 30px;\
                                           }\
                                           QDateTimeEdit::down-arrow {\
                                           image: url(:/png/QComboBox_down.png);\
                                           }");
        QDateTimeEdit_model->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
        QDateTimeEdit_model->setCalendarPopup(true);
        tableWidget->setRowHeight(row, 40);
        tableWidget->setCellWidget(row,column,QDateTimeEdit_model);
    }
    else if(modelObj.value("dataType").toString() == "STRUCT")
    {
        QJsonArray specsArray = modelObj.value("specs").toArray();
        int specsCount = specsArray.size();
        QStringList QTableWidget_modelHeader = {"ID", "KEY", "VALUE"};

        QTableWidget *QTableWidget_model = new QTableWidget();
        QTableWidget_model->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
        QTableWidget_model->verticalHeader()->setVisible(false);
        QTableWidget_model->setRowCount(specsCount);
        QTableWidget_model->setColumnCount(3);
        QTableWidget_model->setColumnWidth(0,40);
        QTableWidget_model->setColumnWidth(1,80);
        QTableWidget_model->setColumnWidth(2,200);
        QTableWidget_model->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
        QTableWidget_model->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);//允许拉伸
        QTableWidget_model->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
        QTableWidget_model->setHorizontalHeaderLabels(QTableWidget_modelHeader);
        QTableWidget_model->horizontalHeader()->setFont(QFont("黑体", 16));
        QTableWidget_model->setWordWrap(false);//取消自动换行
//        QTableWidget_model->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QTableWidget_model->setShowGrid(false);
        QTableWidget_model->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
        QTableWidget_model->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                                  color: rgb(104, 112, 130); \
                                                                  font-size:16px; \
                                                                  background: transparent; \
                                                                  background: rgb(246, 249, 255); \
                                                                  padding: 1px;border: none; \
                                                                  border-style:solid; \
                                                                  border-right-width:0px; \
                                                                  border-left-width:0px; \
                                                                  }");
        QTableWidget_model->setStyleSheet("QTableWidget{ \
                                              color: rgb(104, 112, 130); \
                                              font-size:14px; \
                                              border-style:solid; \
                                              border-width:1px; \
                                              border-color:rgb(229, 231, 234); \
                                              border-radius:4px; \
                                              padding:2px; \
                                              } \
                                              QTableWidget::item{ \
                                              border-style:solid; \
                                              border-bottom-width:1px; \
                                              border-color:rgb(229, 231, 234); \
                                              } \
                                              QTableWidget::indicator{width: 20px;height:20px; } \
                                              QTableWidget::indicator:checked {image: url(:/png/checkBox1.png);} \
                                              QTableWidget::indicator:unchecked {image: url(:/png/checkBox0.png);} \
                                              QTableWidget::item::selected { \
                                              background: rgba(47, 105, 235, 0.6); \
                                              }");
        tableWidget->setRowHeight(row, 200);
        tableWidget->setCellWidget(row,column,QTableWidget_model);

        for(int specs_i=0;specs_i<specsCount;specs_i++)
        {
            QJsonObject specsObj = specsArray.at(specs_i).toObject();
            QTableWidgetItem *itemID;
            if(specsObj.value("id").isString())
            {
                itemID = new QTableWidgetItem(QString::number(specsObj.value("id").toString().toInt()));
            }
            else
            {
                itemID = new QTableWidgetItem(QString::number(specsObj.value("id").toInt()));
            }
            itemID->setFlags(itemID->flags() & (~Qt::ItemIsEditable));
            QTableWidget_model->setItem(specs_i,0,itemID);
            QTableWidgetItem *itemKey = new QTableWidgetItem(specsObj.value("name").toString());
            itemKey->setFlags(itemKey->flags() & (~Qt::ItemIsEditable));
            QTableWidget_model->setItem(specs_i,1,itemKey);
            modelCreate(specs_i,2,specsObj,QTableWidget_model,writeEnable);
        }
        return specsCount+2;   
    }
    else if(modelObj.value("dataType").toString() == "ARRAY")
    {
        QJsonObject specsObject = modelObj.value("specs").toObject();
        int specsCount = 0;
        if(specsObject.value("size").isString())
            specsCount = specsObject.value("size").toString().toInt();
        else if(specsObject.value("size").isDouble())
            specsCount = specsObject.value("size").toInt();
        QStringList QTableWidget_modelHeader = {"INDEX","VALUE"};
        QTableWidget *QTableWidget_model = new QTableWidget();
        QTableWidget_model->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
//        QTableWidget_model->setSelectionBehavior(QAbstractItemView::SelectRows);
        QTableWidget_model->verticalHeader()->setVisible(false);
        QTableWidget_model->setRowCount(specsCount);
        QTableWidget_model->setColumnCount(2);
        QTableWidget_model->setColumnWidth(0,45);
        QTableWidget_model->setColumnWidth(1,200);
        QTableWidget_model->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
        QTableWidget_model->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
        QTableWidget_model->setHorizontalHeaderLabels(QTableWidget_modelHeader);
        QTableWidget_model->horizontalHeader()->setFont(QFont("黑体", 16));
        QTableWidget_model->setWordWrap(false);//取消自动换行
        QTableWidget_model->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QTableWidget_model->setShowGrid(false);
        QTableWidget_model->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
        QTableWidget_model->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                                  color: rgb(104, 112, 130); \
                                                                  font-size:16px; \
                                                                  background: transparent; \
                                                                  background: rgb(246, 249, 255); \
                                                                  padding: 1px;border: none; \
                                                                  border-style:solid; \
                                                                  border-right-width:0px; \
                                                                  border-left-width:0px; \
                                                                  }");
        QTableWidget_model->setStyleSheet("QTableWidget{ \
                                              color: rgb(104, 112, 130); \
                                              font-size:14px; \
                                              border-style:solid; \
                                              border-width:1px; \
                                              border-color:rgb(229, 231, 234); \
                                              border-radius:4px; \
                                              padding:2px; \
                                              } \
                                              QTableWidget::item{ \
                                              border-style:solid; \
                                              border-bottom-width:1px; \
                                              border-color:rgb(229, 231, 234); \
                                              } \
                                              QTableWidget::indicator{width: 20px;height:20px; } \
                                              QTableWidget::indicator:checked {image: url(:/png/checkBox1.png);} \
                                              QTableWidget::indicator:unchecked {image: url(:/png/checkBox0.png);} \
                                              QTableWidget::item::selected { \
                                              background: rgba(47, 105, 235, 0.6); \
                                              }");
        tableWidget->setRowHeight(row, 200);
        tableWidget->setCellWidget(row,column,QTableWidget_model);

        for(int specs_i=0;specs_i<specsCount;specs_i++)
        {
            QTableWidgetItem *itemID = new QTableWidgetItem(QString::number(0));
            itemID->setFlags(itemID->flags() & (~Qt::ItemIsEditable));
            QTableWidget_model->setItem(specs_i,0,itemID);
            modelCreate(specs_i,1,specsObject,QTableWidget_model,writeEnable);
        }
        return specsCount+1;
    }
    return 1;
}
/**************************************************************************
** 功能	@brief : 更新物模型数据到qtablewidget
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelProcess::updateModelPointList(QJsonArray modelArray,QTableWidget * tableWidget)
{
//    qInfo()<<__FUNCTION__;
//    bool isEvent = false;
    if(modelArray.at(0).toObject().value("type").toString() == "PROPERTY")
    {
        g_properties = modelArray;
    }
    int modelRowCount = modelArray.size();
    tableWidget->setRowCount(modelArray.size());
    //隐藏头部
    tableWidget->verticalHeader()->setVisible(false);
    for(int model_i=0;model_i<modelRowCount;model_i++)
    {
        QJsonObject modelObj = modelArray.at(model_i).toObject();
        if(modelObj.value("id").isString())
        {
            QTableWidgetItem *itemID = new QTableWidgetItem(QString::number(modelObj.value("id").toString().toInt()));
            itemID->setFlags(itemID->flags() & (~Qt::ItemIsEditable));
            tableWidget->setItem(model_i,0,itemID);
        }
        else
        {
            QTableWidgetItem *itemID = new QTableWidgetItem(QString::number(modelObj.value("id").toInt()));
            itemID->setFlags(itemID->flags() & (~Qt::ItemIsEditable));
            tableWidget->setItem(model_i,0,itemID);
        }
        QTableWidgetItem *itemNamel = new QTableWidgetItem(modelObj.value("name").toString());
        itemNamel->setFlags(itemNamel->flags() &(~Qt::ItemIsEditable));
        tableWidget->setItem(model_i,1,itemNamel);

        if(modelObj.value("type").toString() == "SERVICE" || modelObj.value("type").toString() == "EVENT")
        {
//            if (modelObj.value("type").toString() == "EVENT")
//            {
//                isEvent = true;
//            }
            QJsonArray inputDataArray = modelObj.value("inputData").toArray();
            QJsonArray outputDataArray = modelObj.value("outputData").toArray();
            int rowCount = inputDataArray.count()+outputDataArray.count();
            info_type info;
            info.name.append(QString::number(modelObj.value("id").toInt()));
            QStringList QTableWidget_modelHeader = {"ID", "KEY", "VALUE"};
            QTableWidget *QTableWidget_model = new QTableWidget();
            QTableWidget_model->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
//            QTableWidget_model->setSelectionBehavior(QAbstractItemView::SelectRows);
            QTableWidget_model->verticalHeader()->setVisible(false);
            QTableWidget_model->setRowCount(rowCount);
            QTableWidget_model->setColumnCount(3);
            QTableWidget_model->setColumnWidth(0,40);
            QTableWidget_model->setColumnWidth(1,80);
            QTableWidget_model->setColumnWidth(2,200);
            QTableWidget_model->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
            QTableWidget_model->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);//允许拉伸
            QTableWidget_model->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
            QTableWidget_model->setHorizontalHeaderLabels(QTableWidget_modelHeader);
            QTableWidget_model->horizontalHeader()->setFont(QFont("黑体", 16));
            QTableWidget_model->setWordWrap(false);//取消自动换行
            QTableWidget_model->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//            QTableWidget_model->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            QTableWidget_model->setShowGrid(false);
            QTableWidget_model->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
            QTableWidget_model->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                                      color: rgb(104, 112, 130); \
                                                                      font-size:16px; \
                                                                      background: transparent; \
                                                                      background: rgb(246, 249, 255); \
                                                                      padding: 1px;border: none; \
                                                                      border-style:solid; \
                                                                      border-right-width:0px; \
                                                                      border-left-width:0px; \
                                                                      }");
            QTableWidget_model->setStyleSheet("QTableWidget{ \
                                                  color: rgb(104, 112, 130); \
                                                  font-size:14px; \
                                                  border-style:solid; \
                                                  border-width:1px; \
                                                  border-color:rgb(229, 231, 234); \
                                                  border-radius:4px; \
                                                  padding:2px; \
                                                  } \
                                                  QTableWidget::item{ \
                                                  border-style:solid; \
                                                  border-bottom-width:1px; \
                                                  border-color:rgb(229, 231, 234); \
                                                  } \
                                                  QTableWidget::indicator{width: 20px;height:20px; } \
                                                  QTableWidget::indicator:checked {image: url(:/png/checkBox1.png);} \
                                                  QTableWidget::indicator:unchecked {image: url(:/png/checkBox0.png);} \
                                                  QTableWidget::item::selected { \
                                                  background: rgba(47, 105, 235, 0.6); \
                                                  }");
            tableWidget->setRowHeight(model_i, 200);
            tableWidget->setCellWidget(model_i,2,QTableWidget_model);

            int nodeCount = 0;
            int inputDataSum = inputDataArray.count();
            if(inputDataSum > 0)
            {
                for (int specs_i = 0; specs_i < inputDataSum; specs_i++)
                {
                    QJsonObject specsObj = inputDataArray.at(specs_i).toObject();
                    if(g_properties.isEmpty())
                    {
                        qCritical()<<"service or event model can not explain";
                        return;
                    }
                    if(!specsObj.value("$ref").isUndefined() && (!specsObj.value("$ref").isNull()))
                    {
                        QString orientation = specsObj.value("$ref").toString();
                        QStringList nameList = orientation.split("/");
                        if(nameList.size() == 4 && QString(nameList[1]).contains("properties") && QString(nameList[2]).contains("id"))
                        {
                            int idex =  QString(nameList[3]).toInt();
                            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(idex));
                            QTableWidget_model->setItem(specs_i,0,idItem);
                            for(int propertiesModel_i=0;propertiesModel_i<g_properties.size();propertiesModel_i++)
                            {
                                QJsonObject propertyModelObj = g_properties.at(propertiesModel_i).toObject();
                                if(propertyModelObj.value("id").toInt() == idex)
                                {
                                    QTableWidgetItem *nameItem = new QTableWidgetItem(propertyModelObj.value("name").toString());
                                    QTableWidget_model->setItem(specs_i,1,nameItem);
                                    nodeCount+=modelCreate(specs_i,2,propertyModelObj,QTableWidget_model,false);
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        QTableWidgetItem *itemID = new QTableWidgetItem(QString::number(specsObj.value("id").toInt()));
                        QTableWidgetItem *itemName = new QTableWidgetItem(specsObj.value("name").toString());

//                        itemName->setBackground(QBrush(QColor(255,250,205)));
//                        itemName->setBackgroundColor(QColor(255,250,205));

                        QTableWidget_model->setItem(specs_i,0,itemID);
                        QTableWidget_model->setItem(specs_i,1,itemName);
                        nodeCount += modelCreate(specs_i,2,specsObj,QTableWidget_model,false);
                    }
                    info.input.insert(QString::number(specsObj.value("id").toInt()),0);
                }
            }
            int outputDataSum = outputDataArray.count();
            if(outputDataSum)
            {
                for (int specs_i = 0; specs_i < outputDataSum; specs_i++)
                {
                    QJsonObject specsObj = outputDataArray.at(specs_i).toObject();
                    if(g_properties.isEmpty())
                    {
                        qCritical()<<"service or event model can not explain";
                        return;
                    }
                    if(!specsObj.value("$ref").isUndefined() && (!specsObj.value("$ref").isNull()))
                    {
                        QString orientation = specsObj.value("$ref").toString();
                        QStringList nameList = orientation.split("/");
                        if(nameList.size() == 4 && QString(nameList[1]).contains("properties") && QString(nameList[2]).contains("id"))
                        {
                            int idex =  QString(nameList[3]).toInt();
                            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(idex));
                            QTableWidget_model->setItem(inputDataSum+specs_i,0,idItem);
                            for(int propertiesModel_i=0;propertiesModel_i<g_properties.size();propertiesModel_i++)
                            {
                                QJsonObject propertyModelObj = g_properties.at(propertiesModel_i).toObject();
                                if(propertyModelObj.value("id").toInt() == idex)
                                {
                                    QTableWidgetItem *nameItem = new QTableWidgetItem(propertyModelObj.value("name").toString());
//                                    idItem->setFlags(idItem->flags() & (~Qt::ItemIsEditable));
//                                    nameItem->setFlags(nameItem->flags() & (~Qt::ItemIsEditable));
                                    QTableWidget_model->setItem(inputDataSum+specs_i,1,nameItem);
                                    nodeCount+=modelCreate(inputDataSum+specs_i,2,propertyModelObj,QTableWidget_model);
                                    break;
                                }
                            }
                        }
                    }
                    else
                    {
                        QTableWidgetItem *itemID = new QTableWidgetItem(QString::number(specsObj.value("id").toInt()));
                        QTableWidgetItem *itemName = new QTableWidgetItem(specsObj.value("name").toString());
//                        itemID->setFlags(itemID->flags() & (~Qt::ItemIsEditable));
//                        itemName->setFlags(itemName->flags() & (~Qt::ItemIsEditable));
//                        itemName->setBackgroundColor(QColor(255,250,205));
                        QTableWidget_model->setItem(inputDataSum+specs_i,0,itemID);
                        QTableWidget_model->setItem(inputDataSum+specs_i,1,itemName);
                        nodeCount += modelCreate(inputDataSum+specs_i,2,specsObj,QTableWidget_model);
                    }
                    info.output.insert(QString::number(specsObj.value("id").toInt()),0);
                }
            }
            serverMapList->addMapToTable(info);
//            QTableWidget_model->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
//            QTableWidget_model->horizontalHeader()->setSectionResizeMode(QTableWidget_model->horizontalHeader()->count()-1,QHeaderView::Stretch);
//            QTableWidget_model->setMaximumHeight(30+30*nodeCount);
//            QTableWidget_model->resizeRowsToContents();
        }
        else if(modelObj.value("type").toString() == "PROPERTY")
        {
            if(modelObj.value("dataType").toString() == "ARRAY")
            {
                QJsonObject specsObj = modelObj.value("specs").toObject();
                int size;
                if(specsObj.value("size").isString())
                {
                    size = specsObj.value("size").toString().toInt();
                }
                else
                {
                    size = specsObj.value("size").toInt();
                }
                QStringList QTableWidget_modelHeader = {"INDEX","VALUE"};

                QTableWidget *QTableWidget_model = new QTableWidget();
                QTableWidget_model->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerPixel);
//                QTableWidget_model->setSelectionBehavior(QAbstractItemView::SelectRows);
                QTableWidget_model->verticalHeader()->setVisible(false);
                QTableWidget_model->setRowCount(size);
                QTableWidget_model->setColumnCount(2);
                QTableWidget_model->setColumnWidth(0,45);
                QTableWidget_model->setColumnWidth(1,200);
                QTableWidget_model->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
                QTableWidget_model->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);//允许拉伸
                QTableWidget_model->setHorizontalHeaderLabels(QTableWidget_modelHeader);
                QTableWidget_model->horizontalHeader()->setFont(QFont("黑体", 16));
                QTableWidget_model->setWordWrap(false);//取消自动换行
                QTableWidget_model->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//                QTableWidget_model->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                QTableWidget_model->setShowGrid(false);
                QTableWidget_model->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
                QTableWidget_model->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                                          color: rgb(104, 112, 130); \
                                                                          font-size:16px; \
                                                                          background: transparent; \
                                                                          background: rgb(246, 249, 255); \
                                                                          padding: 1px;border: none; \
                                                                          border-style:solid; \
                                                                          border-right-width:0px; \
                                                                          border-left-width:0px; \
                                                                          }");
                QTableWidget_model->setStyleSheet("QTableWidget{ \
                                                      color: rgb(104, 112, 130); \
                                                      font-size:14px; \
                                                      border-style:solid; \
                                                      border-width:1px; \
                                                      border-color:rgb(229, 231, 234); \
                                                      border-radius:4px; \
                                                      padding:2px; \
                                                      } \
                                                      QTableWidget::item{ \
                                                      border-style:solid; \
                                                      border-bottom-width:1px; \
                                                      border-color:rgb(229, 231, 234); \
                                                      } \
                                                      QTableWidget::indicator{width: 20px;height:20px; } \
                                                      QTableWidget::indicator:checked {image: url(:/png/checkBox1.png);} \
                                                      QTableWidget::indicator:unchecked {image: url(:/png/checkBox0.png);} \
                                                      QTableWidget::item::selected { \
                                                      background: rgba(47, 105, 235, 0.6); \
                                                      }");
                tableWidget->setRowHeight(model_i, 200);
                tableWidget->setCellWidget(model_i,2,QTableWidget_model);
                int nodeCount = 0;
                for (int specs_i = 0; specs_i < size; specs_i++)
                {
                    QTableWidgetItem *itemName = new QTableWidgetItem(QString::number(specs_i));
                    itemName->setFlags(itemName->flags() & (~Qt::ItemIsEditable));
                    QTableWidget_model->setItem(specs_i,0,itemName);
                    nodeCount += modelCreate(specs_i,1,specsObj,QTableWidget_model);
                }
            }
            else
            {
                modelCreate(model_i,2,modelObj,tableWidget);
            }
        }
        else
        {
            qWarning()<<QObject::tr("暂无此事件处理");
        }
    }

    //重新调整行高
    tableWidget->horizontalHeader()->setDefaultSectionSize(50);  //设置默认宽度
    tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);
    tableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    tableWidget->setColumnWidth(0,40);
    tableWidget->setColumnWidth(1,130);
    tableWidget->setColumnWidth(2,200);
    tableWidget->setWordWrap(false);//取消自动换行
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableWidget->setShowGrid(false);
    tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
    tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              border-right-width:0px; \
                                                              border-left-width:0px; \
                                                              }");
    tableWidget->setStyleSheet("QTableWidget{ \
                                          color: rgb(104, 112, 130); \
                                          font-size:14px; \
                                          border-style:solid; \
                                          border-width:1px; \
                                          border-color:rgb(229, 231, 234); \
                                          border-radius:4px; \
                                          padding:2px; \
                                          } \
                                          QTableWidget::item{ \
                                          border-style:solid; \
                                          border-bottom-width:1px; \
                                          border-color:rgb(229, 231, 234); \
                                          } \
                                          QTableWidget::indicator{width: 20px;height:20px; } \
                                          QTableWidget::indicator:checked {image: url(:/png/checkBox1.png);} \
                                          QTableWidget::indicator:unchecked {image: url(:/png/checkBox0.png);} \
                                          QTableWidget::item::selected { \
                                          background: rgba(47, 105, 235, 0.6); \
                                          }");
}

/**************************************************************************
** 功能	@brief : 分析匹配接收的物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int modelProcess::analyzeModelData(QByteArray data, QTableWidget *tableWidget, int *offset,int *arrayCount)
{
    qDebug()<<"offset"<<*offset<<data.mid(*offset).length();
    int analysisFlag = 1;
    bool ok;
    int id = data.mid(*offset,2).toHex().toInt(&ok,16);
    int type = id&0x07;
    QWidget *box = NULL;
    id >>= 3;
//    qDebug()<<data.mid(*offset).toHex()<<"id"<<id<<"type"<<type;
    *offset += 2;
    if(tableWidget == nullptr)
        qDebug()<<"错误";
    if(tableWidget->columnCount() == 2)
    {
        qDebug()<<"数组"<<*arrayCount<<tableWidget->rowCount();
        box = tableWidget->cellWidget(*arrayCount,1);
        (*arrayCount)++;
    }
    else if(tableWidget->columnCount() == 3)
    {
        qDebug()<<"结构体";
        for (int i = 0; i < tableWidget->rowCount(); i++)
        {
            qDebug()<<tableWidget->item(i,0)->text().toInt();
            if(tableWidget->item(i,0)->text().toInt() == id)
            {
                box = tableWidget->cellWidget(i,2);
                break;
            }
        }
        if(box == NULL)
        {
            qCritical()<<"找不到ID"<<id;
            return -1;
        }
    }
    else
    {
        qDebug()<<"errno";
    }

    qDebug()<<"box"<<box->metaObject()->className();
    switch (type)
    {
        case DP_TTLV_TYPE_BOOL_FALSE:
        {
            SwitchButton *boolBox = (SwitchButton *)box;
            boolBox->setChecked(false);
            break;
        }
        case DP_TTLV_TYPE_BOOL_TRUE:
        {
            SwitchButton *boolBox = (SwitchButton *)box;
            boolBox->setChecked(true);
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
            if(box->inherits("QComboBox"))
            {
                newComboBox *comboBox = (newComboBox *)box;
                int currentID = 0;
                qDebug()<<"vaule_double:"<<vaule_double;
                currentID = comboBox->getComboxItemIdex(vaule_double);
                qDebug()<<"currentID:"<<currentID;
                comboBox->setCurrentIndex(currentID);
            }
            else if(box->inherits("QDoubleSpinBox"))
            {
                QDoubleSpinBox *spinBox = (QDoubleSpinBox *)box;
                spinBox->setValue(vaule_double);
            }
            else if(box->inherits("QDateTimeEdit"))
            {
                QDateTimeEdit *DateBox = (QDateTimeEdit *)box;
                QDateTime time = QDateTime::fromMSecsSinceEpoch(vaule_double);
                DateBox->setDateTime(time);
            }
            break;
        }
        case DP_TTLV_TYPE_BINARY:
        {
            int dataLen = data.mid(*offset,2).toHex().toInt(&ok,16);
            newQLineEdit *lineEditBox = (newQLineEdit *)box;
            lineEditBox->setText(data.mid(*offset+2,dataLen));
            *offset += 2+dataLen;
            break;
        }
        case DP_TTLV_TYPE_STRUCT:
        {
            int count = data.mid(*offset,2).toHex().toInt(&ok,16);
            *offset += 2;
            QTableWidget *tableWidget_box;
            for(int i = 0;i<count;i++)
            {
                tableWidget_box = (QTableWidget *)box;
                if(0 == analyzeModelData(data,tableWidget_box,offset,arrayCount))
                {
                    qCritical()<<"explain data error";
                    break;
                }
            }
            break;
        }
        default:
            analysisFlag = 0;
            break;
    }
    return analysisFlag;
}
/**************************************************************************
** 功能	@brief : 处理接收的物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int modelProcess::recvModelData(QByteArray data,QTableWidget * tableWidget)
{
    qInfo()<<__FUNCTION__;
    qDebug()<<"data:"<<data.toHex();
    int offset = 0;
    int arrayCount = 0;
    int rowCount = 0;
    int ret;
    QTableWidget *oldQTableWidget = tableWidget;
    do
    {
        ret = analyzeModelData(data,tableWidget,&offset,&arrayCount);
        if(ret == 0)
        {
            MMessageBox::critical(NULL,QObject::tr("错误"),QObject::tr("物模型数据解析错误"),QObject::tr("确认"));
            break;
        }
        else if(ret == 1)
        {
            tableWidget = oldQTableWidget;
            rowCount++;
            arrayCount = 0;
        }
        else if(ret == -1)
            return ret;
    }while(offset<data.length() && rowCount<tableWidget->rowCount());
    return offset;
}
/**************************************************************************
** 功能	@brief : 解析接收的物模型数据的json格式
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool modelProcess::explainBaseJsonData(QJsonValue modelValue,QWidget * box)
{
    qInfo()<<__FUNCTION__;
    if(modelValue.isString())
    {
        QLineEdit *lineEditBox = (QLineEdit *)box;
        lineEditBox->setText(modelValue.toString());
    }
    else if(modelValue.isBool())
    {

        SwitchButton *boolBox = (SwitchButton *)box;
        boolBox->setChecked(modelValue.toBool());
    }else if(modelValue.isDouble())
    {
        qDebug()<<"box"<<box->metaObject()->className();
        if(box->inherits("QComboBox"))
        {
            newComboBox *comboBox = (newComboBox *)box;
            int currentID = 0;
            currentID = comboBox->getComboxItemIdex(modelValue.toInt());
            comboBox->setCurrentIndex(currentID);
        }
        else if(box->inherits("QDoubleSpinBox"))
        {
            QDoubleSpinBox *spinBox = (QDoubleSpinBox *)box;
            spinBox->setValue(modelValue.toDouble());
        }
        else if(box->inherits("QDateTimeEdit"))
        {
            QDateTimeEdit *DateBox = (QDateTimeEdit *)box;
            QDateTime time = QDateTime::fromMSecsSinceEpoch(qint64(modelValue.toDouble()));
            DateBox->setDateTime(time);
        }
    }
    else
        return false;
    return true;
}

void modelProcess::setServerFlag(bool flag)
{
    this->serverFlag = flag;
}
/**************************************************************************
** 功能	@brief : 更新读取到的物模型数据到qtablewidget
** 输入	@param :json的array类
** 输出	@retval:
***************************************************************************/
void modelProcess::updateReadModelPointList(QJsonArray modelArray,QTableWidget * tableWidget)
{
    qInfo()<<__FUNCTION__;
    int modelObjCount = modelArray.size();
    QWidget *box = NULL;
    int item_i=0;
    for(int model_i=0;model_i<modelObjCount;model_i++)
    {
        bool exsitFlag = false;
        for(;item_i < tableWidget->rowCount();item_i++)
        {
            QJsonValue value;
            value = modelArray.at(item_i);
            if(value.isArray())
            {
                QJsonArray jsonArray = value.toArray();
                if(tableWidget->columnCount() == 2)
                    box = tableWidget->cellWidget(item_i,1);
                else
                    box = tableWidget->cellWidget(item_i,2);
                updateReadModelPointList(jsonArray,(QTableWidget *)box);
            }
            else if(value.isObject())
            {
                QJsonObject jsonObject = value.toObject();
                if(tableWidget->columnCount() == 2)
                    box = tableWidget->cellWidget(item_i,1);
                else
                    box = tableWidget->cellWidget(item_i,2);
                updateReadModelPointList(jsonObject,(QTableWidget *)box);
            }

            if(box == NULL)
            {
                if(tableWidget->columnCount() == 2)
                    box = tableWidget->cellWidget(item_i,1);
                else
                    box = tableWidget->cellWidget(item_i,2);
            }
            if(explainBaseJsonData(value,box))
            {
                exsitFlag = true;
                break;
            }
            if(exsitFlag == false)
            {
                //return -1;
            }
         }
      }
}
/**************************************************************************
** 功能	@brief : 更新读取到的物模型数据到qtablewidget
** 输入	@param :json的object类
** 输出	@retval:
***************************************************************************/
bool modelProcess::updateReadModelPointList(QJsonObject modelObj,QTableWidget * tableWidget)
{
    qInfo()<<__FUNCTION__;
    bool handleDataFlag=false;



    QWidget *box = NULL;
    for(int item_i=0;item_i < tableWidget->rowCount();item_i++)
    {
        box = tableWidget->cellWidget(item_i,2);
        if (box != NULL)
        {
            qDebug()<<"box:"<<box->metaObject()->className();
        }
    }
    QJsonObject::iterator it = modelObj.begin();
    while(it != modelObj.end())
    {
        bool exsitFlag = false;
        box = NULL;
        for(int item_i=0;item_i < tableWidget->rowCount();item_i++)
        {
            int key = it.key().compare(tableWidget->item(item_i,0)->text());
            qDebug()<<"tableWidget->item(item_i,0)->text():"<<tableWidget->item(item_i,0)->text();
            qDebug()<<"key:"<<key;
            QJsonValue value;
            if(key!=0)
            {
                continue;
            }
            else
            {
                value = it.value();
                handleDataFlag = true;
            }
            if(value.isArray())
            {
                QJsonArray jsonArray = value.toArray();
                if(tableWidget->columnCount() == 2)
                    box = tableWidget->cellWidget(item_i,1);
                else
                    box = tableWidget->cellWidget(item_i,2);
                if (box->metaObject()->className() == QString("QTableWidget"))
                {
                    updateReadModelPointList(jsonArray,(QTableWidget *)box);
                }
                else
                {
                    MMessageBox::critical(NULL,QObject::tr("错误"),QObject::tr("物模型数据解析错误"),QObject::tr("确认"));
                }
            }
            else if(value.isObject())
            {
                QJsonObject jsonObject = value.toObject();
                if(tableWidget->columnCount() == 2)
                    box = tableWidget->cellWidget(item_i,1);
                else
                    box = tableWidget->cellWidget(item_i,2);
                if (box->metaObject()->className() == QString("QTableWidget"))
                {
                    updateReadModelPointList(jsonObject,(QTableWidget *)box);
                }
                else
                {
                    MMessageBox::critical(NULL,QObject::tr("错误"),QObject::tr("物模型数据解析错误"),QObject::tr("确认"));
                }
            }
            if(box == NULL)
            {
                if(tableWidget->columnCount() == 2)
                    box = tableWidget->cellWidget(item_i,1);
                else
                    box = tableWidget->cellWidget(item_i,2);
            }
           if(explainBaseJsonData(value,box))
           {
                exsitFlag = true;
                break;
           }

            if(exsitFlag == false)
            {
                qDebug()<<"errno ";
                //return -1;
            }
         }
        it++;
      }
    return handleDataFlag;
}
/**************************************************************************
** 功能	@brief : 处理接收的物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int modelProcess::recvModelData(QByteArray data,QTableWidget * tableWidget,int mode)
{
    Q_UNUSED(mode);
    qInfo()<<__FUNCTION__<< __LINE__;
    int ret = 1;
    if(data.isEmpty())
    {
        ret = -1;
    }
    else
    {
        jsonFile *jsonJudge = new jsonFile(NULL);
        if(jsonJudge->isJsonObject(data))
        {

            QJsonParseError jsonErr;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(data,&jsonErr);
            if(jsonErr.error == QJsonParseError::NoError)
            {
                QJsonObject obj = jsonDoc.object();
                bool result = updateReadModelPointList(obj,tableWidget);
                if(result == false)
                {
                    ret = -1;
                }
            }
        }
        else
        {
            ret = recvModelData(data,tableWidget);
        }
        delete jsonJudge;
    }

    return ret;
}

/**************************************************************************
** 功能	@brief : 解析读取物模型的AT命令数据的id号
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QByteArray modelProcess::extractServerId(QByteArray data,int mode)
{
    qInfo()<<__FUNCTION__;
    if(mode == TTLVMODESNED)
    {
        bool ok;
        int id = data.mid(0,2).toHex().toInt(&ok,16);
        id >>= 3;

        return QString::number(id).toUtf8();

    }
    else
    {
        QJsonParseError jsonErr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data,&jsonErr);
        if(jsonErr.error == QJsonParseError::NoError)
        {
            QJsonObject obj = jsonDoc.object();
            QJsonObject::iterator it = obj.begin();
            QString id = it.key();
            return id.toUtf8();
        }
        else
            return NULL;
    }
}
/**************************************************************************
** 功能	@brief :   获取选择的qtablewidget行
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QList<int> modelProcess::getListSelectedRows(QTableWidget * tableWidget)
{
    qInfo()<<__FUNCTION__;
    QList<QTableWidgetSelectionRange>ranges = tableWidget->selectedRanges();
    QList<int> rowList;
    int count=ranges.count();
    for(int i=0;i<count;i++)
    {

       int topRow=ranges.at(i).topRow();
       int bottomRow=ranges.at(i).bottomRow();
       for(int j=topRow;j<=bottomRow;j++)
       {
           rowList.append(j);
       }
    }
    std::sort(rowList.begin(), rowList.end());
    QList<int> rowListReverse;
    for(int i =rowList.count()-1;i>=0;i--)
    {
        rowListReverse.append(rowList[i]);
    }
    return rowListReverse;
}

/**************************************************************************
** 功能	@brief :   判断16进制数的长度
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int intOfCharacter(uint64_t Number)
{
    int count = 0;
    if(Number == 0)
    {
        return 1;
    }
    uint64_t NumberFork = Number;
    for (int i = 0; i < 8; i++)
    {
        if(NumberFork > 0)
        {
            count++;
        }
        else
        {
            break;
        }
        NumberFork = (NumberFork>> 8) & 0xFFFFFFFFFFFFFFFF;
    }
    return count;
}
/**************************************************************************
** 功能	@brief :   获取doble数据的长度
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int getIntFormDouble(double tempDouble,quint64 *tempInt)
{
    QString doubleString;
    doubleString = QString::number(tempDouble,'f',15);
    QStringList doubleStringList = doubleString.split(".");
    QString test = doubleStringList.at(1);
    if(doubleStringList.count() == 2)
    {
        for (int i = doubleStringList.at(1).length()-1; i >= 0 ; i--)
        {
            if(doubleStringList.at(1)[i] != "0")
            {             
                QString number;
                if(doubleStringList.at(0).toLongLong() > 0)
                {
                    number.append(doubleStringList.at(0));
                }
                number.append(test);
                *tempInt = number.toLongLong();
                return i +1;
            }
            test.chop(1);
        }
    }
    *tempInt = doubleStringList.at(0).toLongLong();
    return 0;
}

/**************************************************************************
** 功能	@brief :   获取控件中的数据并转成ttlv格式数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QByteArray modelProcess::getDataFormBox(int id,QWidget * box)
{
    qDebug()<<id<<box;
    if(box == NULL)
        return NULL;
    QByteArray data;
    if(box->inherits("SwitchButton"))
    {
        SwitchButton *boolBox = (SwitchButton *)box;
        bool boxValue = boolBox->getChecked();
        data.append(IntToByte((id<<3)|boxValue,2));
    }
    else if(box->inherits("QComboBox"))
    {
        newComboBox *comboBox = (newComboBox *)box;
        int boxValue = 0;
        boxValue = comboBox->getComboxItemValue(comboBox->currentIndex());

        data.append(IntToByte((id<<3)|2,2));
        data.append(IntToByte(0,1));
        data.append(IntToByte(boxValue,1));
    }
    else if(box->inherits("QDoubleSpinBox"))
    {
        QDoubleSpinBox *spinBox = (QDoubleSpinBox *)box;
        double boxValue = spinBox->value();
        bool negative = false;
        int amp = 0;
        int byteLen = 0;
        if(boxValue < 0)
        {
            negative = true;
            boxValue = -boxValue;
        }
        quint64 intvalue = 0;
        amp = getIntFormDouble(boxValue,&intvalue);
        byteLen = intOfCharacter(intvalue);
//        qDebug()<<"negative"<<negative<<amp<<byteLen;
        data.append(IntToByte((id<<3)|2,2));
        data.append(IntToByte((negative<<7)|(amp << 3)|(byteLen-1),1));
        data.append(IntToByte(intvalue,byteLen));

    }
    else if(box->inherits("QDateTimeEdit"))
    {
        QDateTimeEdit *DateBox = (QDateTimeEdit *)box;
        QDateTime time = DateBox->dateTime();
//        qint64 timeT = time.currentMSecsSinceEpoch();   //将当前时间转为时间戳
        quint64 timeT = (qint64)(time.toTime_t())* 1000;
        bool negative = false;
        int amp = 0;
        int byteLen = 0;
        quint64 intvalue = 0;
        amp = getIntFormDouble(timeT,&intvalue);
        byteLen = intOfCharacter(intvalue);
//        qDebug()<<"negative"<<negative<<amp<<byteLen;
        data.append(IntToByte((id<<3)|2,2));
        data.append(IntToByte((negative<<7)|(amp << 3)|(byteLen-1),1));
        data.append(IntToByte(intvalue,byteLen));
    }
    else if(box->inherits("QLineEdit"))
    {
        newQLineEdit * LineEditBox = (newQLineEdit *)box;
        data.append(IntToByte((id<<3)|3,2));

        qDebug()<<LineEditBox->text();
        qDebug()<<"length:"<<LineEditBox->text().count();
        qDebug()<<"size:"<<LineEditBox->text().toUtf8().length();

        if (LineEditBox->objectName() == "byte")
        {
            QByteArray byte;
            QString stringByte = LineEditBox->text();
            for (int i = 1; i < stringByte.size() / 2; i ++)
            {
                bool ok;
                byte.append(IntToByte(stringByte.mid(i*2,2).toInt(&ok, 16),1));
            }
            data.append(IntToByte(byte.length(),2));
            data.append(byte);
        }
        else
        {
            data.append(IntToByte(LineEditBox->text().toUtf8().length(),2));
            data.append(LineEditBox->text().toUtf8());
        }
    }
    else if(box->inherits("QTableWidget"))
    {
        QTableWidget * tableWidgetBox = (QTableWidget *)box;
        qDebug()<<tableWidgetBox->horizontalHeaderItem(0)->text();
        data.append(IntToByte((id<<3)|4,2));
        data.append(IntToByte(tableWidgetBox->rowCount(),2));
        if(tableWidgetBox->horizontalHeaderItem(0)->text() == "INDEX")
        {
            qInfo()<<QObject::tr("数组");
            for (int row = 0; row < tableWidgetBox->rowCount(); row++)
            {
                QWidget *childBox = tableWidgetBox->cellWidget(row,1);
                data.append(getDataFormBox(0,childBox));
            }
        }
        else if(tableWidgetBox->horizontalHeaderItem(0)->text() == "ID")
        {
            qInfo()<<QObject::tr("表格");
            for (int row = 0; row < tableWidgetBox->rowCount(); row++)
            {
                int childId = tableWidgetBox->item(row,0)->text().toInt();
                QWidget *childBox = tableWidgetBox->cellWidget(row,2);
                data.append(getDataFormBox(childId,childBox));
            }
        }
    }
    return data;
}

/**************************************************************************
** 功能	@brief :   获取控件中的数据并转成json格式数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QJsonObject modelProcess::getJsonDataFormBox(int id,QWidget * box)
{
    QJsonObject Obj;
    if(box->inherits("SwitchButton"))
    {
        SwitchButton *boolBox = (SwitchButton *)box;
        bool boxValue = boolBox->getChecked();
        Obj.insert(QString::number(id),boxValue);
    }
    else if(box->inherits("QComboBox"))
    {

        newComboBox *comboBox = (newComboBox *)box;
        int boxValue = 0;
        boxValue = comboBox->getComboxItemValue(comboBox->currentIndex());
        Obj.insert(QString::number(id),boxValue);
    }
    else if(box->inherits("QDoubleSpinBox"))
    {
        QDoubleSpinBox *spinBox = (QDoubleSpinBox *)box;
        double boxValue = spinBox->value();
        Obj.insert(QString::number(id),boxValue);

    }
    else if(box->inherits("QDateTimeEdit"))
    {
        QDateTimeEdit *DateBox = (QDateTimeEdit *)box;
        QDateTime time = DateBox->dateTime();
        qint64 timeT = (qint64)(time.toTime_t())* 1000;
        Obj.insert(QString::number(id),timeT);
    }
    else if(box->inherits("QLineEdit"))
    {
        newQLineEdit * LineEditBox = (newQLineEdit *)box;
        // 检测 字节流 还是 字符串
        Obj.insert(QString::number(id),LineEditBox->text());
    }
    else if(box->inherits("QTableWidget"))
    {
        QTableWidget * tableWidgetBox = (QTableWidget *)box;
        if(tableWidgetBox->horizontalHeaderItem(0)->text() == "INDEX")
        {
            qInfo()<<QObject::tr("数组");
            QJsonArray jsonArray;
            for (int row = 0; row < tableWidgetBox->rowCount(); row++)
            {
                QWidget *childBox = tableWidgetBox->cellWidget(row,1);
                QJsonObject currentObj = getJsonDataFormBox(0,childBox);
                QJsonObject::iterator it = currentObj.begin();
                while (it !=currentObj.end())
                {
                    jsonArray.append(it.value());
                    it++;
                }
            }
            Obj.insert(QString::number(id),QJsonValue(jsonArray));
        }
        else if(tableWidgetBox->horizontalHeaderItem(0)->text() == "ID")
        {
            qInfo()<<QObject::tr("表格");
            QJsonObject tableObj;
            for (int row = 0; row < tableWidgetBox->rowCount(); row++)
            {
                int childId = tableWidgetBox->item(row,0)->text().toInt();
                QWidget *childBox = tableWidgetBox->cellWidget(row,2);
                QJsonObject currentObj = getJsonDataFormBox(childId,childBox);
                QJsonObject::iterator it = currentObj.begin();
                while (it !=currentObj.end())
                {
                    tableObj.insert(it.key(),it.value());
                    it++;
                }
            }
            Obj.insert(QString::number(id),QJsonValue(tableObj));
        }
    }
    return Obj;
}

/**************************************************************************
** 功能	@brief :   发送选中列表的物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QByteArray modelProcess::sendModelData(QTableWidget * tableWidget,bool mode)
{
    qDebug()<<__FUNCTION__;
    QByteArray data;
    QList<int> reportList = this->getListSelectedRows(tableWidget);
    qDebug()<<reportList<<"count:"<<reportList.count();
    int reportRow;
    QJsonObject Obj;
    foreach (reportRow, reportList)
    {
        int id = tableWidget->item(reportRow,0)->text().toInt();
        QWidget *box = new QWidget();
        if(tableWidget->columnCount() == 3)
            box = tableWidget->cellWidget(reportRow,2);
        else if(tableWidget->columnCount() == 2)
            box = tableWidget->cellWidget(reportRow,1);
        qDebug()<<"id:"<<QString::number(id)<<box->metaObject()->className();
        if(mode == TTLVMODESNED)
            data.append(getDataFormBox(id,box));
        else
        {
            QJsonObject currentObj = getJsonDataFormBox(id,box);
            QJsonObject::iterator it = currentObj.begin();
            while (it !=currentObj.end())
            {
//                qDebug()<<"it.key():"<<it.key()<<","<<it.value();
                Obj.insert(it.key(),it.value());
                it++;
            }
        }
    }
    qDebug()<<"sendModelData data:"<<data;
    if(mode == JSONMODESEND)
    {
        QJsonDocument document;
        document.setObject(Obj);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);
        QString strJson(byteArray);
        if(!byteArray.isEmpty())
        {
            qDebug()<<"json data:"<<strJson;
            return strJson.toUtf8();
        }
    }
    else
    {

        qDebug()<<data.toHex();
        return data;
    }
    return NULL;
}

/**************************************************************************
** 功能	@brief :   读取列表的物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QByteArray modelProcess::readModelData(QList<QByteArray> idList,QTableWidget * tableWidget,bool mode)
{
    qDebug()<<__FUNCTION__;
    QByteArray data;
    QByteArray idByte;
    if(idList.count() == 0)
    {
        QList<int> reportList = this->getListSelectedRows(tableWidget);
        int reportRow;
        foreach (reportRow, reportList)
        {
            idList.append(tableWidget->item(reportRow,0)->text().toUtf8());
        }
    }
    QJsonObject Obj;
    foreach (idByte, idList)
    {
        for (int row = 0; row < tableWidget->rowCount(); row++)
        {
            int id = tableWidget->item(row,0)->text().toInt();
            qDebug()<<"==========id:"<<id;
            if(id == idByte.toInt())
            {
                QWidget *box;
                if(tableWidget->columnCount() == 3)
                    box = tableWidget->cellWidget(row,2);
                else if(tableWidget->columnCount() == 2)
                    box = tableWidget->cellWidget(row,1);
                if(mode == TTLVMODESNED)
                {
                    data.append(getDataFormBox(id,box));
                }
                else
                {
                    QJsonObject currentObj = getJsonDataFormBox(id,box);
                    QJsonObject::iterator it = currentObj.begin();
                    while (it !=currentObj.end())
                    {
                        Obj.insert(it.key(),it.value());
                        it++;
                    }
                }
                break;
            }
        }

    }
//    qDebug()<<"Obj.count():"<<Obj.count();
    if(mode == JSONMODESEND)
    {
        QJsonDocument document;
        document.setObject(Obj);
        QByteArray byteArray = document.toJson(QJsonDocument::Compact);
        QString strJson(byteArray);
        if(!byteArray.isEmpty() && Obj.count() > 0)
        {
//            qDebug()<<"json data:"<<strJson;
            return strJson.toUtf8();
        }
    }

    return data;
}

/**************************************************************************
** 功能	@brief :   读取列表的物模型数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void modelProcess::readModelId(QList<QByteArray> idList,QTableWidget * tableWidget)
{
    if(idList.count() == 0)
        qWarning()<<"idList为空";
    for (int row = 0; row < tableWidget->rowCount(); row++)
    {
        if(tableWidget->item(row,0)->text() == idList.at(0))
        {
            tableWidget->selectRow(row);
            break;
        }
    }
}

