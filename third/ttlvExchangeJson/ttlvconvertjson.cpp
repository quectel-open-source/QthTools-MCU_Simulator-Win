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
#include "ttlvconvertjson.h"
#include "ui_ttlvconvertjson.h"
#include "kernel/mmessagebox.h"
#include "ttlvDataAnysis.h"

#include <QDebug>
#include <QLabel>
#include <QScrollBar>
#include <QTableView>
#include <math.h>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

enum
{
    TTLV_TO_JSON_MODE = 0,
    JSON_TO_TTLV_MODE
};

//enum
//{
//    CMD_GET_INFO = 0x0041,
//    CMD_REPORT_ACK = 0x0042,
//    CMD_REPORT = 0x0044,
//};

ttlvConvertJson::ttlvConvertJson(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ttlvConvertJson)
{
    ui->setupUi(this);
    uiAttributeSet();
    ui->radioButton_TJ->setChecked(true);
    currentChangeMode = TTLV_TO_JSON_MODE;

    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->textEdit_src->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    cmdString[CMD_GET_INFO] = QString("模组信息获取");
//    cmdString[CMD_REPORT_ACK] = QString("模组信息上报-回复");
//    cmdString[CMD_REPORT] = QString("模组信息上报");
    ui->tableWidget->setMinimumWidth(200);
    ui->tableWidget->setColumnWidth(3,200);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(100);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);//允许拉伸
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);//允许拉伸
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
//    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
    ui->tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              }");
    ui->tableWidget->setStyleSheet("QTableWidget{ \
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

    //去除窗体边框
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    //窗体关闭信号
    connect(ui->pushButton_close, &QPushButton::clicked, [=]() {close(); });
    connect(ui->pushButton_Maximize, &QPushButton::clicked, [=](){
                if (this->isMaximized())
                {
                    ui->pushButton_Maximize->setStyleSheet("QPushButton:hover{background-color: rgb(220, 220, 220);}QPushButton{image: url(:/png/MaximizeOff.png);border-width:0;border-style:outset;background-color:transparent;}");
                    QVBoxLayout *layout = this->findChild<QVBoxLayout *>();
                    layout->setContentsMargins(10,10,10,10);
                    this->showNormal();
                }
                else
                {
                    ui->pushButton_Maximize->setStyleSheet("QPushButton:hover{background-color: rgb(220, 220, 220);}QPushButton{image: url(:/png/MaximizeOn.png);border-width:0;border-style:outset;background-color:transparent;}");
                    QVBoxLayout *layout = this->findChild<QVBoxLayout *>();
                    layout->setContentsMargins(0,0,0,0);
                    this->showMaximized();
                }
                });
    connect(ui->pushButton_Minimize, &QPushButton::clicked, [=]() {this->showMinimized(); });
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#C0C0C0"));
    shadow->setBlurRadius(15);
    shadow->setOffset(5);
    ui->widget->setGraphicsEffect(shadow);
//    ui->pushButton_Minimize->hide();
//    ui->pushButton_Maximize->hide();
//    ui->pushButton_Maximize->setEnabled(false);
}

/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void ttlvConvertJson::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    //判断是否是鼠标左键信号，是否处于最大化状态
    if(event->button() == Qt::LeftButton &&  !this->isMaximized())
    {
        if (ui->widget_windowTitle->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        {
            m_bDrag = true;
            mouseStartPoint = event->globalPos();
            windowTopLeftPoint = this->frameGeometry().topLeft();
        }
    }
}

void ttlvConvertJson::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bDrag)
    {
        //获得鼠标移动的距离
        QPoint distance = event->globalPos() - mouseStartPoint;
        //QPoint distance = event->pos() - mouseStartPoint;
        //改变窗口的位置
        this->move(windowTopLeftPoint + distance);
    }
}

void ttlvConvertJson::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

ttlvConvertJson::~ttlvConvertJson()
{
    delete ui;
}

//static QByteArray intToByte(uint64_t number,int size)
//{
//    QByteArray dataByteArray;
//    uint8_t byte;
//    if(size > 8)
//    {
//        return dataByteArray;
//    }
//    for(int i = 0;i<size;i++)
//    {
//        byte = (number >> (8 * i)) & 0xFF;
//        dataByteArray.insert(0,byte);
//    }
//    return dataByteArray;
//}

