﻿/*
 * Copyright (c) 2014-2021 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QDateTime>
#include <QMessageBox>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QNetworkAccessManager>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

#include <math.h>
#include "Downloader.h"
#include "../include/updateConfig.h"
#include "kernel/mmessagebox.h"

static const QString PARTIAL_DOWN(".part");

Downloader::Downloader(QWidget *parent)
   : QWidget(parent)
{
   m_ui = new Ui::Downloader;
   m_ui->setupUi(this);

   /* Initialize private members */
   m_manager = new QNetworkAccessManager();

   /* Initialize internal values */
   m_url = "";
   m_fileName = "";
   m_startTime = 0;
   m_useCustomProcedures = false;
   m_mandatoryUpdate = false;
   m_cancelInstall = false;
   /* Set download directory */
   m_downloadDir.setPath(vitalFilePath);

   /* Make the window look like a modal dialog */
   setWindowIcon(QIcon());
   setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

   /* Configure the appearance and behavior of the buttons */
//   m_ui->openButton->setEnabled(false);
//   m_ui->openButton->setVisible(false);
//   m_ui->stopButton->setEnabled(false);
//   m_ui->stopButton->setVisible(false);
   connect(m_ui->stopButton, SIGNAL(clicked()), this, SLOT(cancelDownload()));
//   connect(m_ui->openButton, SIGNAL(clicked()), this, SLOT(installUpdate()));
   /* Resize to fit */
   setFixedSize(minimumSizeHint());

   //去除窗体边框
   this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
   //窗体关闭信号
   connect(m_ui->pushButton_close, &QPushButton::clicked, [=]() {close(); });
   this->setWindowModality(Qt::ApplicationModal);
   this->setAttribute(Qt::WA_TranslucentBackground, true);
}

void Downloader::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    cancelDownload();
}

Downloader::~Downloader()
{
   delete m_ui;
   delete m_reply;
   delete m_manager;
}

/**************************************************************************
** 功能	@brief :  鼠标按下事件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void Downloader::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    //判断是否是鼠标左键信号，是否处于最大化状态
    if(event->button() == Qt::LeftButton &&  !this->isMaximized())
    {
        if (m_ui->widget_windowTitle->geometry().contains(this->mapFromGlobal(QCursor::pos())))
        {
            m_bDrag = true;
            mouseStartPoint = event->globalPos();
            windowTopLeftPoint = this->frameGeometry().topLeft();
        }
    }
}

void Downloader::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bDrag)
    {
        //获得鼠标移动的距离
        QPoint distance = event->globalPos() - mouseStartPoint;
        //QPoint distance = event->pos() - mouseStartPoint;
        //改变窗口的位置
        this->move(windowTopLeftPoint + distance);
    }
}

void Downloader::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bDrag = false;
    }
}

/**
 * Returns \c true if the updater shall not intervene when the download has
 * finished (you can use the \c QSimpleUpdater signals to know when the
 * download is completed).
 */
bool Downloader::useCustomInstallProcedures() const
{
    qDebug()<<__FUNCTION__;
   return m_useCustomProcedures;
}

/**
 * Changes the URL, which is used to indentify the downloader dialog
 * with an \c Updater instance
 *
 * \note the \a url parameter is not the download URL, it is the URL of
 *       the AppCast file
 */
void Downloader::setUrlId(const QString &url)
{
   m_url = url;
}

/**
 * Begins downloading the file at the given \a url
 */
void Downloader::startDownload(const QUrl &url)
{
   /* Reset UI */
   this->setWindowTitle(tr("更新"));
   m_ui->progressBar->setValue(0);
   m_ui->downloadLabel->setText(tr("下载更新"));
   m_ui->timeLabel->setText(tr("剩余时长") + ": " + tr("未知"));

   /* Configure the network request */
   QNetworkRequest request(url);

   request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

   if (!m_userAgentString.isEmpty())
      request.setRawHeader("User-Agent", m_userAgentString.toUtf8());

   /* Start download */
   m_reply = m_manager->get(request);
   m_startTime = QDateTime::currentDateTime().toSecsSinceEpoch();

   /* Ensure that downloads directory exists */
   if (!m_downloadDir.exists())
      m_downloadDir.mkpath("./cache/");

   /* Remove old downloads */
   QFile::remove(m_downloadDir.filePath(m_fileName));
   QFile::remove(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN));

   /* Update UI when download progress changes or download finishes */
   connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));

