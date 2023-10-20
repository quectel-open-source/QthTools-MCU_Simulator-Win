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
#ifndef PASSWINDOW_H
#define PASSWINDOW_H

#include "mainwindow.h"
#include "third/completertextedit.h"

#include <QCompleter>
namespace Ui {
class passWindow;
}



class passWindow : public QWidget
{
    Q_OBJECT

public:
    explicit passWindow(int mode = 0,QWidget *parent = nullptr);
    ~passWindow();
    void setParm(void *father);
    void csdkPassDataIssue(QByteArray data);
    void csdkPassDataRead(QString readByte,QString readCount,QByteArray data);
    void uiAttributeSet(void);
    void resetUIValue();
    QObject *findParent(QObject *obj,QString className,QString name);
    void hideWidgetForSubDev();
    int getReadCurrentCount();
    void setFuncPermission(int mode);
    void listWidgetShow(QStringList data,QRect showPoint);
    void listWidgetCheckClose();
    void setReadMaxCount(int value);
    bool retranslationUi();

private slots:
//    void on_pushButton_passSend_clicked();
    void on_checkBox_passSendHex_stateChanged(int arg1);
    void on_checkBox_passReadHex_stateChanged(int arg1);
    void on_pushButton_passRead_clicked();
    void passInput_textChanged();
//    void on_pushButton_sendModeSelect_clicked();
    void itemClickedHandle(QListWidgetItem *item);
//    void on_pushButton_readActionSelect_clicked();
    void leaveEvent(QEvent *e);
    void on_pushButton_passSend_clicked();
    void on_pushButton_directlySend_clicked();
    void on_pushButton_passReadMax_clicked();

    void on_pushButton_subPassSend_clicked();

    void on_pushButton_subDirectlySend_clicked();

private:
    bool eventFilter(QObject *target, QEvent *event);
    void mousePressEvent(QKeyEvent *e);

private:
    Ui::passWindow *ui;
    MainWindow *father;
    int userMode;
    QStandardItemModel *model_passInput;
    QCompleter * completer_passInput;
    CompleterTextEdit *textEdit_passInput;
    int passInputMaxLength;
    QLabel *noticeLabel;
    QString dataCache;
    newListWiget *listWidget = NULL;
};

#endif // PASSWINDOW_H
