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


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

DEFINES = __STDC_CONSTANT_MACROS

win32 {
        INCLUDEPATH += . \
        "C:/Qt_Projekte/meta_mp" \
        "C:\Program Files\Mega-Nerd\libsndfile\include" \
        "C:/Qt_Projekte/PSndPlayer" \
        "C:/Qt_Projekte/PSndWave/QWave2" \
        "C:/Qt_Projekte/PSndWave" \
        "C:/Program Files/portaudio-r1891-build/include" \
        "C:/Program Files/ffmpeg-20131202-git-e3d7a39-win32-dev/include" \
        "C:/Program Files/ffmpeg-20131202-git-e3d7a39-win32-dev/include"

        #LIBS += "C:/Qt_Projekte/build-PSndPlayer-Desktop_MinGw-Debug/debug/PSndPlayer.dll"
        LIBS += "C:/Qt_Projekte/build-PSndWave-Desktop_Qt_5_0_2_MinGW_32bit-Release/release/PSndWave.dll"
        LIBS += "C:/Qt_Projekte/build-PSndPlayer-Desktop_Qt_5_0_2_MinGW_32bit-Release/release/PSndPlayer.dll"
        #LIBS += "C:/Qt_Projekte/build-PSndWave-Desktop_MinGw-Debug/release/PSndWave.dll"
        LIBS+= "C:/Qt_Projekte/meta-build-Desktop_Qt_4_8_4_MinGw2-Release/release/libmeta.a"
        LIBS += "C:/Program Files/Mega-Nerd/libsndfile/lib/libsndfile-1.lib"
        LIBS += "C:/Program Files/ffmpeg-20131202-git-e3d7a39-win32-shared/bin/avformat-55.dll"
        LIBS += "C:/Program Files/ffmpeg-20131202-git-e3d7a39-win32-shared/bin/avcodec-55.dll"
        LIBS += "C:/Program Files/ffmpeg-20131202-git-e3d7a39-win32-shared/bin/avutil-52.dll"
        LIBS += "C:/Program Files/mpg123-1.15.0-x86/libmpg123-0.dll"
        LIBS += "C:/Program Files/portaudio-r1891-build/lib/Win32/Release/portaudio_x86.dll"

}