//   connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));

   connect(m_reply, SIGNAL(finished()), this, SLOT(finished()));

//   timerCheckDownload = new QTimer();
//   connect(timerCheckDownload,SIGNAL(timeout()),this,SLOT(downLoadTimeOut()));
//   timerCheckDownload->start(10000);

   showNormal();
}

/**
 * Changes the name of the downloaded file
 */
void Downloader::setFileName(const QString &file)
{
   m_fileName = file;

   if (m_fileName.isEmpty())
      m_fileName = "QSU_Update.bin";
}

/**
 * Changes the user-agent string used to communicate with the remote HTTP server
 */
void Downloader::setUserAgentString(const QString &agent)
{
   m_userAgentString = agent;
}

void Downloader::finished()
{
   if(m_reply->error() == QNetworkReply::NoError)
   {
       /* Rename file */
       QFile::rename(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN), m_downloadDir.filePath(m_fileName));

       /* Notify application */
       emit downloadFinished(m_url, m_downloadDir.filePath(m_fileName));

       /* Install the update */
       m_reply->close();
       if(false == m_cancelInstall)
       {
            installUpdate();
       }
       setVisible(false);
   }
   else
   {
       if (!m_mandatoryUpdate)
       {
           MMessageBox::critical(this,tr("错误"),tr("下载失败，请重试！"),tr("确认"));
       }
       QFile::remove(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN));
       m_reply->close();
       setVisible(false);
   }
}

/**
 * Opens the downloaded file.
 * \note If the downloaded file is not found, then the function will alert the
 *       user about the error.
 */
void Downloader::openDownload()
{
   if (!m_fileName.isEmpty())
   {
//      QDesktopServices::openUrl(QUrl::fromLocalFile(m_downloadDir.filePath(m_fileName)));
       emit updateExeEvent(m_fileName);
   }
   else
   {
      MMessageBox::critical(this, tr("错误"), tr("无法找到更新固件！"), tr("确认"));
   }
}

/**
 * Instructs the OS to open the downloaded file.
 *
 * \note If \c useCustomInstallProcedures() returns \c true, the function will
 *       not instruct the OS to open the downloaded file. You can use the
 *       signals fired by the \c QSimpleUpdater to install the update with your
 *       own implementations/code.
 */
void Downloader::installUpdate()
{
   if (useCustomInstallProcedures())
      return;
    qDebug()<<"installUpdate";
    if (!useCustomInstallProcedures())
       openDownload();
}

/**
 * Prompts the user if he/she wants to cancel the download and cancels the
 * download if the user agrees to do that.
 */
void Downloader::cancelDownload()
{
   if (!m_reply->isFinished())
   {
        hide();
        m_cancelInstall = true;
        m_reply->abort();
        if (m_mandatoryUpdate)
        {
            MMessageBox::warning(this, tr("错误"), tr("升级失败，程序进程无法使用"),tr("确认"));
            QApplication::quit();
        }
   }
   else
   {
       if (m_mandatoryUpdate)
       {
           QApplication::quit();
       }
       hide();
   }
}

/**
 * Writes the downloaded data to the disk
 */
void Downloader::saveFile(qint64 received, qint64 total)
{
   Q_UNUSED(received);
   Q_UNUSED(total);
   /* Check if we need to redirect */
   QUrl url = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
   if (!url.isEmpty())
   {
      startDownload(url);
      return;
   }

   /* Save downloaded data to disk */
   QFile file(m_downloadDir.filePath(m_fileName + PARTIAL_DOWN));
   if (file.open(QIODevice::WriteOnly | QIODevice::Append))
   {
      file.write(m_reply->readAll());
      file.close();
   }
}

/**
 * Calculates the appropiate size units (bytes, KB or MB) for the received
 * data and the total download size. Then, this function proceeds to update the
 * dialog controls/UI.
 */
