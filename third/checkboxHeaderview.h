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
#ifndef CHECKBOXHEADERVIEW_H
#define CHECKBOXHEADERVIEW_H

#include <QtGui>
#include <QPainter>
#include <QHeaderView>
#include <QStyleOptionButton>
#include <QStyle>
#include <QCheckBox>

/// 复选框表头

class CheckBoxHeaderView : public QHeaderView
{
    Q_OBJECT
private:
    int     m_checkColIndex;    //列下标
    QPoint  m_topLeft;          //勾选框起始坐标
    QSize   m_checkSize;        //勾选框大小
    bool    m_isChecked;        //勾选框状态
public:
    CheckBoxHeaderView( int checkColumnIndex, QPoint topLeft, QSize size, Qt::Orientation orientation, QWidget * parent = 0) : QHeaderView(orientation, parent)
    {
        m_checkColIndex = checkColumnIndex;
        m_topLeft = topLeft;
        m_checkSize = size;
        m_isChecked = false;
    }

    void setCheckState(bool state)
    {
        m_isChecked = state;
    }

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
    {
        painter->save();
        QHeaderView::paintSection(painter, rect, logicalIndex);
        painter->restore();
        if (logicalIndex == m_checkColIndex)
        {
            QStyleOptionButton option;
            int width = 10;
            for (int i=0; i<logicalIndex; ++i)
            {
                width += sectionSize( i );
            }
            option.rect = QRect(m_topLeft.x(), m_topLeft.y(), m_checkSize.width(), m_checkSize.height());
            if (m_isChecked)
            {
                option.state = QStyle::State_On;
            }
            else
            {
                option.state = QStyle::State_Off;
            }
            QCheckBox *check = new QCheckBox;
            QString sheet = QString("QCheckBox::indicator {width: %1px;  height: %2px;}").arg(m_checkSize.width()).arg(m_checkSize.height());
            check->setStyleSheet(sheet);
            this->style()->drawControl(QStyle::CE_CheckBox, &option, painter, check);
        }
    }

    void mousePressEvent(QMouseEvent *event)
    {
        if (visualIndexAt(event->pos().x()) == m_checkColIndex)
        {
            m_isChecked = !m_isChecked;
            this->updateSection(m_checkColIndex);
            emit checkStatusChange(m_isChecked);
        }
        QHeaderView::mousePressEvent(event);
    }
signals:
    void checkStatusChange(bool);
};

#endif // CHECKBOXHEADERVIEW_H

