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
#ifndef ZOOMMOVE_H
#define ZOOMMOVE_H

#include <QWidget>

enum {
    EDGENULL = 0,
    TOPLEFT,
    TOPRIGHT,
    BOTTOMRIGHT,
    BOTTOMLEFT,
    TOP,
    RIGHT,
    BOTTOM,
    LEFT
};

class ZoomMove : public QWidget {
    Q_OBJECT

public:
    explicit ZoomMove(QWidget *parent = nullptr);
    ~ZoomMove();

private:
    bool key_down,      //0:松开, 1:按下
         zoom,          //0:无操作, 1:拉扯
         move;          //0:无操作, 1:移动

    int is_on_edge,     //0:不沾边, 1:上边, 2:右上方 , 3:右边 , 4:右下方 , 5:下边 , 6:左下方 , 7:左边 , 8:左上方
        left,
        top,
        right,
        bottom,
        min_width,
        min_height;

    QPoint last_point,      //当前窗口的左上方顶点
           current_point,   //当前鼠标相对于屏幕的位置，不能将 current_point 与 point 看作同一坐标点，后者没有前者稳定
           point,           //当前鼠标在屏幕中的位置
           temp_point;      //用于存放窗口的四个顶点

    QWidget *widget;

    void updateCursor();                            //更新鼠标图标
    void mouseIsOnEdge(QPoint point, QRect rect);   //判断鼠标是否在边停靠
    void resizeWidget(QPoint p);                    //根据拉伸重置窗口大小
    void widgetEventHandler(QEvent *e);             //窗口事件处理
    void mouseHoverEventHandler(QHoverEvent *e);    //鼠标停靠窗口
    void mouseMoveEventHandler(QMouseEvent *e);     //鼠标移动
    void mousePressEventHandler(QMouseEvent *e);    //鼠标被按下
    void mouseReleaseEventHandler(QMouseEvent *e);  //鼠标被松开

protected:
    QWidget *father;
    virtual bool eventFilter(QObject *obj, QEvent *e);
};


#endif // ZOOMMOVE_H
