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
#ifndef SUBDEVCOMM_H
#define SUBDEVCOMM_H

#include <QWidget>
#include "app/passwindow.h"
#include "app/modelwindow.h"

typedef struct
{
    QString PK;
    QString DK;
}deviceInfo;

namespace Ui {
class subDevComm;
}

class subDevComm : public QWidget
{
    Q_OBJECT

public:
    subDevComm(QWidget *parent = nullptr);
    subDevComm(QStringList data,QWidget *parent= nullptr);
    subDevComm(QStringList data,QList<deviceInfo>userInfo,QWidget *parent= nullptr);
    ~subDevComm();
    void uiAttributeSet();
    QString getPKInfo();
    QString getDKInfo();
    int getReadValue(int userType);
    void handlePassData(QList<QByteArray> paraList);
    void handleModuleData(QList<QByteArray> paraList);
    void tabModuleChangeJson(QString path);
    void setModuleFile(QString pathFile);
    void setPassDataRecvInfo(QList<QByteArray> paraList,int mode);
    void setModelDataRecvInfo(QList<QByteArray> paraList,int mode);
    void setSubDevStatus(QString PK, QString DK, bool checked);
    void infoInit(QStringList data);
    void setPassReadMaxCount(int value);
    void setTabbModuleReadMaxLen(int len);
    bool retranslationUi();
    QWidget *tabPass;
    QWidget *tabModule;

signals:
    void closeWidgetEvent();

private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void on_pushButton_pass_clicked();
    void on_pushButton_model_clicked();

private:
    Ui::subDevComm *ui;
    void hideModuleWidget();
    void hideSubWidget(int index);
    QList<deviceInfo>usersInfo;
    void closeEvent(QCloseEvent *event);
    bool m_bDrag = false;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;
};

#endif // SUBDEVCOMM_H
