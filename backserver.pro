QT -= gui
QT += network
QT += sql

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_STATIC

INCLUDEPATH += \
        soft-se/ \
        httpsServer/ \
        tcpServer/ \
        api/ \
        integrations/

win32{
    LIBS += -LC:/Qt/Tools/OpenSSL/Win_x64/lib -llibssl -llibcrypto
    INCLUDEPATH += C:/Qt/Tools/OpenSSL/Win_x64/include/
}
osx{
    LIBS += -L/usr/local/opt/openssl/lib -L/usr/local/lib
    INCLUDEPATH += /usr/local/opt/openssl/include
}
SOURCES += \
    httpsServer/http_parser.c \
    httpsServer/httpheader.cpp \
    httpsServer/httpsclient.cpp \
    httpsServer/httpsserver.cpp \
        logger.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    httpsServer/http_parser.h \
    httpsServer/httpheader.h \
    httpsServer/httpsclient.h \
    httpsServer/httpsserver.h \
    logger.h

#QMAKE_POST_LINK += $$(QTDIR)/bin/windeployqt $$OUT_PWD/release --compiler-runtime --qmldir $$PWD $$escape_expand(\n\t)
