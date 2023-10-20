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
#include "passwindow.h"
#include "ui_passwindow.h"
#include <QTextCodec>
#include <QKeyEvent>
#include <QClipboard>
#include "kernel/mmessagebox.h"

/**************************************************************************
** 功能	@brief : 透传界面
** 输入	@param :
** 输出	@retval:
***************************************************************************/
passWindow::passWindow(int mode, QWidget *parent):
QWidget(parent),
ui(new Ui::passWindow)
{
    ui->setupUi(this);
    /* 透传命令输入框 */
    uiAttributeSet();
    userMode = mode;
    ui->comboBox_passqos->setView(new QListView());
}

passWindow::~passWindow()
{
    delete ui;
}
/**************************************************************************
** 功能	@brief : 设置父和界面属性 （注：在类实例化化后实现）
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::setParm(void *father)
{
    if(0 == userMode)
    {
        this->father = (MainWindow*)father;
        this->father->configprocess->historyRead("passSend",model_passInput);
    }
}
/**************************************************************************
** 功能	@brief : 查找类名
** 输入	@param :
** 输出	@retval:
***************************************************************************/
QObject *passWindow::findParent(QObject *obj,QString className,QString name)
{
    QObject *parentObj = obj;
    while ((parentObj = parentObj->parent()))
    {
        if((className.isEmpty() || className == parentObj->metaObject()->className()) && (name.isEmpty() || name == parentObj->objectName()))
        {
            return parentObj;
        }
    }
    return NULL;
}
/**************************************************************************
** 功能	@brief :根据子设备类型隐藏部分控件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::hideWidgetForSubDev()
{
    delete ui->label_passqos;
    delete ui->comboBox_passqos;
    ui->pushButton_passSend->hide();
    ui->pushButton_directlySend->hide();
    ui->pushButton_subPassSend->show();
    ui->pushButton_subDirectlySend->show();
    update();
}
/**************************************************************************
** 功能	@brief :获取 读取透传数据的当前长度
** 输入	@param :
** 输出	@retval:
***************************************************************************/
int passWindow::getReadCurrentCount()
{
    return ui->spinBox_passReadByte->value();
}
/**************************************************************************
** 功能	@brief :设置物模型功能是否可用
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::setFuncPermission(int mode)
{
    if(1 == mode)
    {
        qDebug()<<"setFuncPermission(int mode)";
//        ui->groupBox_read->setEnabled(false);
    }
}

/**************************************************************************
** 功能	@brief : 菜单选择
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::listWidgetShow(QStringList data, QRect showPoint)
{
    int listWidget_widget = data.size()*showPoint.height(); //20 * data.size();
    if(NULL == listWidget)
    {
        listWidget = new newListWiget();
    }
    else if(NULL != listWidget)
    {
        delete listWidget;
        listWidget = NULL;
        listWidget = new newListWiget();
    }
    for(int i = 0;i<data.size();i++)
    {
        listWidget->addItem(data[i]);
    }
    listWidget->setWindowFlags(Qt::FramelessWindowHint);
    listWidget->setAttribute(Qt::WA_TranslucentBackground);
    listWidget->setAttribute(Qt::WA_QuitOnClose,false);
    listWidget->setGeometry(showPoint.x(),showPoint.y(),showPoint.width(),listWidget_widget);
    connect(listWidget,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(itemClickedHandle(QListWidgetItem*)));
//    connect(listWidget,SIGNAL(leaveEvent(QEvent *)),this,SLOT(leaveEvent(QEvent *)));
    listWidget->setWindowModality(Qt::ApplicationModal);
    listWidget->show();
}

/**************************************************************************
** 功能	@brief : 设置 读取透传数据的当前长度
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::setReadMaxCount(int value)
{
    ui->spinBox_passReadByte->setValue(value);
}
/**************************************************************************
** 功能	@brief : 翻译转换
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool passWindow::retranslationUi()
{
    if(NULL != listWidget)
    {
         listWidget->deleteLater();
         listWidget = NULL;
    }
    ui->retranslateUi(this);
    return true;
}

/**************************************************************************
** 功能	@brief : 功能选择
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::itemClickedHandle(QListWidgetItem *item)
{
    if(0 == item->text().compare(tr("直接发送")) || 0 == item->text().compare(tr("透传发送")))
    {
        if(0 == item->text().compare(tr("直接发送")))
        {
            if(Qt::Checked == ui->checkBox_passSendHex->checkState())
            {
                ui->checkBox_passSendHex->setChecked(false);
            }
            ui->checkBox_passSendHex->setEnabled(false);
        }
        else if(!ui->checkBox_passSendHex->isEnabled())
        {
            ui->checkBox_passSendHex->setEnabled(true);
        }
        ui->pushButton_passSend->setText(item->text());
    }
    else if(0 == item->text().compare(tr("读取数据")) || 0 == item->text().compare(tr("获取读取的最大值")))
    {
        ui->pushButton_passRead->setText(item->text());
    }
    listWidget->close();
    listWidget = NULL;
}

/**************************************************************************
** 功能	@brief : 界面UI设计
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::uiAttributeSet()
{
    model_passInput = new QStandardItemModel(0, 1, this);
    completer_passInput = new QCompleter(model_passInput, this);
    textEdit_passInput = new CompleterTextEdit;
    textEdit_passInput->setCompleter(completer_passInput);
    QVBoxLayout *vbox_passInput = new QVBoxLayout;
    vbox_passInput->addWidget(textEdit_passInput);
    vbox_passInput->setContentsMargins(0,0,0,0);
    ui->widget_passInput->setLayout(vbox_passInput);
    passInputMaxLength = 4096;
    connect(textEdit_passInput,SIGNAL(textChanged()),this,SLOT(passInput_textChanged()));

    ui->textEdit_passRead->setFocusPolicy(Qt::NoFocus);
    textEdit_passInput->moveCursor(QTextCursor::Start);
    //不接受富文本写入
    textEdit_passInput->setAcceptRichText(false);
    // 设置该控件无右键菜单
    // textEdit_passInput->setContextMenuPolicy(Qt::NoContextMenu);

    ui->spinBox_passReadByte->setEnabled(true);

    if(0 == ui->pushButton_passSend->text().compare(tr("直接发送")))
    {
        if(Qt::Unchecked == ui->checkBox_passSendHex->checkState())
        {
            ui->checkBox_passSendHex->setCheckable(false);
        }
        ui->checkBox_passSendHex->setEnabled(false);
    }
    //QComboBox，QSpinBox注册鼠标滑轮事件
    ui->comboBox_passqos->installEventFilter(this);
    ui->spinBox_passReadByte->installEventFilter(this);
    ui->pushButton_subPassSend->hide();
    ui->pushButton_subDirectlySend->hide();
}
/**************************************************************************
** 功能	@brief : 界面信息清除
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::resetUIValue()
{
    ui->textEdit_passRead->clear();
    textEdit_passInput->clear();
    ui->spinBox_passReadByte->setValue(512);
}
/**************************************************************************
** 功能	@brief : 透传模式发送的数据 是否为16进制的设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::on_checkBox_passSendHex_stateChanged(int arg1)
{
    qInfo()<<__FUNCTION__;
    if(arg1 == 0)
    {
        QByteArray sendData = QByteArray::fromHex(textEdit_passInput->toPlainText().toUtf8());
        textEdit_passInput->setText(sendData);
        ui->pushButton_subDirectlySend->setEnabled(true);
        ui->pushButton_directlySend->setEnabled(true);
    }
    else
    {
        QByteArray sendData = QString(textEdit_passInput->toPlainText()).toUtf8().toHex();
        textEdit_passInput->setText(sendData);
        ui->pushButton_subDirectlySend->setEnabled(false);
        ui->pushButton_directlySend->setEnabled(false);
    }
}
/**************************************************************************
** 功能	@brief : 透传模式读取的数据 是否为16进制的设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::on_checkBox_passReadHex_stateChanged(int arg1)
{
    qInfo()<<__FUNCTION__;
    if(arg1 == 0)
    {
        QByteArray sendData = QByteArray::fromHex(QString(ui->textEdit_passRead->toPlainText()).toUtf8());
        ui->textEdit_passRead->setText(sendData);
    }
    else
    {
        QByteArray sendData = QString(ui->textEdit_passRead->toPlainText()).toUtf8().toHex();
        ui->textEdit_passRead->setText(sendData);
    }
}
/**************************************************************************
** 功能	@brief : 透传模式读取数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::on_pushButton_passRead_clicked()
{
    if(0 == this->userMode)
    {
        myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QString,"AT+QIOTRD="+QString::number(ui->spinBox_passReadByte->value())));
    }
    else if(1 == this->userMode)
    {
        myObjMessServer::getObjMessServer()->notify("subdevwindow_ATRecvPassData",Q_ARG(int,ui->spinBox_passReadByte->value()));
    }
}
/**************************************************************************
** 功能	@brief : 透传模式读取最大数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::on_pushButton_passReadMax_clicked()
{
    if(0 == this->userMode)
    {
        myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QString,"AT+QIOTRD=?"));
    }
    else if(1 == this->userMode)
    {
       myObjMessServer::getObjMessServer()->notify("subdevwindow_ATSubSendData",Q_ARG(QByteArray,QString("AT+QIOTSUBRD=?").toUtf8()),Q_ARG(bool,true));
    }
}

/**************************************************************************
** 功能	@brief : 历史数据 再次使用
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::csdkPassDataIssue(QByteArray data)
{
    qInfo()<<__FUNCTION__;
    if(data.isNull())
    {
        if(!ui->label_passReadCount->text().isEmpty())
        {
            ui->label_passReadCount->setText(QString::number(ui->label_passReadCount->text().toInt()+1));
        }
    }
    else
    {
        if(ui->checkBox_passReadHex->isChecked())
        {
            ui->textEdit_passRead->setText(data.toHex());
        }
        else
        {
            QByteArray sendData = QByteArray::fromHex(QString(data.toHex()).toUtf8());
            ui->textEdit_passRead->setText(sendData);
        }
    }
}
/**************************************************************************
** 功能	@brief : 历史数据 读取
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::csdkPassDataRead(QString readByte,QString readCount,QByteArray data)
{
    qInfo()<<__FUNCTION__;
    ui->label_passReadByte->setText(readByte);
    ui->label_passReadCount->setText(readCount);
    if(!data.isNull())
    {
        dataCache.append(data);
        ui->textEdit_passRead->clear();
        if(ui->checkBox_passReadHex->isChecked())
        {
            ui->textEdit_passRead->setText(data.toHex());
        }
        else
        {
            QByteArray sendData = QByteArray::fromHex(QString(data.toHex()).toUtf8());
            ui->textEdit_passRead->setText(sendData);
        }
        if(readByte.toInt() !=0)
        {
            return;
        }
        else
        {
            dataCache.clear();
        }
    }
}
/**************************************************************************
** 功能	@brief : 透传模式 数据发送框变化
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::passInput_textChanged()
{

    QString textContent = textEdit_passInput->toPlainText();
    int length = textContent.count();
    qDebug()<<"passInput_textChanged,length:"<<length;
    if(length > passInputMaxLength)
    {
        MMessageBox::critical(this,tr("错误"),tr("剪切数据过长,已自动删减为4096字节长度的字符串"),tr("确认"));

        int length = textEdit_passInput->toPlainText().count();
        QString text = textEdit_passInput->toPlainText();
        int position = textEdit_passInput->textCursor().position(); //输入后光标原应该在的位置
        QTextCursor text_cursor = textEdit_passInput->textCursor();
        text.remove(position - (length - passInputMaxLength), length - passInputMaxLength);//去掉多余的文字
        textEdit_passInput->setText(text);
        text_cursor.setPosition(position - (length - passInputMaxLength));//光标移到正确的位置
        textEdit_passInput->setTextCursor(text_cursor);
    }
}
/**************************************************************************
** 功能	@brief : 透传模式 过滤发送数据
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool passWindow::eventFilter(QObject *target, QEvent *event)
{
    if(target->metaObject()->className() == QStringLiteral("QComboBox") || target->metaObject()->className() == QStringLiteral("QSpinBox"))
    {
        if (event->type() == QEvent::Wheel)//鼠标滚轮事件
        {
            return true;//禁用下拉框的滚轮改变项的功能
        }
    }
    if (target == textEdit_passInput)
    {
        if (event->type() == QEvent::KeyPress)
        {

            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->matches(QKeySequence::Paste) )
            {

                QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
                QString originalText = clipboard->text();	     //获取剪贴板上文本信息
                QString originalText2;
                if(originalText.count()>passInputMaxLength)
                    originalText2 = originalText.left(passInputMaxLength);
                clipboard->setText(originalText2);
                textEdit_passInput->setText(originalText2);
                return true;
            }
        }
        // 鼠标右键事件无法获取
//        if (event->type() == QEvent::MouseButtonRelease)
//        {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//            if (mouseEvent->button() == Qt::MidButton)
//            {
//                return true;
//            }
//        }
//        if (event->type() == QEvent::MouseButtonPress)
//        {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//        }
    }

    return QWidget::eventFilter(target, event);
}
/**************************************************************************
** 功能	@brief : 鼠标事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::mousePressEvent(QKeyEvent *e)
{
    Q_UNUSED(e);
    qDebug()<<__FUNCTION__;
    if(listWidget != NULL)
    {
        if(!listWidget->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        {
            disconnect(listWidget,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(itemClickedHandle(QListWidgetItem*)));
//            disconnect(listWidget,SIGNAL(leaveEvent(QEvent *)),this,SLOT(leaveEvent(QEvent *)));
            listWidget->close();
            delete listWidget;
            listWidget = NULL;
        }
    }
}
/**************************************************************************
** 功能	@brief : 菜单关闭
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::listWidgetCheckClose()
{
    if(listWidget != NULL)
    {
        disconnect(listWidget,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(itemClickedHandle(QListWidgetItem*)));
//        disconnect(listWidget,SIGNAL(leaveEvent(QEvent *)),this,SLOT(leaveEvent(QEvent *)));
        listWidget->close();
        delete listWidget;
        listWidget = NULL;
    }
}
/**************************************************************************
** 功能	@brief : 界面离开事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::leaveEvent(QEvent *e)
{
    Q_UNUSED(e);
    qDebug()<<__FUNCTION__;
    if(listWidget != NULL && !(this->geometry().contains(this->mapFromGlobal(QCursor::pos()))))
    {
        if(!listWidget->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        {
            disconnect(listWidget,SIGNAL(itemPressed(QListWidgetItem*)),this,SLOT(itemClickedHandle(QListWidgetItem*)));
            disconnect(listWidget,SIGNAL(leaveEvent(QEvent *)),this,SLOT(leaveEvent(QEvent *)));
            listWidget->close();
            delete listWidget;
            listWidget = NULL;
        }
    }
}
/**************************************************************************
** 功能	@brief : 透传数据发送(透传发送)
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::on_pushButton_passSend_clicked()
{
    qInfo()<<__FUNCTION__;
    for(int i=0;i<model_passInput->rowCount();i++)
    {
        if(model_passInput->index(i,0).data().toString() == textEdit_passInput->toPlainText())
        {
            model_passInput->removeRow(i);
            break;
        }
    }
    model_passInput->insertRow(0);
    model_passInput->setData(model_passInput->index(0, 0), textEdit_passInput->toPlainText());
    QByteArray sendData;
    bool isHexData = ui->checkBox_passSendHex->isChecked();
    if(isHexData)
    {
         sendData = QByteArray::fromHex(textEdit_passInput->toPlainText().toUtf8());
        // this->father->ATCMDSend("AT+QIOTSEND="+ui->comboBox_passqos->currentText()+","+QString::number(sendData.length()),sendData);
    }
    else
    {
        sendData = QString(textEdit_passInput->toPlainText()).toUtf8();
        // this->father->ATCMDSend("AT+QIOTSEND="+ui->comboBox_passqos->currentText()+","+QString::number(sendData.length()),sendData);
    }

    if(0 == sendData.length())
    {
        MMessageBox::critical(this,tr("错误"),tr("发送数据不能为空"),tr("确认"));
        return;
    }
    if(userMode == 0)
    {
        myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QString,"AT+QIOTSEND="+ui->comboBox_passqos->currentText()+","+QString::number(sendData.length())),Q_ARG(QByteArray,sendData));
    }
    else if(userMode == 1)
    {
        myObjMessServer::getObjMessServer()->notify("subdevwindow_ATSendPassData",Q_ARG(QByteArray,sendData));
    }

    if(model_passInput->rowCount() > 30)
    {
        model_passInput->removeRow(30);
    }

//    myObjMessServer::getObjMessServer()->notify("configprocess_historySave",Q_ARG(QString,"passSend"),Q_ARG(QStandardItemModel*,model_passInput));

    if(userMode == 0)
    {
         this->father->configprocess->historySave("passSend",*model_passInput);
         this->father->statusBarShow(tr("透传数据已发送"),3);
    }
    else if(userMode == 1)
    {
        myObjMessServer::getObjMessServer()->notify("mainwindow_statusBarShow",Q_ARG(QString,tr("透传数据已发送")),Q_ARG(int,3));
    }
}
/**************************************************************************
** 功能	@brief : 透传数据发送(直接发送)
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void passWindow::on_pushButton_directlySend_clicked()
{
    qInfo()<<__FUNCTION__;
    for(int i=0;i<model_passInput->rowCount();i++)
    {
        if(model_passInput->index(i,0).data().toString() == textEdit_passInput->toPlainText())
        {
            model_passInput->removeRow(i);
            break;
        }
    }
    model_passInput->insertRow(0);
    model_passInput->setData(model_passInput->index(0, 0), textEdit_passInput->toPlainText());
    QByteArray sendData;
    bool isHexData = ui->checkBox_passSendHex->isChecked();
    if(isHexData)
    {
         sendData = QByteArray::fromHex(QString(textEdit_passInput->toPlainText()).toUtf8());
        // this->father->ATCMDSend("AT+QIOTSEND="+ui->comboBox_passqos->currentText()+","+QString::number(sendData.length()),sendData);
    }
    else
    {
        sendData = QString(textEdit_passInput->toPlainText()).toUtf8();
        // this->father->ATCMDSend("AT+QIOTSEND="+ui->comboBox_passqos->currentText()+","+QString::number(sendData.length()),sendData);
    }
    if(0 == sendData.length())
    {
        MMessageBox::critical(this,tr("错误"),tr("发送数据不能为空"),tr("确认"));
        return;
    }
    if(userMode == 0)
    {
        myObjMessServer::getObjMessServer()->notify("mainwindow_ATCMDSend",Q_ARG(QByteArray,QString("AT+QIOTSEND="+ui->comboBox_passqos->currentText()+","+QString::number(sendData.length())+",\"").toUtf8()+sendData+QString("\"").toUtf8()),Q_ARG(bool,true));
    }
    else if(userMode == 1)
    {
        myObjMessServer::getObjMessServer()->notify("subdevwindow_ATSendDirectlyData",Q_ARG(QByteArray,sendData),Q_ARG(bool,true));
    }

    if(model_passInput->rowCount() > 30)
    {
        model_passInput->removeRow(30);
    }

//    myObjMessServer::getObjMessServer()->notify("configprocess_historySave",Q_ARG(QString,"passSend"),Q_ARG(QStandardItemModel*,model_passInput));

    if(userMode == 0)
    {
         this->father->configprocess->historySave("passSend",*model_passInput);
         this->father->statusBarShow(tr("透传数据已发送"),3);
    }
    else if(userMode == 1)
    {
        myObjMessServer::getObjMessServer()->notify("mainwindow_statusBarShow",Q_ARG(QString,tr("透传数据已发送")),Q_ARG(int,3));
    }
}

void passWindow::on_pushButton_subPassSend_clicked()
{
    on_pushButton_passSend_clicked();
}

void passWindow::on_pushButton_subDirectlySend_clicked()
{
    on_pushButton_directlySend_clicked();
}
