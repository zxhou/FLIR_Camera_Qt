#-------------------------------------------------
#
# Project created by QtCreator 2017-07-19T16:44:30
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fifth_test
TEMPLATE = app

INCLUDEPATH += D:/opencv2413/opencv/build/include
CONFIG(debug,debug|release){
    LIBS += D:\opencv2413\opencv\build\x86\vc12\lib\opencv_ml2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_calib3d2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_contrib2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_core2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_features2d2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_flann2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_gpu2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_highgui2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_imgproc2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_legacy2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_objdetect2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_ts2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_video2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_nonfree2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_ocl2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_photo2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_superres2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_stitching2413d.lib \
            D:\opencv2413\opencv\build\x86\vc12\lib\opencv_videostab2413d.lib \
}
CONFIG(debug,debug|release){
    LIBS += $$quote(C:\Program Files (x86)\Pleora Technologies Inc\eBUS SDK\Libraries\*.lib)
}

#LIBS += -L$$quote(C:\Program Files (x86)\Pleora Technologies Inc\eBUS SDK\Libraries)
INCLUDEPATH += $$quote(C:\Program Files (x86)\Pleora Technologies Inc\eBUS SDK\Includes)

INCLUDEPATH += C:/local/boost_1_60_0

CONFIG(debug,debug|release){
    LIBS += C:\local\boost_1_60_0\lib32-msvc-12.0\libboost_thread-vc120-mt-gd-1_60.lib
    LIBS += C:\local\boost_1_60_0\lib32-msvc-12.0\libboost_date_time-vc120-mt-gd-1_60.lib
    LIBS += C:\local\boost_1_60_0\lib32-msvc-12.0\libboost_system-vc120-mt-gd-1_60.lib
    LIBS += C:\local\boost_1_60_0\lib32-msvc-12.0\libboost_chrono-vc120-mt-gd-1_60.lib
}




SOURCES += main.cpp\
        mainwindow.cpp \
    backend.cpp

HEADERS  += mainwindow.h \
    backend.h

RESOURCES += \
    res.qrc
