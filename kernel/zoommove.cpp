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
#include "zoommove.h"
#include <QEvent>
#include <QHoverEvent>
#include <QDebug>

ZoomMove::ZoomMove(QWidget *parent):
    QWidget(parent) {
    key_down =      //默认鼠标为松开状态
    zoom =          //默认无拉扯操作
    move = 0;       //默认无移动操作
    is_on_edge = 0; //默认鼠标没有在边停靠
    father = parent;
    father->installEventFilter(this); //开启事件过滤
}

ZoomMove::~ZoomMove() {
    if (widget != NULL)
    {
        widget->removeEventFilter(this);  //关闭事件过滤
    }
}

void ZoomMove::updateCursor()
{
    switch(is_on_edge)
    {
        case TOPLEFT:     //左上方
        case BOTTOMRIGHT:     //右下方
            widget -> setCursor(Qt::SizeFDiagCursor);   //左上-右下
            break;
        case TOPRIGHT:     //右上方
        case BOTTOMLEFT:     //左下方
            widget -> setCursor(Qt::SizeBDiagCursor);   //右上-左下
            break;
        case TOP:     //上边
        case BOTTOM:     //下边
            widget -> setCursor(Qt::SizeVerCursor);     //上-下
            break;
        case RIGHT:     //右边
        case LEFT:     //左边
            widget -> setCursor(Qt::SizeHorCursor);     //左-右
            break;
        case EDGENULL:
        default:
            widget -> setCursor(Qt::ArrowCursor);       //箭头
            break;
    }
}

void ZoomMove::mouseIsOnEdge(QPoint point, QRect rect) {
    if((point.x() - rect.x() < 5) && (point.y() - rect.y() < 5)) {
        is_on_edge = TOPLEFT; //左上方
    }
    else if((rect.x() + rect.width() - point.x() < 5) && (point.y() - rect.y() < 5)) {
        is_on_edge = TOPRIGHT; //右上方
    }
    else if((rect.x() + rect.width() - point.x() < 5) && (rect.y() + rect.height() - point.y() < 5)) {
        is_on_edge = BOTTOMRIGHT; //右下方
    }
    else if((point.x() - rect.x() < 5) && (rect.y() + rect.height() - point.y() < 5)) {
        is_on_edge = BOTTOMLEFT; //左下方
    }
    else if(point.y() - rect.y() < 5) {
        is_on_edge = TOP; //上边
    }
    else if(rect.x() + rect.width() - point.x() < 5) {
        is_on_edge = RIGHT; //右边
    }
    else if(rect.y() + rect.height() - point.y() < 5) {
        is_on_edge = BOTTOM; //下边
    }
    else if(point.x() - rect.x() < 5) {
        is_on_edge = LEFT; //左边
    }
    else {
        is_on_edge = EDGENULL; //不沾边
    }

    zoom = is_on_edge == EDGENULL ? 0 : 1;
}

