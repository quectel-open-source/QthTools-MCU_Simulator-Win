QT       += core gui
QT       += network
QT       += serialport
QT       += axcontainer
QT       += concurrent
QT       += sql
QT      += multimedia
QT      += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include ($$PWD/third/QSimpleUpdater/QSimpleUpdater.pri)
include ($$PWD/third/ttlvExchangeJson/ttlvExchangeJson.pri)
include ($$PWD/third/LogModule/LogModule.pri)

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/third
INCLUDEPATH += $$PWD/third/externWidget

CONFIG += c++11
CONFIG += resources_big

QMAKE_CFLAGS_RELEASE += -g
QMAKE_CXXFLAGS_RELEASE += -g
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_LFLAGS_RELEASE = -mthreads -W

#DEFINES += QSU_INCLUDE_MOC=1
#DEFINES += Qt_NO_CAST_FROM_ ASCII

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    app/atcmdwindow.cpp \
    app/lanwindow.cpp \
    app/mutilstringsend.cpp \
    app/productconfig.cpp \
    app/subdevwindow.cpp \
    kernel/athub.cpp \
    kernel/externmath.cpp \
    kernel/mapList.cpp \
    kernel/mmessagebox.cpp \
    kernel/myObjMessApater.cpp \
    kernel/myObjMessServer.cpp \
    kernel/mysqlite.cpp \
#    kernel/toolupdate.cpp \
    kernel/userconfigsave.cpp \
    kernel/zoommove.cpp \
    mainattribute.cpp \
    subwindow/addconfigwindow.cpp \
    kernel/httpclient.cpp \
    kernel/atcmdanalysis.cpp \
    kernel/atcmdprocess.cpp \
    kernel/configprocess.cpp \
    kernel/filemanager.cpp \
    kernel/jsonFile.cpp \
    kernel/modelprocess.cpp \
    kernel/multithread.cpp \
    kernel/serial.cpp \
    kernel/ttlvanylsis.cpp \
    app/configwindow.cpp \
    app/debugwindow.cpp \
    subwindow/hexstringshow.cpp \
    app/infowindow.cpp \
    main.cpp \
    mainwindow.cpp \
    app/modelwindow.cpp \
    app/otawindow.cpp \
    app/passwindow.cpp \
    subwindow/inputdialog.cpp \
    subwindow/stringshow.cpp \
    subwindow/subdevcomm.cpp \
    subwindow/subdevcreate.cpp \
#    third/ccrashstack.cpp \
    third/completertextedit.cpp \
    third/editpushbutton.cpp \
    third/hexshowtablewidget.cpp \
    third/itembrokerhex.cpp \
    third/jsoninfomanage.cpp \
    third/newcombobox.cpp \
    third/newqlineedit.cpp \
    third/switchbutton.cpp \
    userconfigure.cpp

HEADERS += \
    app/app_config.h \
    app/atcmdwindow.h \
    app/lanwindow.h \
    app/mutilstringsend.h \
    app/productconfig.h \
    app/subdevwindow.h \
    baseQTConfigure.h \
    kernel/athub.h \
    kernel/externmath.h \
    kernel/kernel_config.h \
    kernel/mapList.h \
    kernel/mmessagebox.h \
    kernel/myObjMessApater.h \
    kernel/myObjMessServer.h \
    kernel/mysqlite.h \
#    kernel/toolupdate.h \
    kernel/userconfigsave.h \
    kernel/zoommove.h \
    mainattribute.h \
    subwindow/addconfigwindow.h \
    kernel/httpclient.h \
    kernel/atcmdanalysis.h \
    kernel/atcmdprocess.h \
    kernel/configprocess.h \
    kernel/filemanager.h \
    kernel/jsonFile.h \
    kernel/modelprocess.h \
    kernel/multithread.h \
    kernel/serial.h \
    kernel/ttlvanylsis.h \
    app/configwindow.h \
    app/debugwindow.h \
    subwindow/hexstringshow.h \
    app/infowindow.h \
    mainwindow.h \
    app/modelwindow.h \
    app/otawindow.h \
    app/passwindow.h \
    subwindow/inputdialog.h \
    subwindow/stringshow.h \
    subwindow/subdevcomm.h \
    subwindow/subdevcreate.h \
#    third/ccrashstack.h \
    third/checkboxHeaderview.h \
    third/completertextedit.h \
    third/editpushbutton.h \
    third/hexshowtablewidget.h \
    third/itembrokerhex.h \
    third/jsoninfomanage.h \
    third/newcombobox.h \
    third/newqlineedit.h \
    third/switchbutton.h \
    third/third_config.h \
    userconfigure.h

FORMS += \
    app/atcmdwindow.ui \
    app/lanwindow.ui \
    app/mutilstringsend.ui \
    app/productconfig.ui \
    app/subDevwindow.ui \
    kernel/mmessagebox.ui \
#    kernel/toolupdate.ui \
    subwindow/addconfigwindow.ui \
    app/configwindow.ui \
    app/debugwindow.ui \
    subwindow/hexstringshow.ui \
    app/infowindow.ui \
    mainwindow.ui \
    app/modelwindow.ui \
    app/otawindow.ui \
    app/passwindow.ui \
    subwindow/inputdialog.ui \
    subwindow/stringshow.ui \
    subwindow/subdevcomm.ui \
    subwindow/subdevcreate.ui

TRANSLATIONS += \
    chinese.ts english.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



RESOURCES += \
    icon.qrc \
    png.qrc

RC_FILE += icon/quectel.rc

DISTFILES += \
    icon/quectel.ico \
    icon/quectel.rc
#QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG
#QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
#QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

#不同的构建套件 debug release 依赖不同的链接库
CONFIG(debug, debug|release){
LIBS += -L$$PWD/quc/ -lqucd
} else {
LIBS += -L$$PWD/quc/ -lquc
}




#CONFIG(release, debug|release){
#QMAKE_LFLAGS += /MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"
#}
