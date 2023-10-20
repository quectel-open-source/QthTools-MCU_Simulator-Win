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
#ifndef INFOWINDOW_H
#define INFOWINDOW_H

#include <QWidget>
#include "mainwindow.h"

namespace Ui {
class infoWindow;
}

class infoWindow : public QWidget
{
    Q_OBJECT

public:
    explicit infoWindow(QWidget *parent = nullptr);
    ~infoWindow();
    void setParm(void *father);
    void csdkInfoGet(int count,QList<QByteArray> paraList);
    void locationUpdate(QByteArrayList data,int number);
    void locationNameUpdata(QByteArrayList data,QTableWidget *widget);
    void locaionDataUpdate(QByteArrayList data,QTableWidget *widget);
    void buttonEvent(QPushButton *senderObj,QTableWidget *widget);
    void resetUIValue();
    bool retranslationUi();
    void setInfoWindowModel(int model);
//    void locationExternUpdate(QByteArrayList data);
//    bool diffListMember(QModelIndex *one ,QModelIndex *two);
private slots:
    void OnBtnClicked();
    void OnBtnOutlayClicked();
    void on_pushButton_infoGet_clicked();
    void on_pushButton_infoReport_clicked();
    void on_pushButton_configTest_clicked();
    void on_pushButton_configGet_clicked();
    void on_pushButton_configSet_clicked();
    void on_pushButton_outlayGPSTest_clicked();
    void on_pushButton_outlayGPSSet_clicked();
    void on_pushButton_outlayGPSAdd_clicked();
    void on_pushButton_outlayGPSDelete_clicked();
    void on_pushButton_reportInsideLbs_clicked();

    void on_pushButton_locationInfo_clicked();

    void on_pushButton_locationInfoExc_clicked();

private:
    Ui::infoWindow *ui;
    MainWindow *father;
    QList<int> getListSelectedRows(QTableWidget * tableWidget);
    QTableWidget *currentTableWidget;
};

#endif // INFOWINDOW_H