void ZoomMove::resizeWidget(QPoint p)
{
    min_width = widget -> minimumWidth();
    min_height = widget -> minimumHeight();

    QRect frame = widget -> frameGeometry(); //获取窗口的几何框架

    switch(is_on_edge)
    {    //根据边框的位置改变窗口的形状
        case TOPLEFT:     //左上方
            temp_point = frame.topLeft();
            if(frame.bottomRight().x() - p.x() > min_width) { temp_point.setX(p.x()); }
            if(frame.bottomRight().y() - p.y() > min_height) { temp_point.setY(p.y()); }
            if(frame.bottomRight().x() - p.x() <= min_width && frame.bottomRight().y() - p.y() <= min_height) {
                temp_point.setX(frame.bottomRight().x() - min_width);
                temp_point.setY(frame.bottomRight().y() - min_height);
            }
            frame.setTopLeft(temp_point);
            break;

        case TOPRIGHT:     //右上方
            temp_point = frame.topRight();
            if  (p.x() - frame.bottomLeft().x() > 0)
            {
                temp_point.setX(p.x());
            }
            if (frame.bottomLeft().y() - p.y() > min_height)
            {
                temp_point.setY(p.y());
            }
            if (p.x() - frame.bottomLeft().x() <= 0 && frame.bottomLeft().y() - p.y() <= min_height)
            {
                temp_point.setX(p.x());
                temp_point.setY(frame.bottomLeft().y() - min_height);
            }
            frame.setTopRight(temp_point);
            break;

        case BOTTOMRIGHT:   //右下方
            temp_point = frame.bottomRight();
            if (p.x() - frame.topLeft().x() > 0)
            {
                temp_point.setX(p.x());
            }
            if (p.y() - frame.topLeft().y() > 0)
            {
                temp_point.setY(p.y());
            }
            if (p.x() - frame.topLeft().x() <= 0 && p.y() - frame.topLeft().y() <= 0)
            {
                temp_point.setX(p.x());
                temp_point.setY(p.y());
            }
            frame.setBottomRight(temp_point);
            break;

        case BOTTOMLEFT:    //左下方
            temp_point = frame.bottomLeft();
            if (frame.topRight().x() - p.x() > min_width)
            {
                temp_point.setX(p.x());
            }
            if (p.y() - frame.topRight().y() > 0)
            {
                temp_point.setY(p.y());
            }
            if (frame.topRight().x() - p.x() <= min_width && p.y() - frame.topRight().y() <= 0)
            {
                temp_point.setX(frame.topRight().x() - min_width);
                temp_point.setY(p.y());
            }
            frame.setBottomLeft(temp_point);
            break;

        case TOP:           //上边
            temp_point = frame.topRight();
            if (frame.bottomLeft().y() - p.y() > min_height)
            {
                temp_point.setY(p.y());
            }
            frame.setTopRight(temp_point);
            break;

        case RIGHT:         //右边
            temp_point = frame.bottomRight();
            if (p.x() - frame.topLeft().x() > 0)
            {
                temp_point.setX(p.x());
            }
            frame.setBottomRight(temp_point);
            break;

        case BOTTOM:        //下边
            temp_point = frame.bottomLeft();
            if (p.y() - frame.topRight().y() > 0)
            {
                temp_point.setY(p.y());
            }
            frame.setBottomLeft(temp_point);
            break;

        case LEFT:          //左边
            temp_point = frame.topLeft();
            if (frame.bottomRight().x() - p.x() > min_width)
            {
                temp_point.setX(p.x());
            }
            frame.setTopLeft(temp_point);
            break;

        default:
            break;
    }
    widget -> setGeometry(frame);    //给窗口设置新的几何框架
}

void ZoomMove::widgetEventHandler(QEvent *e)
{
    switch(e -> type())
    {
        case QEvent::HoverMove:
            mouseHoverEventHandler(static_cast<QHoverEvent *>(e));
            break;
        case QEvent::MouseMove:
            mouseMoveEventHandler(static_cast<QMouseEvent *>(e));
            break;
        case QEvent::MouseButtonPress:
            mousePressEventHandler(static_cast<QMouseEvent *>(e));
            break;
        case QEvent::MouseButtonRelease:
            mouseReleaseEventHandler(static_cast<QMouseEvent *>(e));
            break;
        default:
            break;
    }
}

void ZoomMove::mouseHoverEventHandler(QHoverEvent *e)
{
    if(!key_down)     //在窗口被拉扯或移动的过程中，停止对鼠标的悬浮监听
    {
        point = widget -> mapToGlobal(e -> pos());

        mouseIsOnEdge(point, widget -> frameGeometry());
        updateCursor();
    }
}

void ZoomMove::mouseMoveEventHandler(QMouseEvent *e)
{
    Q_UNUSED(e);
    if(key_down)
    {
        current_point = e -> globalPos();

        //current_point - last_point 即窗口的左上角顶点
        if(move && last_point.y() < 56) {
            widget->move(current_point - last_point);
        }

        if(zoom) {
            resizeWidget(current_point);
        }
    }
}

void ZoomMove::mousePressEventHandler(QMouseEvent *e)
{
    if(e -> button() == Qt::LeftButton && !father->isMaximized())
    {
        key_down = 1;
        move = (!zoom && !move) & 1;
        last_point = e -> pos();
    }
}

void ZoomMove::mouseReleaseEventHandler(QMouseEvent *e)
{
    if(e -> button() == Qt::LeftButton && !father->isMaximized())
    {
        move = 0;
        key_down = move;       //更正移动状态
    }
}

bool ZoomMove::eventFilter(QObject *obj, QEvent *e)
{
    switch(e -> type())
    {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::HoverMove:
            widget = static_cast<QWidget *>(obj);
            widgetEventHandler(e);
            return true;
        default:
            return QObject::eventFilter(obj, e);
    }
}

