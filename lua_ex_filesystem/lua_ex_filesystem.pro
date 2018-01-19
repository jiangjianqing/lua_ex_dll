#-------------------------------------------------
#
# Project created by QtCreator 2018-01-19T15:51:39
#
#-------------------------------------------------

QT       -= core gui
CONFIG += c++14
TARGET = xfilesystem
TEMPLATE = lib

#使Release版本可调试
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

DEFINES += LUA_EXTEND_TEST_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#define platform variable for folder name
win32 {contains(QMAKE_TARGET.arch, x86_64) {PLATFORM = x64} else {PLATFORM = Win32}}
macx {PLATFORM = macx}
unix:!macx:!android {PLATFORM = linux}

#define configuration variable for folder name
CONFIG(debug, debug|release) {CONFIGURATION = Debug} else {CONFIGURATION = Release}


DESTDIR = ../_bin/$$TARGET/$$CONFIGURATION/$$PLATFORM
OBJECTS_DIR = ../_intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM
MOC_DIR = ../_intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM
RCC_DIR = ../_intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM
UI_DIR = ../_intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM

HEADERS     += $$files(*.h,true)
SOURCES     += $$files(*.cpp,true)
SOURCES     += $$files(*.cc,true)
FORMS       += $$files(*.ui,true)
RESOURCES   += $$files(*.qrc,true)
OTHER_FILES += $$files(*,true)

win32: {

    #重要：目的就是让别的头文件别包含了”winsock.h”内容，否则加入libuv会导致编译错误： 'sockaddr': 'struct' type redefinition
    DEFINES += _WINSOCKAPI_
    LIBS += wsock32.lib Ws2_32.lib Advapi32.lib User32.lib Iphlpapi.lib Psapi.lib Userenv.lib

    #为编译生成lua可以调用的 dll，window下必须加入该定义
    DEFINES += LUA_BUILD_AS_DLL

    CONFIG(release, debug|release):{
        #LIBS += -L$$PWD/../_lib/protobuf/lib/$$PLATFORM -llibprotobuf
    }
    else:CONFIG(debug, debug|release): {
        #LIBS += -L$$PWD/../_lib/protobuf/lib/$$PLATFORM -llibprotobufd
    }


    #copy stuff after compiling
    #resources.path = ../_bin/$$CONFIGURATION/$$PLATFORM
    #resources.files += media/process_icons
    #INSTALLS += resources

    #清理目标目录
    QMAKE_POST_LINK +=  rmdir /s/q  .\\$$DESTDIR\\config & \

    #在release时才会执行windeployqt
    #CONFIG(release, debug|release): QMAKE_POST_LINK +=  windeployqt.exe --no-angle --no-svg --no-system-d3d-compiler --no-quick-import --no-translations $$DESTDIR\\$$TARGET.exe & \
    #else:CONFIG(debug, debug|release):{
    #LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lIPL
    #QMAKE_POST_LINK +=  windeployqt.exe --no-angle --no-svg --no-system-d3d-compiler --no-quick-import --no-translations ../_bin/$$CONFIGURATION/$$PLATFORM/$$TARGET.exe & \

    #2018.01.19 QMAKE_POST_LINK对分割符有要求，windows用 "\\",linux用"/"
    QMAKE_POST_LINK +=  $${QMAKE_COPY_DIR} ..\\_lib\\windows $$DESTDIR & \
                        $${QMAKE_COPY_DIR} ..\\_lib\\windows\\$$CONFIGURATION $$DESTDIR & \
                        $${QMAKE_COPY_DIR} media\\config $$DESTDIR\\config\\ & \
#                        $${QMAKE_COPY_DIR} media\\process_icons ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\process_icons\\ & \
#                        $${QMAKE_COPY_DIR} media\\examples ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\examples\\ & \
#                        $${QMAKE_COPY_DIR} media\\examples\images ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\examples\\images\\ & \
#                        $${QMAKE_COPY_DIR} media\\plugin_development ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\plugin_development & \
#                        $${QMAKE_COPY_DIR} ..\\IPL\\include ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\plugin_development\\_lib\\include & \
#                        del ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\IPL.exp & \
#                        del ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\IPL.lib & \

}

linux: {
    INCLUDEPATH += $$PWD/../_lib/boost_1_66_0/include
    LIBS += -L$$PWD/../_lib/boost_1_66_0/lib -lboost_system -lboost_filesystem
}
#加入通用lib支持

#特别注意:在linux下，-l后面需要忽略"lib"字符，比如libuv.so，就要写成-luv
LIBS += -L$$PWD/../_lib/libuv/lib/$$PLATFORM -luv
INCLUDEPATH += $$PWD/../_lib/libuv/include

INCLUDEPATH += $$PWD/../_lib/lua/include
LIBS += -L$$PWD/../_lib/lua/lib/$$PLATFORM -llua
linux: {
    LIBS +=-ldl  #显式加载动态库的动态函数库,解决 undefined reference to symbol 'dlclose@@GLIBC_2.2.5'
}

INCLUDEPATH += $$PWD/include/

message($$TARGET" project Defines:")
message($$DEFINES)