void Downloader::calculateSizes(qint64 received, qint64 total)
{
   QString totalSize;
   QString receivedSize;

   if (total < 1024)
      totalSize = tr("%1 bytes").arg(total);

   else if (total < 1048576)
      totalSize = tr("%1 KB").arg(round(total / 1024));

   else
      totalSize = tr("%1 MB").arg(round(total / 1048576));

   if (received < 1024)
      receivedSize = tr("%1 bytes").arg(received);

   else if (received < 1048576)
      receivedSize = tr("%1 KB").arg(received / 1024);

   else
      receivedSize = tr("%1 MB").arg(received / 1048576);

   m_ui->downloadLabel->setText(tr("下载更新") + " (" + receivedSize + " " + tr("of") + " " + totalSize
                                + ")");
}

/**
 * Uses the \a received and \a total parameters to get the download progress
 * and update the progressbar value on the dialog.
 */
void Downloader::updateProgress(qint64 received, qint64 total)
{
   qDebug()<<"received:"<<received<<"total:"<<total;
   if (total > 0)
   {
      m_ui->progressBar->setMinimum(0);
      m_ui->progressBar->setMaximum(100);
      m_ui->progressBar->setValue((received * 100) / total);

      calculateSizes(received, total);
      calculateTimeRemaining(received, total);
      saveFile(received, total);
   }
   else
   {
      m_ui->progressBar->setMinimum(0);
      m_ui->progressBar->setMaximum(0);
      m_ui->progressBar->setValue(-1);
      m_ui->downloadLabel->setText(tr("更新中") + "...");
      m_ui->timeLabel->setText(QString("%1: %2").arg(tr("剩余时长")).arg(tr("未知")));
   }
}

/**
 * Uses two time samples (from the current time and a previous sample) to
 * calculate how many bytes have been downloaded.
 *
 * Then, this function proceeds to calculate the appropiate units of time
 * (hours, minutes or seconds) and constructs a user-friendly string, which
 * is displayed in the dialog.
 */
void Downloader::calculateTimeRemaining(qint64 received, qint64 total)
{
   uint difference = QDateTime::currentDateTime().toSecsSinceEpoch() - m_startTime;

   if (difference > 0)
   {
      QString timeString;
      qreal timeRemaining = (total - received) / (received / difference);

      if (timeRemaining > 7200)
      {
         timeRemaining /= 3600;
         int hours = int(timeRemaining + 0.5);

         if (hours > 1)
            timeString = tr("大约 %1 小时").arg(hours);
         else
            timeString = tr("大约 1 小时");
      }

      else if (timeRemaining > 60)
      {
         timeRemaining /= 60;
         int minutes = int(timeRemaining + 0.5);

         if (minutes > 1)
            timeString = tr("%1 分钟").arg(minutes);
         else
            timeString = tr("1 分钟");
      }

      else if (timeRemaining <= 60)
      {
         int seconds = int(timeRemaining + 0.5);

         if (seconds > 1)
            timeString = tr("%1 秒").arg(seconds);
         else
            timeString = tr("1 秒");
      }

      m_ui->timeLabel->setText(tr("剩余时长") + ": " + timeString);
   }
}

/**
 * Rounds the given \a input to two decimal places
 */
qreal Downloader::round(const qreal &input)
{
   return static_cast<qreal>(roundf(static_cast<float>(input) * 100) / 100);
}

QString Downloader::downloadDir() const
{
   return m_downloadDir.absolutePath();
}

void Downloader::setDownloadDir(const QString &downloadDir)
{
   if (m_downloadDir.absolutePath() != downloadDir)
       m_downloadDir.setPath(downloadDir);
}

void Downloader::setCancelInstall(const bool mode)
{
    m_cancelInstall = mode;
}

/**
 * If the \a mandatory_update is set to \c true, the \c Downloader has to download and install the
 * update. If the user cancels or exits, the application will close
 */
void Downloader::setMandatoryUpdate(const bool mandatory_update)
{
   m_mandatoryUpdate = mandatory_update;
}

/**
 * If the \a custom parameter is set to \c true, then the \c Downloader will not
 * attempt to open the downloaded file.
 *
 * Use the signals fired by the \c QSimpleUpdater to implement your own install
 * procedures.
 */
void Downloader::setUseCustomInstallProcedures(const bool custom)
{
   m_useCustomProcedures = custom;
}

#if QSU_INCLUDE_MOC
#   include "moc_Downloader.cpp"
#endif
