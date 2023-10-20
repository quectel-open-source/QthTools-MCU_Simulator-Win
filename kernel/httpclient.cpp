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
#include "httpclient.h"
#include <QMessageBox>

HttpClient::HttpClient()
{

}

QString HttpClient::get_request(QString url)
{
    QNetworkRequest request;
    QNetworkAccessManager* naManager = new QNetworkAccessManager();
    QUrl strUrl = url;
    request.setUrl(strUrl);
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    conf.setProtocol(QSsl::TlsV1SslV3);
    request.setSslConfiguration(conf);
    request.setRawHeader("Content-Type", "charset='utf-8'");
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("X-Q-ApiKey", "e27cd509-3f4f-455b-885e-922edccf5568");
    QNetworkReply* reply = naManager->get(request);
    QByteArray responseData;
    QEventLoop eventLoop;
    QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    eventLoop.exec();
    QTextCodec* codec = QTextCodec::codecForName("utf8");
    QString strReply = codec->toUnicode(reply->readAll());
    reply->deleteLater();
    return strReply;
}

QString HttpClient::get_STL_date(QString PK, int cloud)
{
    QString STL_date ="";
    QString url;
    if (cloud == 1)
    {
        url = "https://iot-api.quecteleu.com/v2/quectsl/tsl/mcu/api/product/ability/published/getTslData?productKey=" + PK;
    }
    else if (cloud == 2)
    {
        url = "https://iot-api.quectelus.com/v2/quectsl/tsl/mcu/api/product/ability/published/getTslData?productKey=" + PK;
    }
    else
    {
        url = "https://iot-api.quectelcn.com/v2/quectsl/tsl/mcu/api/product/ability/published/getTslData?productKey=" + PK;
    }
    QString request_info = get_request(url);
    qDebug()<<"平台应答:"<<request_info;

    QJsonDocument tojson;
    QJsonParseError ParseError;
    tojson =QJsonDocument::fromJson(request_info.toUtf8(), &ParseError);
    if(!tojson.isNull() && ParseError.error == QJsonParseError::NoError)
    {
        if(tojson.isObject())
        {
            QJsonObject Object = tojson.object();
            if(!Object.isEmpty())
            {
                if(Object.contains("data"))
                {
                    QJsonObject dateObject = Object.value(QStringLiteral("data")).toObject();
                    if (!dateObject.isEmpty())
                    {
                        if(dateObject.contains("abilityData"))
                        {
                            STL_date = dateObject.value("abilityData").toString();
                        }
                    }
                }
            }
        }
    }

    return STL_date;
}
