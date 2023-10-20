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
#ifndef MUTILSTRINGSEND_H
#define MUTILSTRINGSEND_H

#include <QWidget>
#include <QGridLayout>
#include <QMenu>

namespace Ui {
class mutilStringSend;
}

class mutilStringSend : public QWidget
{
    Q_OBJECT

public:
    explicit mutilStringSend(QWidget *parent = nullptr);
    ~mutilStringSend();
    bool retranslationUi();
private:
    void initSqlite();
    void updateLineCounts(int currentVaildLine);
    void insertLine(QGridLayout *dstLayout,int line,QString text,QString note,bool mode=false);
    void findHistoryUpdateWidget(QGridLayout *layout,QList<QStringList> list,bool mode,int startPlace = 0);
    void uiAttributeSet(void);
private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void sendString();
    void tabWidget_currentChanged(int index);
    void on_tabWidget_tabBarDoubleClicked(int index);
    void on_pushButton_importDb_clicked();
    void on_pushButton_exportDb_clicked();
//    void recvTabClose(int index);
    void recvButtonTextChange(int index,QString text);
signals:
    void ATCMDsend(QString data);
private:
    Ui::mutilStringSend *ui;
    QObject *userInfoSql = NULL;
    int currentLineCounts = 100;
    int m_oldTabColumn = 0;
    QAction *deleteAction;
    QMenu *buttonMenu;
    bool m_bDrag = false;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;
};

#endif // MUTILSTRINGSEND_H
