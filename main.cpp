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
#include "mainwindow.h"
#pragma execution_character_set("utf-8")
#include "kernel/mmessagebox.h"
#include <QSystemSemaphore>
#include <QSharedMemory>
#include "userconfigure.h"
#include "LogModule/ccrashstack.h"
QTranslator *m_translator =NULL;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font("黑体", 10);
    a.setFont(font);
    // 创建信号量
    QSystemSemaphore semaphore("SingleAppTest2Semaphore", 1);
    // 启用信号量，禁止其他实例通过共享内存一起工作
    semaphore.acquire();

#ifndef Q_OS_WIN32
    // 在linux / unix 程序异常结束共享内存不会回收
    // 在这里需要提供释放内存的接口，就是在程序运行的时候如果有这段内存 先清除掉
    QSharedMemory nix_fix_shared_memory("SingleAppTest2");
    if (nix_fix_shared_memory.attach())
    {undefined
        nix_fix_shared_memory.detach();
    }
#endif
    // 创建一个共享内存  “SingleAppTest2”表示一段内存的标识key 可作为唯一程序的标识
    QSharedMemory sharedMemory("QthTools-MCU_Simulator-Win");
    // 测试是否已经运行
    bool isRunning = false;
    // 试图将共享内存的副本附加到现有的段中。
    if (sharedMemory.attach())
    {
        // 如果成功，则确定已经存在运行实例
        isRunning = true;
    }
    else
    {
        // 否则申请一字节内存
        sharedMemory.create(1);
        // 确定不存在运行实例
        isRunning = false;
    }
    semaphore.release();

    //获取系统语言与加载语言包
    QLocale locale;
    if(NULL != m_translator)
    {
        qApp->removeTranslator(m_translator);
        delete m_translator;
        m_translator = NULL;
    }
    m_translator = new QTranslator();
    if(locale.language() == QLocale::English)
    {
        m_translator->load(":/english.qm");
        qApp->installTranslator(m_translator);
    }
    else
    {
        m_translator->load(":/chinese.qm");
        qApp->installTranslator(m_translator);
    }

    // 如果您已经运行了应用程序的一个实例，那么我们将通知用户。
    if (isRunning)
    {
        MMessageBox msgBox(MMessageBox::Warning,QObject::tr("提示"),QObject::tr("已有重复程序处于运行中状态！"),QObject::tr("确认"));
        msgBox.exec();
        return 1;
    }
    //日志模块，开始日志储存
    OpenLog *openlog = new OpenLog();
    openlog->setLog();

    MainWindow w;

    w.setAttribute(Qt::WA_QuitOnClose,true);
    w.show();
    return a.exec();
}