//static int getByteSize(uint64_t number)
//{
//    int size;
//    uint64_t numberFork = number;
//    int maxSize = 7;
//    while(1)
//    {
//        numberFork = (numberFork>>8)& pow(0XFF,maxSize--);
//        if(numberFork > 0)
//            size++;
//        else
//            break;
//    }
//    return size;
//}


void ttlvConvertJson::tj_dataFilter(QString src, QByteArray &dst)
{
    src = src.toUpper();
    src = src.simplified();
    QString squeezStr;
    QStringList srcList = src.split(" ");
    if(srcList.size() >= 2)
    {
        for(int i = 0;i<srcList.size();i++)
        {
            bool isNeedInsert = true;
            QString node = srcList.at(i);
            if(node.size() > 2 && (node.contains("0X",Qt::CaseInsensitive)))
            {
                QStringList nodeList = node.split("0X");
                for(int j =0;j<nodeList.count();j++)
                {
                    squeezStr.append(nodeList[j]);
                }
                isNeedInsert = false;
            }
            else if(node.size() == 1)
            {
                node.insert(0,"0");
            }
            if(isNeedInsert)
            {
                squeezStr.append(node);
            }
        }
    }
    if(squeezStr.isEmpty())
    {
        if(src.contains("0X"))
        {
            QStringList list = src.split("0X",QString::KeepEmptyParts,Qt::CaseInsensitive);
            for(int i = 0;i<list.size();i++)
            {
               QString node = list.at(i);
               squeezStr.append(node.mid(1));
            }
        }
        else
            squeezStr = src;
    }
    QString upStr = squeezStr.toUpper();
    qDebug()<<"upStr:"<<upStr;
    for(int i =0;i<upStr.size();i+=2)
    {
        int highByte =0 ,lowByte=0;
        if(((highByte = getInt(upStr[i].toLatin1())) != -1) && ((lowByte = getInt(upStr[i+1].toLatin1()))!= -1))
        {
            dst.append(highByte*16+lowByte);
        }
        else
        {
//            ui->statusbar->showMessage("explain ttlv data : data number error");
            break;
        }
    }

}

