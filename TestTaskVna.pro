QT += core gui charts network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    InteractionSVNA/InteractionSVNA.cpp \
    CommunicationSVNA/Socket.cpp \
    InteractionSVNA/InteractionSVNAbySocket.cpp \
    Handler.cpp \
    Main.cpp \
    MainWindow.cpp \
    Converters.cpp


HEADERS += \
    CommunicationSVNA/CommunicationSVNA.h \
    CommunicationSVNA/Socket.h \
    InteractionSVNA/InteractionSVNA.h \
    InteractionSVNA/InteractionSVNAbySocket.h \
    Device.h \
    Handler.h \
    MainWindow.h \
    Converters.h \
    AppSettings.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
