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
#include "subdevcreate.h"
#include "ui_subdevcreate.h"
#include "kernel/mmessagebox.h"
#include "kernel/filemanager.h"
#include <QFileInfo>
#include "kernel/jsonFile.h"
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>
#include <QListView>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDirIterator>

subDevCreate::subDevCreate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::subDevCreate)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("子设备信息设置"));
    //去除窗体边框
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    //窗体关闭信号
    connect(ui->pushButton_close_2, &QPushButton::clicked, [=]() {close(); });
    this->setWindowModality(Qt::ApplicationModal);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#C0C0C0"));
    shadow->setBlurRadius(15);
    shadow->setOffset(5);
    ui->widget->setGraphicsEffect(shadow);

    ui->lineEdit_subDev_pk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
    ui->lineEdit_subDev_ps->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
    ui->lineEdit_subDev_dk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
    ui->lineEdit_subDev_ds->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
    ui->spinBox_subDev_tls->setRange(0,1);
    ui->spinBox_subDev_lifetime->setRange(60,65534);
    ui->comboBox_productMode->setCurrentIndex(0);
    ui->label_model->hide();
    ui->lineEdit_file->hide();
    ui->lineEdit_getFile->hide();
    ui->pushButton_file->hide();
    ui->comboBox_environment->hide();
    ui->pushButton_getFile->hide();
    ui->checkBox_automatic->hide();

//    ui->pushButton_selectPath->hide();

    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->comboBox_productMode->setView(new QListView());
    ui->comboBox_environment->setView(new QListView());

    httpClient = new HttpClient();
}

//subDevCreate::subDevCreate(bool *isValid, QWidget *parent):
//    QWidget(parent),
//    ui(new Ui::subDevCreate)
//{
//    ui->setupUi(this);
//    this->setWindowTitle(tr("子设备信息设置"));
//    ui->lineEdit_subDev_pk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
//    ui->lineEdit_subDev_ps->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
//    ui->lineEdit_subDev_dk->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{16}")));
//    ui->lineEdit_subDev_ds->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}")));
//    ui->spinBox_subDev_tls->setRange(0,1);
//    ui->spinBox_subDev_lifetime->setRange(60,65534);
//    ui->lineEdit_file->hide();
//    ui->pushButton_selectPath->hide();
//}

subDevCreate::~subDevCreate()
{
    delete ui;
}

/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void subDevCreate::mousePressEvent(QMouseEvent *event)
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

void subDevCreate::mouseMoveEvent(QMouseEvent *event)
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

void subDevCreate::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

void subDevCreate::retranslationUi()
{
    ui->retranslateUi(this);
}

void subDevCreate::on_pushButton_subDev_ok_clicked()
{
    /*********************************************************/
    if(ui->lineEdit_subDev_pk->text().isEmpty() || ui->lineEdit_subDev_dk->text().isEmpty() || ui->lineEdit_subDev_ps->text().isEmpty())
    {
        MMessageBox::critical(this,tr("错误"),tr("内容不能为空"),tr("确认"));
        return;
    }
    if(ui->lineEdit_subDev_pk->text().length() < 6)
    {
        MMessageBox::critical(this,tr("错误"),tr("产品号长度必须大于或等于6位"),tr("确认"));
        return;
    }
    if(ui->lineEdit_subDev_ps->text().length() < 16)
    {
        MMessageBox::critical(this,tr("错误"),tr("产品密钥长度必须大于或等于16位"),tr("确认"));
        return;
    }
    if(!ui->lineEdit_subDev_dk->text().isEmpty() && ui->lineEdit_subDev_dk->text().length() < 4)
    {
        MMessageBox::critical(this,tr("错误"),tr("dk长度必须大于或等于4位"),tr("确认"));
        return;
    }

    subDevConfigInfo_t subDevConfigInfo;
    subDevConfigInfo.pk = ui->lineEdit_subDev_pk->text();
    subDevConfigInfo.ps = ui->lineEdit_subDev_ps->text();
    subDevConfigInfo.dk = ui->lineEdit_subDev_dk->text();
    if(!ui->lineEdit_subDev_ds->text().isEmpty())
        subDevConfigInfo.ds = ui->lineEdit_subDev_ds->text();
    subDevConfigInfo.tls = QString::number(ui->spinBox_subDev_tls->value());
    subDevConfigInfo.lifetime = QString::number(ui->spinBox_subDev_lifetime->value());


    if(0 == ui->comboBox_productMode->currentIndex() )
    {
        subDevConfigInfo.filePath.clear();
    }
    else
    {
        subDevConfigInfo.filePath = ui->lineEdit_file->text();
        if(subDevConfigInfo.filePath.isEmpty())
        {
            MMessageBox::warning(this,tr("警告"),tr("选择物模型数据格式，必须上传文件"),tr("确认"));
            return;
        }
        QFileInfo info(subDevConfigInfo.filePath);
        if(info.exists())
        {
            qInfo()<<info.size();
            if(info.size() > 1024*1024)
            {
                MMessageBox::critical(this,tr("警告"),tr("文件超出限制"),tr("确认"));
                return;
            }
        }
        else
        {
            MMessageBox::critical(this,tr("警告"),tr("文件不存在"),tr("确认"));
            return;
        }
        jsonFile *jsonfile = new jsonFile(subDevConfigInfo.filePath);
        bool ret = jsonfile->readFile();
        delete(jsonfile);
        if(ret == false)
        {
            MMessageBox::critical(this,tr("错误"),tr("导入的物模型为错误格式"),tr("确认"));
            return;
        }

    }
    emit sendSubDevCreateInfo(subDevConfigInfo);
}

