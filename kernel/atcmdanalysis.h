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
#ifndef ATCMDANALYSIS_H
#define ATCMDANALYSIS_H
#include <QList>
#include <QTextEdit>

typedef void (*atAnalysiscb)(QString data);
class atcmdAnalysis
{
public:
    void handle_atcmdMatchList(QString data);
    void analysis_QIOTREG(QString data);
    void analysis_QIOTSTATE(QString data);
    void analysis_QIOTSEND(QString data);
    void analysis_QIOTRD(QString data);
    void analysis_QIOTCFG(QString data);
    void analysis_QIOTMCUVER(QString data);
    void analysis_QIOTINFO(QString data);
    void analysis_QIOTUPDATE(QString data);
    void analysis_QIOTOTARD(QString data);
    void analysis_QIOTEVT(QString data);
    void analysis_QIOTMODELTD(QString data);
    void analysis_QIOTMODELRD(QString data);
    void analysis_QIOTLOCIN(QString data);
    void analysis_QIOTLOCEXT(QString data);
    void analysis_QIOTOTAREQ(QString data);
    void analysis_QFOTAUP(QString data);
    void analysis_QFOTACFG(QString data);
    void analysis_QIOTSUBCONN(QString data);
    void analysis_QIOTSUBDISCONN(QString data);
    void analysis_QIOTSUBSEND(QString data);
    void analysis_QIOTSUBRD(QString data);
    void analysis_QIOTSUBTSLTD(QString data);
    void analysis_QIOTSUBTSLRD(QString data);
    void analysis_QIOTSUBINFO(QString data);
    void analysis_QIOTSUBHBT(QString data);
    void analysis_QIOTSUBEVT(QString data);



    atcmdAnalysis();
    QList<QPair<QString,QString>> atcmdTextAnalysis(QTextEdit *textEdit);
private:
    QList<QPair<QString,QString>> currentParaList;
    QString removeDoubleQuotationMarks(QString data);

};

#endif // ATCMDANALYSIS_H