void ttlvConvertJson::explainJsonObject(QJsonObject srcObj,QByteArray &ttlvData)
{
    QString type;
    int id = -1;
//    if (srcObj.contains("id"))
//    {
//        QJsonValue value = srcObj.value("id");
//        if (value.isDouble()) {
//            id = value.toInt();
//        }
//        else
//        {
//            return;
//        }
//    }
//    if (srcObj.contains("type"))
//    {
//        QJsonValue value = srcObj.value("type");
//        if (value.isString()) {
//            type = value.toString();
//            qDebug() << "type : " << type;

//        }
//    }



    ttlvDataAnysis ttlvDataAnysisExample;
    QString key = QString(srcObj.keys().at(0));
    qDebug()<<"key:"<<key;
    if(key.contains(QRegExp("^\\d+$")))
    {
        id = key.toInt();
    }
    else
    {
        return;
    }
    QJsonValue value = srcObj.value(key);
    if(value.isBool())
    {
        bool dataValue = value.toBool();
        if(false == dataValue)
            ttlvData.append(ttlvDataAnysisExample.IntToByte((id<<3)&0xFFF8,2));
        else
           ttlvData.append(ttlvDataAnysisExample.IntToByte(((id<<3)&0xFFF8)| 0x01,2));
    }
    else if(value.isDouble())
    {
        double dataValue = value.toDouble();
        ttlvData.append(ttlvDataAnysisExample.IntToByte(((id<<3)&0xFFF8)| 0x02,2));
        // 判断正负
        bool isMinus = false;
        if(QString::number(dataValue).contains("-"))
        {
            isMinus = true;
        }
        // 判断小数点
        int decimalSum = 0;
        QStringList list =  QString::number(dataValue).split(".");
        if(list.size() == 2)
        {
            decimalSum = QString(list[1]).size();
        }
        qDebug()<<"decimalSum:"<<decimalSum;
        // 判断字节数
        if(decimalSum > 0)
        {
            dataValue = dataValue *pow(10,decimalSum);
        }
        qDebug()<<"dataValue:"<<dataValue;
        int byteSum = 0;
        int dataValueFork = abs(dataValue);
        while((dataValueFork = dataValueFork/256) > 0)
        {
            byteSum++;
        }
        qint8 dataLen = ((isMinus<<7) & 0x80) + ((decimalSum<<3) & 0x78) + (byteSum & 0x07);
        ttlvData.append(ttlvDataAnysisExample.IntToByte(dataLen & 0xFF,1));
        ttlvData.append(ttlvDataAnysisExample.IntToByte(dataValue,byteSum+1));
        qDebug()<<"explainJsonObject ttlvData:"<<ttlvData.toHex();
    }
    else if(value.isString())
    {
        ttlvData.append(ttlvDataAnysisExample.IntToByte(((id<<3)&0xFFF8)| 0x03,2));
        QString dataValue = value.toString();
        int stringSum = dataValue.size();
        ttlvData.append(ttlvDataAnysisExample.IntToByte(stringSum & 0xFFFF,2));
        ttlvData.append(dataValue.toLatin1());
    }
    else if(value.isObject())
    {
        ttlvData.append(ttlvDataAnysisExample.IntToByte(((id<<3)&0xFFF8)| 0x04,2));
        QJsonObject sonObject = value.toObject();
        QByteArray sonByteArray;
        explainJsonObject(sonObject,sonByteArray);
        if(!sonByteArray.isEmpty())
        {
            ttlvData.append(ttlvDataAnysisExample.IntToByte(1,2));
            ttlvData.append(sonByteArray);
        }
    }
    else if (value.isArray())
    {
        QJsonArray array = value.toArray();
        ttlvData.append(ttlvDataAnysisExample.IntToByte(((id<<3)&0xFFF8)| 0x04,2));
        ttlvData.append(ttlvDataAnysisExample.IntToByte(array.size()&0xFFFF,2));
        for(int i = 0;i<array.size();i++)
        {
            if(array.at(i).isObject())
            {
                QJsonObject sonObject = array.at(i).toObject();
                QByteArray sonByteArray;
                explainJsonObject(sonObject,sonByteArray);
                if(!sonByteArray.isEmpty())
                {
                    ttlvData.append(sonByteArray);
                }
            }

        }
    }
}

bool ttlvConvertJson::explainJsonArray(QJsonArray srcArray,QByteArray &ttlvData)
{
    bool ret = false;
    // 不支持多层数组嵌套
    int i = 0;
    for(i = 0;i<srcArray.size();i++)
    {
        if(srcArray.at(i).isObject())
        {
            QJsonObject sonObject = srcArray.at(i).toObject();
            QByteArray sonByteArray;
            explainJsonObject(sonObject,sonByteArray);
            if(!sonByteArray.isEmpty())
            {
                ttlvData.append(sonByteArray);
                ret = true;
            }
            else
            {
                break;
            }
        }
        else if(srcArray.at(i).isArray())
        {
            QJsonArray array = srcArray.at(i).toArray();
            explainJsonArray(array,ttlvData);
        }
        else
        {
            break;
        }
    }
    return ret;
}


