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
#include "addconfigwindow.h"
#include "ui_addconfigwindow.h"
#include <QDebug>
#include "kernel/filemanager.h"
#include "kernel/mmessagebox.h"
#include <QStandardPaths>
#include <QFileInfo>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QListView>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>

addConfigWindow::addConfigWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addConfigWindow)
{
    ui->setupUi(this);
//    this->setWindowFlags(this->windowFlags()&(~Qt::WindowContextHelpButtonHint));
//    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowModality(Qt::ApplicationModal);
    addConfigWindow::setWindowTitle(tr("请添加配置信息"));
    ui->lineEdit_pk->setValidator(new QRegExpValidator(QRegExp("[0-9A-Za-z]{12}"),this));
    ui->lineEdit_ps->setValidator(new QRegExpValidator(QRegExp("[0-9A-Za-z]{32}"),this));
    ui->comboBox_data->setCurrentIndex(0);
    ui->label_file->hide();
    ui->lineEdit_file->hide();
    ui->lineEdit_getFile->hide();
    ui->pushButton_file->hide();
    ui->comboBox_environment->hide();
    ui->pushButton_getFile->hide();
    ui->checkBox_automatic->hide();
    //去除窗体边框
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    //窗体关闭信号
    connect(ui->pushButton_close, &QPushButton::clicked, [=]() {close(); });
    this->setWindowModality(Qt::ApplicationModal);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor("#C0C0C0"));
    shadow->setBlurRadius(15);
    shadow->setOffset(5);
    ui->widget->setGraphicsEffect(shadow);
    ui->comboBox_data->setView(new QListView());
    ui->comboBox_environment->setView(new QListView());
    httpClient = new HttpClient();
}

addConfigWindow::~addConfigWindow()
{
    delete ui;
}

/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void addConfigWindow::mousePressEvent(QMouseEvent *event)
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

void addConfigWindow::mouseMoveEvent(QMouseEvent *event)
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

void addConfigWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

void addConfigWindow::retranslationUi()
{
    ui->retranslateUi(this);
}
/**************************************************************************
** 功能	@brief :  物模型类型的产品json格式数据导入
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void addConfigWindow::on_pushButton_file_clicked()
{
    qInfo()<<__FUNCTION__;
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
/**************************************************************************
** 功能	@brief :  自动获取物模型类型的产品json格式数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void addConfigWindow::on_pushButton_getFile_clicked()
{
    ui->lineEdit_file->clear();
    ui->lineEdit_getFile->clear();
    if (ui->lineEdit_pk->text() == NULL)
    {
        MMessageBox::information(this, tr("提示"), tr("请输入ProductKey后再试"), tr("确定"));
        return;
    }
    QString date = httpClient->get_STL_date(ui->lineEdit_pk->text(), ui->comboBox_environment->currentIndex());
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
        QString filePath = temporaryFiles + "/" + current_date_time.toString("yyyyMMddhhmmsszzz") + "_" + ui->lineEdit_pk->text() + ".json";
        QFile *newFile = new QFile(filePath);
        newFile->open(QIODevice::WriteOnly);
        newFile->write(date.toUtf8());
        newFile->close();
        delete newFile;
        delete folder;
        ui->lineEdit_file->setText(filePath);
        ui->lineEdit_getFile->setText(current_date_time.toString("yyyyMMddhhmmsszzz") + "_" + ui->lineEdit_pk->text() + ".json");
    }
}
/**************************************************************************
** 功能	@brief :  产品 透传/物模型 类型选择
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void addConfigWindow::on_comboBox_data_currentIndexChanged(int index)
{
    qInfo()<<__FUNCTION__;
    if(index == 0)
    {
        ui->label_file->hide();
        ui->lineEdit_file->hide();
        ui->lineEdit_getFile->hide();
        ui->pushButton_file->hide();
        ui->comboBox_environment->hide();
        ui->pushButton_getFile->hide();
        ui->checkBox_automatic->hide();
    }
    else if(index == 1)
    {
        ui->label_file->show();
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

void addConfigWindow::closeEvent(QCloseEvent *event)
{
    sendCloseEvent(event);
}
/**************************************************************************
** 功能	@brief :  新建产品的判断
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void addConfigWindow::on_pushButton_OK_clicked()
{
    configInfo_t configInfo;
    configInfo.pk = ui->lineEdit_pk->text();
    if(configInfo.pk.length() != 6)
    {
        MMessageBox::critical(this,tr("错误"),tr("产品号长度错误"),tr("确认"));
        return;
    }
    configInfo.ps = ui->lineEdit_ps->text();
    if(configInfo.ps.length() != 16)
    {
        MMessageBox::critical(this,tr("错误"),tr("产品密钥长度错误"),tr("确认"));
        return;
    }
    if(ui->comboBox_data->currentIndex() == 0)
    {
        configInfo.filePath.clear();
    }
    else
    {
        configInfo.filePath = ui->lineEdit_file->text();
        if(configInfo.filePath.isEmpty())
        {
            MMessageBox::information(this,tr("警告"),tr("选择物模型数据格式，必须上传文件"),tr("确认"));
            return;
        }
        QFileInfo info(configInfo.filePath);
        if(info.exists())
        {
            qInfo()<<info.size();
            if(info.size() > 1024*1024)
            {
                MMessageBox::critical(this,tr("错误"),tr("文件超出限制"),tr("确认"));
                return;
            }
        }
        else
        {
            MMessageBox::critical(this,tr("错误"),tr("文件不存在"),tr("确认"));
            return;
        }
        jsonFile *jsonfile = new jsonFile(configInfo.filePath);
        bool ret = jsonfile->readFile();
        delete(jsonfile);
        if(ret == false)
        {
            MMessageBox::critical(this,tr("错误"),tr("导入的物模型文件格式错误"),tr("确认"));
            return;
        }

    }
    emit addConfigSignal(configInfo);
    this->close();
}

void addConfigWindow::on_pushButton_Cancel_clicked()
{
    this->close();
}

void addConfigWindow::on_checkBox_automatic_clicked(bool checked)
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

void addConfigWindow::on_comboBox_environment_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    ui->lineEdit_file->clear();
    ui->lineEdit_getFile->clear();
}
