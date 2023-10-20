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
#ifndef OTAWINDOW_H
#define OTAWINDOW_H

#include <QWidget>
#include "mainwindow.h"


#define FOTA_UPDATA true
#define SOTA_UPDATA false

namespace Ui {
class otaWindow;
}

class otaWindow : public QWidget
{
    Q_OBJECT

public:
    explicit otaWindow(QWidget *parent = nullptr);
    ~otaWindow();
    void setParm(void *father);
    void csdkMcuVerGet(QList<QByteArray> paraList);
    void csdkMcuVerSet(QList<QByteArray> paraList);
    void csdkOtaEventProcess(QList<QByteArray> paraList);
    void csdkOtaFileRead(int startAddr,int len,QByteArray data);
    void clearValue();
    void resetUIValue();
    bool retranslationUi();
private slots:
    void on_pushButton_otaRefused_clicked();
    void on_pushButton_otaConfirm_clicked();
    void on_pushButton_otaDownPiece_clicked();
    void on_pushButton_otaReportStatus_clicked();
    void on_pushButton_sotaRead_clicked();
    void on_pushButton_otaMcuListGet_clicked();
    void on_pushButton_otaMcuVerGet_clicked();
    void on_pushButton_otaMcuVerSet_clicked();
    void on_pushButton_otaMcuVerAdd_clicked();
    void on_pushButton_otaMcuVerDel_clicked();
    void on_pushButton_otaFile_clicked();
    void on_checkBox_sotaAuto_toggled(bool checked);

    void on_comboBox_otaStatus_currentIndexChanged(int index);

    void on_tableWidget_otaMcuVer_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_pushButton_OTARequest_clicked();

    void on_pushButton_http_ota_req_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_pushButton_mqtt_clicked();

    void on_pushButton_http_clicked();

private:
    void uiAttributeSet();
private:
    Ui::otaWindow *ui;
    MainWindow *father;
    fileManager *otafile;
    int sotaStartAdds;
    bool otaType;
};

#endif // OTAWINDOW_H
