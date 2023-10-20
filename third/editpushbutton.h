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
#ifndef EDITPUSHBUTTON_H
#define EDITPUSHBUTTON_H
#include <QPushButton>
#include <QLineEdit>
class editPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit editPushButton(QWidget *parent = 0);
    explicit editPushButton(const QString &text, QWidget *parent = nullptr);
signals:
    void buttonTextChange(int,QString);
private:
    bool eventFilter(QObject *watched, QEvent *event);

    QLineEdit *lineEdit = NULL;
    int mouseHoverSec = 0;
    bool isShowTootip = false;
};

#endif // EDITPUSHBUTTON_H
