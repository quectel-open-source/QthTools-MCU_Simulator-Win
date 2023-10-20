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
// completertextedit.cpp
#include "completertextedit.h"
#include "completertextedit.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QKeyEvent>
#include <QString>
#include <QTextCursor>
#include <QDebug>
#include <QListView>

CompleterTextEdit::CompleterTextEdit(QWidget *parent) :
    QTextEdit(parent), m_completer(NULL) {
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void CompleterTextEdit::setCompleter(QCompleter *completer) {
    m_completer = completer;
    m_completer->popup()->setStyleSheet("QAbstractItemView{\
                                            color: rgb(104, 112, 130);\
                                            font-size:14px;\
                                            height: 25px;}\
                                        QAbstractItemView::item\
                                        { \
                                            border-radius:4px;\
                                            padding-left:5px;\
                                            color: rgb(54, 65, 89);\
                                            height: 25px;  \
                                        }");
    m_completer->setPopup(new QListView());
    if (m_completer) {
        m_completer->setWidget(this);
        connect(m_completer, SIGNAL(activated(QString)),
                this, SLOT(onCompleterActivated(QString)));

    }
//    m_completer->installEventFilter(this);
}

void CompleterTextEdit::keyPressEvent(QKeyEvent *e) {
    if (m_completer)
    {
        if (m_completer->popup()->isVisible()) {
            switch(e->key()) {
            case Qt::Key_Escape:
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Tab:
                e->ignore();
                return;
            default:
                break;
            }
        }
        QTextEdit::keyPressEvent(e);
        QString completerPrefix = this->wordUnderCursor();
        m_completer->setCompletionPrefix(completerPrefix); // 通过设置QCompleter的前缀，来让Completer寻找关键词
        m_completer->complete();
    }
}

void CompleterTextEdit::onCompleterActivated(const QString &completion) {
    QString completionPrefix = wordUnderCursor(),
            shouldInertText = completion;
    QTextCursor cursor = this->textCursor();
    if (!completion.contains(completionPrefix)) {// delete the previously typed.
        cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, completionPrefix.size());
        cursor.clearSelection();
    } else { // 补全相应的字符
        shouldInertText = shouldInertText.replace(
            shouldInertText.indexOf(completionPrefix), completionPrefix.size(), "");
    }
    cursor.insertText(shouldInertText);
}

QString CompleterTextEdit::wordUnderCursor() { //不断向左移动cursor，并选中字符，并查看选中的单词中是否含有空格——空格作为单词的分隔符
    QTextCursor cursor = this->textCursor();
    while (cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor)) {
        if (cursor.selectedText().contains(" ")) {
            break;
        }
    }
    return cursor.selectedText().remove(" ");
}
