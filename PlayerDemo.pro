#-------------------------------------------------
#
# Project created by QtCreator 2013-12-05T18:27:01
#
#-------------------------------------------------
#QMAKE_CXXFLAGS += -std=c++11

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlayerDemo
TEMPLATE = app

CONFIG += c++11


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

DEFINES = __STDC_CONSTANT_MACROS

win32 {
        DEFINES += _CRT_SECURE_NO_WARNINGS
        INCLUDEPATH += "C:/Program Files/portaudio-r1891-build/include" \
        "C:\Program Files\Mega-Nerd\libsndfile\include" \
        "C:/Program Files/ffmpeg-20131202-git-e3d7a39-win32-dev/include"

        LIBS += "C:/Program Files/Mega-Nerd/libsndfile/lib/libsndfile-1.lib"
        #LIBS += "C:/Program Files/ffmpeg-20131202-git-e3d7a39-win32-shared/bin/avformat-55.dll"
        #LIBS += "C:/Program Files/ffmpeg-20131202-git-e3d7a39-win32-shared/bin/avcodec-55.dll"
        #LIBS += "C:/Program Files/ffmpeg-20131202-git-e3d7a39-win32-shared/bin/avutil-52.dll"
        #LIBS += "C:/Program Files/mpg123-1.15.0-x86/libmpg123-0.dll"

        LIBS += "C:\Program Files\portaudio-r1891-build\lib\Win32\Release\portaudio_x86.lib"

}

win32-msvc2012{
        INCLUDEPATH += "C:/Program Files/Microsoft SDKs/Windows/v7.0A/include"
        INCLUDEPATH += . \
        "C:/Qt_Projekte/PSndPlayer"
        LIBS += "C:/Qt_Projekte/build-PSndPlayer-Desktop_Qt_5_2_0_MSVC2012_32bit-Debug/debug/PSndPlayer.lib"
        LIBS+= "C:/Qt_Projekte/meta-build-Desktop_Qt_4_8_4_MinGw2-Release/release/libmeta.a"

}

win32-msvc2013{
        DEFINES += _USING_V110_SDK71_
        INCLUDEPATH += . \
        "C:/QtProjects/PSndPlayer"
        LIBS += "C:/QtProjects/build-PSndPlayer-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug/debug/PSndPlayer.lib"
        #LIBS += "C:/QtProjects/build-PSndPlayer-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Release/release/PSndPlayer.lib"
}

macx {
        greaterThan(QT_MAJOR_VERSION, 4):{
        INCLUDEPATH += /Users/admin/Developer/QtProjects/PSndPlayer
        INCLUDEPATH +=    /usr/local/Cellar/portaudio/19.20111121/include
        INCLUDEPATH +=    /usr/local/Cellar/libsndfile/1.0.25/include

        LIBS +=    -L/usr/local/Cellar/portaudio/19.20111121/lib
        LIBS +=    -L/usr/local/Cellar/libsndfile/1.0.25/lib
        }
    lessThan(QT_MAJOR_VERSION, 5):{
        QMAKE_CXXFLAGS += -std=c++11
        #QMAKE_LFLAGS += -F/System/Library/Frameworks \
        #                -F//Developer/Library/PrivateFrameworks
        #QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.5
        #QMAKE_CXXFLAGS += -mmacosx-version-min=10.5

        #INCLUDEPATH += /Developer/SDKs/libsndfile/libsndfile_86_10.5/local/include
        #INCLUDEPATH += /Developer/Qt_projekte/PSndWave
        #INCLUDEPATH += /Developer/Qt_projekte/PSndWave/QWave2
        INCLUDEPATH += /Developer/Qt_Projekte/PSndPlayer

        INCLUDEPATH += /Users/adminM/Developer/QtProjects/PSndPlayer
        #INCLUDEPATH += . \
        #/usr/local/include
        #../src

        #LIBS += -L/Developer/Qt_Projekte/PSndWave-build-Desktop_GCC_x86_32bit-Debug
        #LIBS += -L/Developer/Qt_Projekte/PSndPlayer-build-Desktop_GCC_x86_32bit-Debug
        #LIBS += -L/Developer/SDKs/libmp123/x86/lib
        #LIBS += -L/Developer/SDKs/meta/x86/lib
        #LIBS += -L/Developer/SDKs/libsndfile/libsndfile_86_10.5/local/lib
        #LIBS += -framework \
        #Carbon \
        #-framework \
        #CoreAudio
    }
        LIBS += -L/usr/local/lib
        LIBS += -L/usr/local/lib
        LIBS += -lsndfile \
                -lPSndPlayer
}
