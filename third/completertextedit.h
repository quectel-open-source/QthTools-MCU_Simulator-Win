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
#ifndef COMPLETERTEXTEDIT_H
#define COMPLETERTEXTEDIT_H

#include <QTextEdit>
#include <QString>
#include <QAbstractItemView>
#include <QItemDelegate>

QT_BEGIN_NAMESPACE

class QCompleter;

QT_END_NAMESPACE

class CompleterTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit CompleterTextEdit(QWidget *parent = 0);
    void setCompleter(QCompleter *completer);

protected:
    void keyPressEvent(QKeyEvent *e); // 响应按键盘事件

private slots:
    void onCompleterActivated(const QString &completion); // 响应选中QCompleter中的选项后，QCompleter发出的activated()信号

private:
    QString wordUnderCursor(); // 获取当前光标所在的单词

private:
    QCompleter *m_completer;
    QList<int> inputNumList;

};

#endif // COMPLETERTEXTEDIT_H