void subDevCreate::on_pushButton_subDev_cancel_clicked()
{
    this->close();
}



void subDevCreate::on_comboBox_productMode_currentIndexChanged(int index)
{
    qInfo()<<__FUNCTION__;
    if(index == 0)
    {
        ui->label_model->hide();
        ui->lineEdit_file->hide();
        ui->lineEdit_getFile->hide();
        ui->pushButton_file->hide();
        ui->comboBox_environment->hide();
        ui->pushButton_getFile->hide();
        ui->checkBox_automatic->hide();
    }
    else if(index == 1)
    {
        ui->label_model->show();
        if (ui->checkBox_automatic->isChecked())
        {
            ui->comboBox_environment->show();
            ui->pushButton_getFile->show();
            ui->lineEdit_file->hide();
            ui->lineEdit_getFile->show();
        }
        else
        {
            ui->pushButton_file->show();
            ui->lineEdit_file->show();
            ui->lineEdit_getFile->hide();
        }
        ui->checkBox_automatic->show();
    }
}

void subDevCreate::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    emit closeWidgetEvent();
}

void subDevCreate::on_pushButton_file_clicked()
{
    fileManager *fileRead = new fileManager();

    if(fileRead->fileManagerOpenAndRead("json") == false)
    {
        return;
    }
    if(fileRead->fileInfo.myFile.fileName().isEmpty())
        ui->lineEdit_file->text().clear();
    else
        ui->lineEdit_file->setText(fileRead->fileInfo.myFile.fileName());
    delete(fileRead);
}

void subDevCreate::on_checkBox_automatic_clicked(bool checked)
{
    if (checked)
    {
        ui->comboBox_environment->show();
        ui->pushButton_getFile->show();
        ui->pushButton_file->hide();
        ui->lineEdit_getFile->show();
        ui->lineEdit_file->hide();
    }
    else
    {
        ui->comboBox_environment->hide();
        ui->pushButton_getFile->hide();
        ui->pushButton_file->show();
        ui->lineEdit_file->show();
        ui->lineEdit_getFile->hide();
    }
}

void subDevCreate::on_pushButton_getFile_clicked()
{
    ui->lineEdit_file->clear();
    ui->lineEdit_getFile->clear();
    if (ui->lineEdit_subDev_pk->text() == NULL)
    {
        MMessageBox::information(this, tr("提示"), tr("请输入ProductKey后再试"), tr("确定"));
        return;
    }
    QString date = httpClient->get_STL_date(ui->lineEdit_subDev_pk->text(), ui->comboBox_environment->currentIndex());
    if (date == NULL)
    {
        MMessageBox::critical(this, tr("错误"), tr("获取失败，请检查ProductKey与数据中心是否正确或是否处于有网状态下"), tr("确定"));
        return;
    }
    else
    {
        QDir *folder = new QDir;
        QString softwarePath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)+"/QthTools-MCU_Simulator-Win/";
        QString temporaryFiles = softwarePath + "/temporaryFiles";
        if (!folder->exists(temporaryFiles))
        {
            folder->mkdir(temporaryFiles);
        }
        if (!folder->isEmpty())
        {
            QDirIterator DirsIterator(temporaryFiles, QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
            while(DirsIterator.hasNext())
            {
                folder->remove(DirsIterator.next());// 删除文件操作如果返回否，那它就是目录
            }
        }
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString filePath = temporaryFiles + "/" + current_date_time.toString("yyyyMMddhhmmsszzz") + "_" + ui->lineEdit_subDev_pk->text() + ".json";
        QFile *newFile = new QFile(filePath);
        newFile->open(QIODevice::WriteOnly);
        newFile->write(date.toUtf8());
        newFile->close();
        delete newFile;
        delete folder;
        ui->lineEdit_file->setText(filePath);
        ui->lineEdit_getFile->setText(current_date_time.toString("yyyyMMddhhmmsszzz") + "_" + ui->lineEdit_subDev_pk->text() + ".json");
    }
}
