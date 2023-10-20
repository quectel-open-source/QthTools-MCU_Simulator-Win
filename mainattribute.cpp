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
#include "mainattribute.h"
#include <QDebug>
#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QStandardPaths>
#include <QTextCodec>
#include "kernel/mmessagebox.h"
#include <QFont>
//这里需要include掉下面两个头文件
#include "Windows.h"
#include "WinUser.h"
#include <QScreen>

#include "userconfigure.h"



mainAttribute::mainAttribute()
{

}

QRect appRect;
int debugLevel;
QtMessageHandler gDefaultHandler = NULL;
extern QString vitalFilePath;
void customMessageHandler(QtMsgType type,
                          const QMessageLogContext &context,
                          const QString &msg)
{
    //Q_UNUSED(context)
    QDateTime _datetime = QDateTime::currentDateTime();
    QString szDate = _datetime.toString("yyyy-MM-dd hh:mm:ss.zzz");//"yyyy-MM-dd hh:mm:ss ddd"
    QString txt(szDate);
    if(type < debugLevel)
    {
        return;
    }
    switch (type)
    {
        case QtDebugMsg://调试信息提示
        {
            txt += QString(" [Debug] ");
            break;
        }
        case QtInfoMsg://信息输出
        {
            txt += QString(" [Info] ");
            break;
        }
        case QtWarningMsg://一般的warning提示
        {
            txt += QString(" [Warning] ");
            break;
        }
        case QtCriticalMsg://严重错误提示
        {
            txt += QString(" [Critical] ");
            break;
        }
        case QtFatalMsg://致命错误提示
        {
            txt += QString(" [Fatal] ");
            //abort();
            break;
        }
        default:
        {
            txt += QString(" [Trace] ");
            break;
        }
    }
    txt.append( QString(" %1").arg(context.file) );
    txt.append( QString("<%1>: ").arg(context.line) );
    txt.append(msg);

    readWriteLock.lockForWrite();
    QFile debugLogFile;
    QString configfile = vitalFilePath+".debug.ini";
    debugLogFile.setFileName(configfile);
    debugLogFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&debugLogFile);
    text_stream << txt << "\r\n";
    text_stream.flush();
    debugLogFile.close();
    readWriteLock.unlock();
    if(gDefaultHandler)
    {
        gDefaultHandler(type,context,msg);
    }
}
#if 0
#if 1
long __stdcall   callback(_EXCEPTION_POINTERS*   excp)
{
    CCrashStack crashStack(excp);
    QString sCrashInfo = crashStack.GetExceptionInfo();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyyMMddhhmmsszzz");
    QString sFileName =QString("%1/dump_").arg(vitalFilePath)+current_date+".log";

    QFile file(sFileName);
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        file.write(sCrashInfo.toUtf8());
        file.close();
    }
    MMessageBox::critical(NULL,"Dump",QObject::tr("<FONT size=4><div><b>对于发生的错误，表示诚挚的歉意</b><br/></div>"),QObject::tr("确认"));
    return   EXCEPTION_EXECUTE_HANDLER;
}
#else
#include "third/ccrashstack.h"
long __stdcall   callback(_EXCEPTION_POINTERS*   excp)
{
    CCrashStack crashStack(excp);
    QString sCrashInfo = crashStack.GetExceptionInfo();

//    TCHAR my_documents[MAX_PATH];
//    SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, my_documents);
//    QString file_path = QString::fromWCharArray(my_documents);
    QDir dir;
    QString file_path = vitalFilePath;

    QDir *folder_path = new QDir;
    bool exist = folder_path->exists(file_path.append("\\QthTools-MCU_Simulator-Win"));
    if(!exist)
    {
        folder_path->mkdir(file_path);
    }
    delete folder_path;
    folder_path = nullptr;

    QString sFileName = "dump.log";;

    QFile file(sFileName);
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        file.write(sCrashInfo.toUtf8());
        file.close();
    }

    return   EXCEPTION_EXECUTE_HANDLER;
}

#endif
#endif

bool mainAttribute::initAppDirPath()
{

    QDir dir(vitalFilePath);
    if(!dir.exists())
    {
        if(!dir.mkdir(vitalFilePath))
        {
            MMessageBox::critical(NULL,"Dump",QString::fromLocal8Bit("<FONT size=4><div><b>程序无法使用</b><br/></div>"),QString::fromLocal8Bit("确认"));
            return false;
        }
    }
    return true;
}


void mainAttribute::clearLogFile()
{
    readWriteLock.lockForWrite();
    QString configfile = vitalFilePath+".debug.ini";
    QFile file(configfile);
    file.resize(0);
    file.close();
    readWriteLock.unlock();
}

void mainAttribute::adjustFont(QApplication *app)
{
    m_app = app;
    //从系统函数中获取显示器的分辨率，设置字体跟随缩放比例来变化
    const float DEFAULT_DPI = 96.0;
    HDC screen = GetDC(nullptr);
    FLOAT dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
    ReleaseDC(nullptr, screen);
    float fontSize = dpiX / DEFAULT_DPI;


    QFont font = app->font();
    font.setPointSize(font.pointSize()*fontSize);
    app->setFont(font);

}
/**************************************************************************
** 功能	@brief :  更改分辨率响应
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mainAttribute::screenSupport()
{
    //添加Qt的对高分屏的支持
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QList<QScreen*> screens = QApplication::screens();
    if (screens.size() > 0) {
        QScreen* screen = screens[0];
        connect(screen, &QScreen::logicalDotsPerInchChanged, this, &mainAttribute::OnLogicalDotsPerInchChanged);
    }

}
/**************************************************************************
** 功能	@brief :  分辨率 自适用
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void mainAttribute::OnLogicalDotsPerInchChanged(qreal dpi)
{
    qDebug()<<__FUNCTION__<<dpi;
    const float DEFAULT_DPI = 96.0;
//    HDC screen = GetDC(nullptr);
//    FLOAT dpiX = static_cast<FLOAT>(GetDeviceCaps(screen, LOGPIXELSX));
//    ReleaseDC(nullptr, screen);
    float fontSize =  dpi/DEFAULT_DPI;


    QFont font = m_app->font();
    font.setPointSize(font.pointSize()*fontSize);
    m_app->setFont(font);
}

/**************************************************************************
** 功能	@brief :  设置dump日志输出
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void initDebugType(bool mode)
{
    Q_UNUSED(mode);
    debugLevel = 0;
    // 更新dump和debug信息输出驱动
//    ::SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CrashInfocallback);
    gDefaultHandler = qInstallMessageHandler(customMessageHandler);

}
