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
#include "otawindow.h"
#include "ui_otawindow.h"
#include "baseQTConfigure.h"
#include <QFileDialog>
#include <QThread>
#include "kernel/mmessagebox.h"

otaWindow::otaWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::otaWindow)
{
    ui->setupUi(this);
    ui->tableWidget_otaMcuVer->setSelectionBehavior ( QAbstractItemView::SelectRows);
    this->otafile = new fileManager();
    sotaStartAdds = 0;
    ui->tableWidget_otaMcuVer->verticalHeader()->setDefaultSectionSize(40);
    ui->tableWidget_otaMcuVer->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_otaMcuVer->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->tableWidget_otaMcuVer->setShowGrid(false);
    ui->tableWidget_otaMcuVer->horizontalHeader()->setDefaultAlignment(Qt::AlignVCenter);
    ui->tableWidget_otaMcuVer->verticalHeader()->setStyleSheet("QHeaderView::section{ \
                                                            color: rgb(104, 112, 130); \
                                                            font-size:16px; \
                                                            background: transparent; \
                                                            background: rgb(246, 249, 255); \
                                                            padding: 1px;border: none; \
                                                            border-style:solid; \
                                                            border-right-width:0px; \
                                                            border-left-width:0px; \
                                                            }");
    ui->tableWidget_otaMcuVer->horizontalHeader()->setStyleSheet("QHeaderView::section{ \
                                                              color: rgb(104, 112, 130); \
                                                              font-size:16px; \
                                                              background: transparent; \
                                                              background: rgb(246, 249, 255); \
                                                              padding: 1px;border: none; \
                                                              border-style:solid; \
                                                              border-right-width:0px; \
                                                              border-left-width:0px; \
                                                              }");
    ui->tableWidget_otaMcuVer->setStyleSheet("QTableWidget{ \
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

otaWindow::~otaWindow()
{
    delete ui;
}
/**************************************************************************
** 功能	@brief : 设置父和界面属性 （注：在类实例化化后实现）
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::setParm(void *father)
{
    this->father = (MainWindow*)father;
    QJsonValue mode = this->father->userConfigFile->readJsonFile("ota_communite_mode");
    ui->tabWidget->setCurrentIndex(mode.toInt());
    uiAttributeSet();
    ui->splitter->setStretchFactor(0, 80);//设置拉伸优先级
    ui->splitter->setStretchFactor(1, 20);

}

void otaWindow::uiAttributeSet()
{
    ui->lineEdit_http_pk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
    ui->lineEdit_http_ps->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));

//    ui->lineEdit_http_url->setInputMask("000.000.000.000:00000;");
    ui->spinBox_http_ver->setValue(1);
    ui->spinBox_http_ver->setEnabled(false);
    ui->spinBox_http_power->setRange(0,100);
    ui->tabWidget->setCurrentIndex(0);

    // 暂不做界面优化
    //    ui->page_http_server->setGeometry(ui->page_http_server->x(),ui->page_http_server->y(),ui->page_http_server->width(),ui->page_http_server->height()-30);
}

/**************************************************************************
** 功能	@brief : 拒绝升级
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaRefused_clicked()
{
    this->father->ATCMDSend("AT+QIOTUPDATE=0");
}
/**************************************************************************
** 功能	@brief : 确认升级
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaConfirm_clicked()
{
    this->father->ATCMDSend("AT+QIOTUPDATE=1");
}
/**************************************************************************
** 功能	@brief : 下载下一块
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaDownPiece_clicked()
{
   this->father->ATCMDSend("AT+QIOTUPDATE=2");
}
/**************************************************************************
** 功能	@brief : 更新中
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaReportStatus_clicked()
{
    this->father->ATCMDSend("AT+QIOTUPDATE=3");
}
/**************************************************************************
** 功能	@brief : sota读取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_sotaRead_clicked()
{
    this->father->ATCMDSend("AT+QIOTOTARD="+QString::number(ui->spinBox_sotaStartAddr->value())+","+QString::number(ui->spinBox_sotaEndAddr->value()));
}
/**************************************************************************
** 功能	@brief : mcu信息获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaMcuListGet_clicked()
{
    this->father->ATCMDSend("AT+QIOTMCUVER?");
}
/**************************************************************************
** 功能	@brief : mcu版本获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaMcuVerGet_clicked()
{
    qInfo()<<__FUNCTION__;
    int index = ui->tableWidget_otaMcuVer->currentRow();
    if(index == -1)
    {
        MMessageBox::information(this,tr("提示"),tr("请先选择MCU模型"),tr("确认"));
    }
    else
    {
        this->father->ATCMDSend("AT+QIOTMCUVER=\""+ui->tableWidget_otaMcuVer->item(index,0)->text()+"\"");
    }
}
/**************************************************************************
** 功能	@brief : mcu版本设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaMcuVerSet_clicked()
{
    qInfo()<<__FUNCTION__;
    QModelIndexList selected = ui->tableWidget_otaMcuVer->selectionModel()->selectedRows();
    QList<int>selectRowNum;
    for(int i = 0;i<selected.count();i++)
        selectRowNum.append(selected[i].row());
    std::sort(selectRowNum.begin(),selectRowNum.end());
    if(selectRowNum.count() == 0)
    {
        MMessageBox::information(this,tr("提示"),tr("请先选择MCU模型"),tr("确认"));
    }
    else
    {
        for(int i =0;i<selectRowNum.count();i++)
        {
            QString mcu_no,mcu_ver;
            mcu_no.append(ui->tableWidget_otaMcuVer->item(selectRowNum[i],0)->text());
            mcu_ver.append(ui->tableWidget_otaMcuVer->item(selectRowNum[i],1)->text());
            if(mcu_no.contains(QRegExp("[\\x4e00-\\x9fa5]+")) || mcu_ver.contains(QRegExp("[\\x4e00-\\x9fa5]+")))
            {
                MMessageBox::critical(this,tr("错误"),tr("标识或版本号不可包含中文"),tr("确认"));
                return;
            }
            this->father->ATCMDSend("AT+QIOTMCUVER=\""+mcu_no+"\",\""+mcu_ver+"\"");
        }
    }
}
/**************************************************************************
** 功能	@brief : 添加mcu
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaMcuVerAdd_clicked()
{
    qInfo()<<__FUNCTION__;
    int index = ui->tableWidget_otaMcuVer->rowCount()+1;
    ui->tableWidget_otaMcuVer->setRowCount(index);
    ui->tableWidget_otaMcuVer->setItem(index-1,0,new QTableWidgetItem());
    ui->tableWidget_otaMcuVer->setItem(index-1,1,new QTableWidgetItem());
}
/**************************************************************************
** 功能	@brief : 删除mcu
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaMcuVerDel_clicked()
{
    qInfo()<<__FUNCTION__;
    if(ui->tableWidget_otaMcuVer->currentRow() >= 0)
    {
        QModelIndexList selected = ui->tableWidget_otaMcuVer->selectionModel()->selectedRows();
        QList<int>selectRowNum;
        for(int i = 0;i<selected.count();i++)
            selectRowNum.append(selected[i].row());
        std::sort(selectRowNum.begin(),selectRowNum.end());
        for(int i =selectRowNum.count()-1;i>=0;i--)
        {
            QString mcuNo = ui->tableWidget_otaMcuVer->item(selectRowNum[i],0)->text();
            if(!mcuNo.isEmpty())
                this->father->ATCMDSend("AT+QIOTMCUVER=\""+mcuNo+"\",\"\"");
            ui->tableWidget_otaMcuVer->removeRow(selectRowNum[i]);
        }
    }
}
/**************************************************************************
** 功能	@brief : 选择ota固件储存位置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_otaFile_clicked()
{
    qInfo()<<__FUNCTION__;
    this->otafile = new fileManager();
    QString fileName ="queciot_sota_"+QDateTime::currentDateTime().toString("yyMMdd_hhmmss")+".bin";
    QString file = QFileDialog::getSaveFileName(0,tr("选择固件存储"),fileName,"*.bin");
    if(this->otafile->fileManagerOpen(file,FileKeepMode) == false)
    {
        MMessageBox::warning(this,tr("警告"),tr("固件存储路径创建失败"),tr("确认"));
        return;
    }
    ui->lineEdit_otaPath->setText(file);
    ui->lineEdit_otaPath->setToolTip(file);

}

/**************************************************************************
** 功能	@brief : mcu版本获取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::csdkMcuVerGet(QList<QByteArray> paraList)
{
    qInfo()<<__FUNCTION__;
    ui->tableWidget_otaMcuVer->setRowCount(paraList.count()/2);
    for(int i=0;i<paraList.count()/2;i++)
    {
        QString key = QString(paraList.at(2*i)).replace("\"","");
        QString value = QString(paraList.at(2*i+1)).replace("\"","");
        ui->tableWidget_otaMcuVer->setItem(i,0,new QTableWidgetItem(key));
        ui->tableWidget_otaMcuVer->setItem(i,1,new QTableWidgetItem(value));
    }
}
/**************************************************************************
** 功能	@brief : mcu版本设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::csdkMcuVerSet(QList<QByteArray> paraList)
{
    qInfo()<<__FUNCTION__;
    QByteArray para;
    QStringList strParaList;
    foreach (para, paraList)
    {
        strParaList.append(para.replace("\"",""));
    }
    for(int i=0;i<ui->tableWidget_otaMcuVer->rowCount();i++)
    {
        if(ui->tableWidget_otaMcuVer->item(i,0)->text() == strParaList.at(0))
        {
            ui->tableWidget_otaMcuVer->setItem(i,1,new QTableWidgetItem(strParaList.at(1)));
        }
    }
    ui->tableWidget_otaMcuVer->update();
}
/**************************************************************************
** 功能	@brief : ota流程处理
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::csdkOtaEventProcess(QList<QByteArray> paraList)
{
    qInfo()<<__FUNCTION__;
    int code = paraList.at(0).toInt();
    switch (code)
    {
    case 10700:
        otaType = FOTA_UPDATA;
        ui->comboBox_otaStatus->setCurrentIndex(1);
        if(paraList.count() >= 7)
        {
            ui->spinBox_sotaFileSize->setValue(paraList.at(6).toInt());
        }
        break;
    case 10701:
    {
        if(this->otafile->fileInfo.myFile.isOpen())
        {
            this->otafile->fileInfo.myFile.close();
            this->otafile->fileInfo.myFile.open(QIODevice::Truncate);
            this->otafile->fileInfo.myFile.close();
            this->otafile->fileInfo.myFile.open(QIODevice::WriteOnly);
        }
        ui->comboBox_otaStatus->setCurrentIndex(2);
        if(paraList.count() >= 4)
        {
            otaType = SOTA_UPDATA;
            ui->spinBox_sotaFileSize->setValue(paraList.at(2).toInt());
            ui->lineEdit_sotaMd5->setText(QByteArray(paraList.at(3)).replace("\"",""));
        }
        else
        {
            otaType = FOTA_UPDATA;
        }
        break;
    }
    case 10702:
        ui->comboBox_otaStatus->setCurrentIndex(3);
        break;
    case 10703:
        ui->comboBox_otaStatus->setCurrentIndex(4);
        if(paraList.count() >= 5)
        {
            ui->spinBox_sotaFileSize->setValue(paraList.at(2).toInt());
            ui->spinBox_sotaBlockStart->setValue(paraList.at(3).toInt());
            ui->spinBox_sotaBlockSize->setValue(paraList.at(4).toInt()); 
        }
        if(ui->spinBox_sotaStartAddr->value() == 0 && otaType == SOTA_UPDATA)
        {
            sotaStartAdds = paraList.at(3).toInt();
        }
        if(ui->checkBox_sotaAuto->isChecked() && otaType == SOTA_UPDATA)
        {
            on_pushButton_sotaRead_clicked();
        }
        break;
    case 10704:
        ui->comboBox_otaStatus->setCurrentIndex(5);
        break;
    case 10705:
        ui->comboBox_otaStatus->setCurrentIndex(6);        
        sotaStartAdds = 0;
        break;
    case 10706:
        ui->comboBox_otaStatus->setCurrentIndex(7);
        ui->spinBox_sotaStartAddr->setValue(0);
        ui->spinBox_sotaBlockStart->setValue(0);
        ui->spinBox_sotaBlockSize->setValue(0);
        sotaStartAdds = 0;
        break;
    default:
        break;
    }
}
/**************************************************************************
** 功能	@brief : 已下载的固件读取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::csdkOtaFileRead(int startAddr,int len,QByteArray data)
{
    qInfo()<<__FUNCTION__;
    qDebug()<<"已接收长度"<<startAddr+len;
    ui->spinBox_sotaStartAddr->setValue(startAddr+len);
    if(ui->checkBox_sotaAuto->isChecked())
    {
        if(this->otafile->fileManagerWrite(startAddr,len,data,FileKeepMode) == 0)
        {
            MMessageBox::critical(this,tr("错误"),tr("文件写入失败，请重试"),tr("确认"));
            ui->spinBox_sotaStartAddr->setValue(0);
            ui->checkBox_sotaAuto->setChecked(false);
            return;
        }
        if(ui->spinBox_sotaStartAddr->value() < ui->spinBox_sotaFileSize->value())
        {
            if(ui->spinBox_sotaStartAddr->value() < ui->spinBox_sotaBlockStart->value() + ui->spinBox_sotaBlockSize->value())
            {
                on_pushButton_sotaRead_clicked();
            }
            else
            {
                on_pushButton_otaDownPiece_clicked();
            }

        }
        else
        {
            this->otafile->fileManagerClose();
            if(this->otafile->fileManagerGetMd5(ui->lineEdit_otaPath->text()).toHex() == ui->lineEdit_sotaMd5->text())
            {
                ui->comboBox_otaStatus->setCurrentIndex(8);
                clearValue();
            }
            else
            {
                ui->comboBox_otaStatus->setCurrentIndex(9);
                clearValue();
            }
        }
    }
}
/**************************************************************************
** 功能	@brief : 清空界面状态
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::clearValue()
{
    ui->spinBox_sotaStartAddr->setValue(0);
    ui->spinBox_sotaBlockStart->setValue(0);
    ui->spinBox_sotaBlockSize->setValue(0);
    ui->spinBox_sotaFileSize->setValue(0);
    ui->lineEdit_otaPath->clear();
    ui->lineEdit_sotaMd5->clear();
    ui->checkBox_sotaAuto->setChecked(false);
    ui->pushButton_otaFile->setEnabled(true);

}

void otaWindow::resetUIValue()
{
    clearValue();
    ui->tableWidget_otaMcuVer->setRowCount(0);
    ui->spinBox_sotaEndAddr->setValue(1024);
}

bool otaWindow::retranslationUi()
{
    ui->retranslateUi(this);
    return true;
}
/**************************************************************************
** 功能	@brief : 设置自动读取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_checkBox_sotaAuto_toggled(bool checked)
{
    qInfo()<<__FUNCTION__;
    if(checked)
    {
        if(ui->lineEdit_otaPath->text().isEmpty())
        {
            MMessageBox::information(this,tr("提示"),tr("请先创建SOTA固件存储路径"),tr("确认"));
            ui->checkBox_sotaAuto->setChecked(false);
            return;
        }
        ui->pushButton_sotaRead->setEnabled(false);
        ui->pushButton_otaFile->setEnabled(false);
    }
    else
    {
        ui->pushButton_sotaRead->setEnabled(true);
        ui->pushButton_otaFile->setEnabled(true);
    }
}
/**************************************************************************
** 功能	@brief : ota状态改变处理
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_comboBox_otaStatus_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        ui->checkBox_sotaAuto->setChecked(false);
        ui->lineEdit_otaPath->clear();
        ui->spinBox_sotaStartAddr->setValue(0);
        ui->spinBox_sotaBlockStart->setValue(0);
        ui->spinBox_sotaBlockSize->setValue(0);
        ui->spinBox_sotaFileSize->setValue(0);
        ui->lineEdit_sotaMd5->clear();
        break;
    default:
        break;
    }
}
/**************************************************************************
** 功能	@brief : mcu版本设置分析
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_tableWidget_otaMcuVer_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);
    if(current != NULL &&current->text().contains(QRegExp("[\\x4e00-\\x9fa5]+")))
    {
        MMessageBox::critical(this,tr("错误"),tr("标识或版本号不可包含中文"),tr("确认"));
        current->setText("");
        ui->tableWidget_otaMcuVer->setItem(current->row(),current->column(),current);
    }
}
/**************************************************************************
** 功能	@brief : ota请求
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_OTARequest_clicked()
{
    qInfo()<<__FUNCTION__<<this->father->userconfigStatus(GETUSERINFO,SOCKETSTATUS) <<this->father->userconfigStatus(GETUSERINFO,MQTTSTATUS);
    bool mode = this->father->userconfigStatus(GETUSERINFO,SOCKETSTATUS) && this->father->userconfigStatus(GETUSERINFO,MQTTSTATUS);
    if(mode == false)
    {
        MMessageBox::critical(this,tr("错误"),tr("串口、tcp或mqtt未连接"),tr("确认"));
        return;
    }
//    if(ui->tableWidget_otaMcuVer->currentRow() >= 0)
//    {
//        QModelIndexList selected = ui->tableWidget_otaMcuVer->selectionModel()->selectedRows();
//        QList<int>selectRowNum;
//        for(int i = 0;i<selected.count();i++)
//            selectRowNum.append(selected[i].row());
//        qSort(selectRowNum.begin(),selectRowNum.end(), qGreater<int>());
//        for(int i =0;i<selectRowNum.count();i++)
//        {
//            QString data;
//            QString mcuNo = ui->tableWidget_otaMcuVer->item(selectRowNum[i],0)->text();
//            if(!mcuNo.isEmpty())
//                data.append("AT+QIOTOTAREQ=\""+mcuNo+"\"");
//            else
//                continue;
//            QString mcuVer = ui->tableWidget_otaMcuVer->item(selectRowNum[i],1)->text();
//            if(!mcuVer.isEmpty())
//                data.append(",\""+mcuVer+"\"");
//            else
//                continue;
//            if(ui->checkBox_extraMess->isChecked())
//            {
//                data.append(",1");
//            }
//            this->father->ATCMDSend(data);
//        }
//    }
//    else
//    {
//        MMessageBox::warning(this,tr("警告"),tr("请选择MCU"),"ok");
//    }
    QString data;
    data.append("AT+QIOTOTAREQ");
    if(ui->checkBox_extraMess->isChecked())
    {
        data.append("=1");
    }
    this->father->ATCMDSend(data);
}


/**************************************************************************
** 功能	@brief : httpOTA 计划请求
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_pushButton_http_ota_req_clicked()
{    
    QString data = "AT+QHOTAREQ="+QString::number(ui->spinBox_http_power->value())+","+QString::number(ui->spinBox_http_ver->value());
    QString pk = ui->lineEdit_http_pk->text();
    QString ps = ui->lineEdit_http_ps->text();
    if(pk.isEmpty()|| ps.isEmpty()|| ui->lineEdit_http_url->text().isEmpty())
    {
        MMessageBox::critical(this,tr("错误"),tr("信息框不能为空"),tr("确认"));//弹出弹窗关闭后，AT指令不会发出
    }
    else
    {
        data.append(",\""+pk+"\",\""+ps+"\"");
        QRegExp reg("[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}:[0-9]{1,5}");
        data.append(",\""+ui->lineEdit_http_url->text()+"\"");
        this->father->ATCMDSend(data);
    }
}
/**************************************************************************
** 功能	@brief : mqttOTA/httpOTA 切换界面信息保存
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void otaWindow::on_tabWidget_currentChanged(int index)
{
    QJsonValue value =  QJsonValue(index);
    this->father->userConfigFile->addJsonToFile("ota_communite_mode",&value);

}

void otaWindow::on_pushButton_mqtt_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
}

void otaWindow::on_pushButton_http_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}
