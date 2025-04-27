QT += quick winextras network

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../Utility/DumpUtil.cpp \
        ../Utility/IcrCriticalSection.cpp \
        ../Utility/ImCharset.cpp \
        ../Utility/ImPath.cpp \
        ../Utility/LogBuffer.cpp \
        ../Utility/LogUtil.cpp \
        ../Utility/httpclientbase.cpp \
        main.cpp \
        maincontroller.cpp \
        settingmanager.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

RC_ICONS = content/res/logo.ico

QMAKE_CXXFLAGS += -DQT_MESSAGELOGCONTEXT

# Enable PDB generation
QMAKE_CFLAGS_RELEASE += /Zi
QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG

HEADERS += \
    ../Utility/DumpUtil.h \
    ../Utility/IcrCriticalSection.h \
    ../Utility/ImCharset.h \
    ../Utility/ImPath.h \
    ../Utility/LogBuffer.h \
    ../Utility/LogMacro.h \
    ../Utility/LogUtil.h \
    ../Utility/httpclientbase.h \
    maincontroller.h \
    settingmanager.h

DISTFILES +=
