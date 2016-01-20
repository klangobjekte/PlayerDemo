@echo off
REM Text wird ausgegeben
echo copy all requiered files to build-directory

echo copy libsndfile-1.dll
copy "C:\Program Files\Mega-Nerd\libsndfile\bin\libsndfile-1.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy meta.dll
copy "C:\QtProjects\build-meta-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit_qt_54_w-Debug\debug\meta.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy PSndPlayer.dll
copy "C:\QtProjects\build-PSndPlayer-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\PSndPlayer.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy portaudio_x86.dll
copy "C:\Program Files\portaudio-r1891-build\lib\Win32\Release\portaudio_x86.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"



echo copy icudt53.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\icudt53.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy icuin53.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\icuin53.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy icuuc53.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\icuuc53.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"


echo copy Qt5Core.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Core.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy Qt5Cored.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Cored.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"

echo copy Qt5DBus.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5DBus.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy Qt5DBusd.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5DBusd.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"

echo copy Qt5Gui.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Gui.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy Qt5Guid.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Guid.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"

echo copy Qt5Multimedia.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Multimedia.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy Qt5Multimediad.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Multimediad.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"

echo copy Qt5Network.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Network.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy Qt5Networkd.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Networkd.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"

echo copy Qt5OpenGL.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5OpenGL.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy Qt5OpenGLd.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5OpenGLd.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"

echo copy Qt5Sql.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Sql.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy Qt5Sqld.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Sqld.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"

echo copy Qt5Xml.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Xml.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy Qt5Xmld.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Xmld.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"

echo copy Qt5Widgets.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Widgets.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
echo copy Qt5Widgetsd.dll
copy "C:\Qt\Qt5.4.2-msvc2013\5.4\msvc2013_opengl\bin\Qt5Widgetsd.dll" "C:\QtProjects\build-PlayerDemo-Desktop_Qt_5_4_2_MSVC2013_OpenGL_32bit-Debug\debug\"
