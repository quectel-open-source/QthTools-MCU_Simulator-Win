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
#include "hexshowtablewidget.h"
#include <QDebug>
#include <QHeaderView>
#include "itembrokerhex.h"
/**************************************************************************
** 功能	@brief :   自制的qtablewidget控件
** 输入	@param :
** 输出	@retval:
***************************************************************************/

hexShowTableWidget::hexShowTableWidget(QWidget *parent):QTableWidget(parent)
{
    setAppearance();
    connect(this,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(showData(QTableWidgetItem*)));
    this->setFocusPolicy(Qt::StrongFocus);
    keyInputSum =0;
}

#include <QItemDelegate>

class ReadOnlyDelegate: public QItemDelegate
{

public:
    ReadOnlyDelegate(QWidget *parent = NULL):QItemDelegate(parent){}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
const QModelIndex &index) const override //final
    {
        Q_UNUSED(parent)
        Q_UNUSED(option)
        Q_UNUSED(index)
        return NULL;
    }
};
/**************************************************************************
** 功能	@brief :   属性设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void hexShowTableWidget::setAppearance()
{
    this->setColumnCount(11);
    this->setRowCount(1);
    this->setHorizontalHeaderLabels(QStringList()<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"10"<<tr("字符串"));
    columSum = 11;
//    qDebug()<<this->columnCount();
    int width = this->geometry().width();
    int itemWidth;
    if(width < 10*10+40)
    {
        itemWidth = 10;
    }
    else
    {
        itemWidth = width/14;
    }
    for(int i=0;i<this->columnCount()-1;i++)
        this->setColumnWidth(i,itemWidth);
    this->setColumnWidth(this->columnCount(),width-itemWidth*10);
    this->horizontalHeader()->setStretchLastSection(true);
    this->setStyleSheet("QTableCornerButton::section{background:skyblue;}");
    this->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
    this->verticalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
//    this->setFocusPolicy(Qt::StrongFocus);
    this->setEditTriggers(QAbstractItemView::CurrentChanged|QAbstractItemView::SelectedClicked);
//    法一：代理
    itemBrokerHex *itembroker = new itemBrokerHex();
    for(int i=0;i<columSum-1;i++)
        this->setItemDelegateForColumn(i,itembroker);
    ReadOnlyDelegate* readOnlyDelegate = new ReadOnlyDelegate();
    this->setItemDelegateForColumn(10,readOnlyDelegate);
}

/**************************************************************************
** 功能	@brief :   键盘按键释放事件处理(这里存在与代理不协调的问题)
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void hexShowTableWidget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Return && this->currentItem() != NULL)
    {
        EnterNextEvent();
    }
    else if(event->key() == Qt::Key_Backspace)
    {
        if(keyInputSum > 0)
        {
            keyInputSum--;
        }
    }
    else if((event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) || (event->key() >= Qt::Key_A && event->key() <= Qt::Key_F))
    {
        if(keyInputSum < 2)
        {
            keyInputSum++;
        }
        if(keyInputSum == 2)
        {
            QList<QLineEdit *> lineEdit = this->findChildren<QLineEdit *>();
            if (!lineEdit.isEmpty())
            {
                if (lineEdit.at(0)->text().size() == 2)
                {
                    EnterNextEvent();
                }
                else
                {
                    keyInputSum = lineEdit.at(0)->text().length();
                }
            }
        }
    }
}
/**************************************************************************
** 功能	@brief :   item下一格
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void hexShowTableWidget::EnterNextEvent(bool flag)
{
    Q_UNUSED(flag);
    this->focusNextPrevChild(true);
    if(this->currentColumn() == 10)
    {
        if(this->currentRow()+1 >= this->rowCount())
            this->insertRow(this->rowCount());
        this->focusNextPrevChild(true);
    }
}

/**************************************************************************
** 功能	@brief :   数据显示到最后一列空格
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void hexShowTableWidget::showData(QTableWidgetItem *item)
{

    int currentColume = item->column();
    int currentRow = item->row();
    if(keyInputSum == 2 && columSum-1 != currentColume)
    {
        if(item->text().count() == 1)
        {
            this->focusNextPrevChild(false);
        }
    }
    if(columSum-1 != currentColume)
    {
        QTableWidgetItem *itemLast = new QTableWidgetItem();
        QString dataAll;
        for(int i =0;i<columSum-1;i++)
        {
            if(this->item(currentRow,i) != NULL)
            {
                QByteArray  data = this->item(currentRow,i)->text().toLatin1();
                if(!data.isEmpty())
                {
                    bool ok;
                    int num = data.toInt(&ok,16);
                    char number = char(num);
                    dataAll.append(QString(number));
                }
            }
        }
        itemLast->setText(dataAll);
        this->setItem(currentRow,columSum-1,itemLast);
        keyInputSum = 0;
    }
    this->show();

}



