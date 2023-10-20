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
#ifndef TTLVCONVERTJSON_H
#define TTLVCONVERTJSON_H

#include <QWidget>
#include <QMap>

namespace Ui {
class ttlvConvertJson;
}

class ttlvConvertJson : public QWidget
{
    Q_OBJECT

public:
    explicit ttlvConvertJson(QWidget *parent = nullptr);
    ~ttlvConvertJson();
    void explainJsonObject(QJsonObject srcObj,QByteArray &ttlvData);
    bool explainJsonArray(QJsonArray srcArray,QByteArray &ttlvData);
    void tj_dataFilter(QString src,QByteArray &dst);
    void jt_dataFilter(QString src);
    int getInt(char asciiNumber);
    void anlyzeTtlvData(QByteArray data);
    QByteArray byteArraySplitLine(QByteArray data);
    void retranslationUi();

private:
    void uiAttributeSet(void);

private:
    Ui::ttlvConvertJson *ui;
    bool currentChangeMode;
    QString ttlvWidgetSrcHistoryData;
    QString jsonWidgetSrcHistoryData;
    bool m_bDrag = false;
    QPoint mouseStartPoint;
    QPoint windowTopLeftPoint;

private slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void on_radioButton_TJ_clicked();
    void on_radioButton_JT_clicked();
    void on_pushButton_change_clicked();
};

#endif // TTLVCONVERTJSON_H