void ttlvConvertJson::jt_dataFilter(QString src)
{
    QByteArray srcByteArray = src.toLatin1();
    QByteArray dstByteArray;
    bool handleFlag =false;
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(srcByteArray, &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {
        if (doucment.isObject())
        {
            QJsonObject object = doucment.object();
            explainJsonObject(object,dstByteArray);
            if(!dstByteArray.isEmpty())
            {
                handleFlag = true;
            }
        }
        else if(doucment.isArray())
        {
            QJsonArray array = doucment.array();
            if(explainJsonArray(array,dstByteArray))
            {
                handleFlag = true;
            }
        }

    }

    if( handleFlag )
    {
        ui->tableWidget->setRowCount(1);
        ui->tableWidget->verticalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
//        ui->tableWidget->verticalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
        qDebug()<<"dstByteArray:"<<dstByteArray;
        ui->tableWidget->setItem(0,0,new QTableWidgetItem(tr("json数据")));
        QTextEdit *textEdit = new QTextEdit();
        textEdit->setStyleSheet("color: rgb(104, 112, 130);\
                                   font-size:14px;\
                                   border-style:solid;\
                                   border-width:1px;\
                                   border-height：20px;\
                                   border-color:rgb(229, 231, 234);\
                                   padding-left:5px;\
                                   padding-right:5px;\
                                   ");
        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        textEdit->setText(src);
        ui->tableWidget->setCellWidget(0,1,(QWidget *)textEdit);

        QTextEdit *ttlvTextEdit = new QTextEdit();
        ttlvTextEdit->setStyleSheet("color: rgb(104, 112, 130);\
                                   font-size:14px;\
                                   border-style:solid;\
                                   border-width:1px;\
                                   border-height：20px;\
                                   border-color:rgb(229, 231, 234);\
                                   padding-left:5px;\
                                   padding-right:5px;\
                                   ");
        ttlvTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ttlvTextEdit->setText(QString(byteArraySplitLine(dstByteArray.toHex())));
        ui->tableWidget->setCellWidget(0,2,(QWidget *)ttlvTextEdit);
//        qDebug()<<"byteArraySplitLine(dstByteArray.toHex()):"<<byteArraySplitLine(dstByteArray.toHex());
//        ui->tableWidget->setItem(0,2,new QTableWidgetItem(QString(byteArraySplitLine(dstByteArray.toHex()))));
        ui->tableWidget->setItem(0,3,new QTableWidgetItem(tr("转换为ttlv数据")));
    }
    else
    {
        MMessageBox::critical(NULL,tr("错误"),tr("json数据解析错误"),tr("确认"));
    }
}

int ttlvConvertJson::getInt(char asciiNumber)
{
    if(asciiNumber >= '0' && asciiNumber <= '9')
    {
        return (asciiNumber-'0');
    }
    else if(asciiNumber >= 'A' && asciiNumber <= 'F')
    {
        return asciiNumber-'A'+10;
    }
    else
    {
        return -1;
    }
}




void ttlvConvertJson::on_radioButton_TJ_clicked()
{
    qDebug()<<"ttlv to json";
    if(TTLV_TO_JSON_MODE != currentChangeMode)
    {
        jsonWidgetSrcHistoryData = ui->textEdit_src->toPlainText();
        ui->tableWidget->clearContents();
        currentChangeMode = TTLV_TO_JSON_MODE;
        if(!ttlvWidgetSrcHistoryData.isEmpty())
        {
            ui->textEdit_src->setText(ttlvWidgetSrcHistoryData);
        }
        else
        {
            ui->textEdit_src->clear();
        }

    }
}

void ttlvConvertJson::on_radioButton_JT_clicked()
{
    qDebug()<<"json to ttlv";
    if(JSON_TO_TTLV_MODE != currentChangeMode)
    {
        ui->tableWidget->clearContents();
        ttlvWidgetSrcHistoryData = ui->textEdit_src->toPlainText();
        currentChangeMode = JSON_TO_TTLV_MODE;
        if(!jsonWidgetSrcHistoryData.isEmpty())
        {
            ui->textEdit_src->setText(jsonWidgetSrcHistoryData);
        }
        else
        {
            ui->textEdit_src->clear();
        }
    }
}




void ttlvConvertJson::on_pushButton_change_clicked()
{
    QString text = ui->textEdit_src->toPlainText();
    if(TTLV_TO_JSON_MODE == currentChangeMode)
    {
        QByteArray dst;
        tj_dataFilter(text,dst);
        if(!dst.isEmpty())
        {
            qDebug()<<"dst:"<<dst;
            ui->tableWidget->clearContents();
            anlyzeTtlvData(dst);
//                ui->textEdit_dst->clear();
//                ttlvAnylsis *test = new ttlvAnylsis();
//                connect(test,SIGNAL(sendTJResult(QString)),this,SLOT(recvTJResult(QString)));
//                test->anlyzeTtlvData(dst);
        }
        else
        {
            MMessageBox::critical(NULL,tr("错误"),tr("ttlv数据解析出错"),tr("确认"));
//            ui->statusbar->clearMessage();
        }
    }
    else if (JSON_TO_TTLV_MODE == currentChangeMode)
    {
        jt_dataFilter(text);
    }
}


void ttlvConvertJson::anlyzeTtlvData(QByteArray data)
{
    int offset = 0;
    int rowCount = 0;
    int ret;
    bool analysisResult = true;
    QJsonArray array;
    qDebug()<<"anlyzeTtlvData data:"<<data;
    qDebug()<<"data.toHex():"<<data.toHex();
    ttlvDataAnysis *test = new ttlvDataAnysis();
    int dataType;
    if(TYPE_HAVE_PROTOCOL_HEAD == (dataType=test->anlyzeTtlvType(data)))
    {
        bool ok;
        ui->tableWidget->setRowCount(6);
        ui->tableWidget->verticalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
        ui->tableWidget->setItem(0,0,new QTableWidgetItem(tr("协议头")));
        ui->tableWidget->setItem(0,1,new QTableWidgetItem(QString(data.mid(0,2).toHex())));

        ui->tableWidget->setItem(0,2,new QTableWidgetItem("0x"+QString(data.mid(0,2).toHex())));
        ui->tableWidget->setItem(0,3,new QTableWidgetItem(tr("固定")));

        ui->tableWidget->setItem(1,0,new QTableWidgetItem(tr("数据域长度")));
        ui->tableWidget->setItem(1,1,new QTableWidgetItem(QString(data.mid(2,2).toHex())));
        int dataLen = (data.mid(2,1).toHex().toInt(&ok,16)<<8) +data.mid(3,1).toHex().toInt(&ok,16);
        int realDataLen = data.mid(4).size();
        if(realDataLen != dataLen || (realDataLen > 65535 || realDataLen < 5) || (dataLen > 65535 || dataLen < 5))
        {
            MMessageBox::critical(NULL,tr("错误"),tr("ttlv数据长度错误"),tr("确认"));
            ui->tableWidget->clearContents();
            return;
        }
        ui->tableWidget->setItem(1,2,new QTableWidgetItem(QString::number(dataLen)));
        ui->tableWidget->setItem(1,3,new QTableWidgetItem(tr("校验和到数据域的字节数")));

        ui->tableWidget->setItem(2,0,new QTableWidgetItem(tr("校验和")));
        ui->tableWidget->setItem(2,1,new QTableWidgetItem(QString(data.mid(4,1).toHex())));
        uint8_t dataSum = data.mid(4,1).toHex().toInt(&ok,16);
        uint8_t realDataSum = test->dataSum(data.mid(5));
        ui->tableWidget->setItem(2,2,new QTableWidgetItem(QString::number(dataSum)));
        if(dataSum != realDataSum)
        {
            MMessageBox::critical(NULL,tr("错误"),tr("ttlv数据校验和错误"),tr("确认"));
            ui->tableWidget->clearContents();
            return;
        }
        ui->tableWidget->setItem(2,3,new QTableWidgetItem(tr("包id到数据域的字节数")));

        ui->tableWidget->setItem(3,0,new QTableWidgetItem(tr("包Id")));
        ui->tableWidget->setItem(3,1,new QTableWidgetItem(QString(data.mid(5,2).toHex())));
        int dataId = (data.mid(5,1).toHex().toInt(&ok,16)<<8) + data.mid(6,1).toHex().toInt(&ok,16);
        if(dataId == 0x0000 || dataId == 0xffff)
        {
            MMessageBox::critical(NULL,tr("错误"),tr("ttlv数据id错误"),tr("确认"));
            ui->tableWidget->clearContents();
            return;
        }
        ui->tableWidget->setItem(3,2,new QTableWidgetItem(QString::number(dataId)));
        ui->tableWidget->setItem(3,3,new QTableWidgetItem(tr("正确")));

        ui->tableWidget->setItem(4,0,new QTableWidgetItem(tr("命令字")));
        ui->tableWidget->setItem(4,1,new QTableWidgetItem(QString(data.mid(7,2).toHex())));
        int cmd = (data.mid(7,1).toHex().toInt(&ok,16)<<8) +data.mid(8,1).toHex().toInt(&ok,16);
        ui->tableWidget->setItem(4,2,new QTableWidgetItem(QString::number(cmd)));
        ui->tableWidget->setItem(4,3,new QTableWidgetItem(test->getMapString(cmd)));

        data = data.mid(9);
    }
    else
    {
        ui->tableWidget->setRowCount(1);
        ui->tableWidget->verticalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);     //然后设置要根据内容使用宽度的列
    }

    do
    {
        QJsonObject obj;
        ret = test->analyzeTtlvDataNode(data,&offset,obj);
        qDebug()<<"offset:"<<offset;
        if(ret == false)
        {
            MMessageBox::critical(NULL,tr("错误"),tr("ttlv数据解析错误"),"确认");
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

           array.append(obj);
#endif
        }

//        else if(ret == -1)
//            return ret;
    }while(offset<data.length());
    if(analysisResult)
    {
        QJsonDocument document;
        if(array.size()>1)
        {
            document.setArray(array);
        }
        else
        {
            document.setObject(array[0].toObject());

        }
        QByteArray byteArray = document.toJson(QJsonDocument::Indented);
        if(TYPE_HAVE_PROTOCOL_HEAD == dataType)
        {
            ui->tableWidget->setItem(5,0,new QTableWidgetItem(tr("ttlv数据")));
            ui->tableWidget->setItem(5,1,new QTableWidgetItem(QString(byteArraySplitLine(data.toHex()))));
            QTextEdit *textEdit = new QTextEdit();
            textEdit->setStyleSheet("color: rgb(104, 112, 130);\
                                       font-size:14px;\
                                       border-style:solid;\
                                       border-width:1px;\
                                       border-height：20px;\
                                       border-color:rgb(229, 231, 234);\
                                       padding-left:5px;\
                                       padding-right:5px;\
                                       ");
            textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            textEdit->setText(QString(byteArray));
            ui->tableWidget->setCellWidget(5,2,(QWidget *)textEdit);
            ui->tableWidget->setItem(5,3,new QTableWidgetItem(tr("解析为json的数据")));
            ui->tableWidget->resizeRowsToContents();
            ui->tableWidget->update();
        }
        else
        {
            ui->tableWidget->setItem(0,0,new QTableWidgetItem(tr("ttlv数据")));
            QTextEdit *srcTextEdit = new QTextEdit();
            srcTextEdit->setStyleSheet("color: rgb(104, 112, 130);\
                                       font-size:14px;\
                                       border-style:solid;\
                                       border-width:1px;\
                                       border-height：20px;\
                                       border-color:rgb(229, 231, 234);\
                                       border-radius:4px;\
                                       padding-left:5px;\
                                       padding-right:5px;\
                                       ");
            srcTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            srcTextEdit->setText(byteArraySplitLine(data.toHex()));
            ui->tableWidget->setCellWidget(0,1,(QWidget *)srcTextEdit);
            QTextEdit *dstTextEdit = new QTextEdit();
            dstTextEdit->setStyleSheet("color: rgb(104, 112, 130);\
                                       font-size:14px;\
                                       border-style:solid;\
                                       border-width:1px;\
                                       border-height：20px;\
                                       border-color:rgb(229, 231, 234);\
                                       padding-left:5px;\
                                       padding-right:5px;\
                                       ");
            dstTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            dstTextEdit->setText(QString(byteArray));
            ui->tableWidget->setCellWidget(0,2,(QWidget *)dstTextEdit);
            ui->tableWidget->setItem(0,3,new QTableWidgetItem(tr("解析为json的数据")));
            ui->tableWidget->resizeRowsToContents();
            ui->tableWidget->update();
        }
    }

}

QByteArray ttlvConvertJson::byteArraySplitLine(QByteArray data)
{
    QByteArray newByteArray = data;
    if(data.size()>20)
    {
        for(int i = 0;i<data.size();i+=20)
        {
            newByteArray.insert(i,"\n");
        }
    }
    return newByteArray;
}

void ttlvConvertJson::retranslationUi()
{
    ui->retranslateUi(this);
    uiAttributeSet();
}

void ttlvConvertJson::uiAttributeSet()
{
    this->setWindowTitle(tr("TTLV - JSON 界面"));
}
