#ifndef BACKEND_H
#define BACKEND_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvDeviceGEV.h>
#include <PvDeviceU3V.h>
#include <PvStream.h>
#include <PvStreamGEV.h>
#include <PvStreamU3V.h>
#include <PvBuffer.h>
#include <PvPipeline.h>

#include <PvGenBrowserWnd.h>
#include <PvDeviceFinderWnd.h>


#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <QObject>
#include <QLabel>
#include <QImage>
#include <QString>

#include <ctime>

//#define NOT_GUI
#define BUFFER_COUNT 8

class irCamera:public QObject
{
    Q_OBJECT

public:
//    friend MainWindow displayImg;
//    friend MainWindow image_qt;
    irCamera();
    irCamera(const irCamera& ir);
//    ~irCamera();
    void imgAcquisition();
    void imgAcquisition2();
    void camConfiguration();

signals:
    void signalImgDisplay1(QImage *qImg);
    void signalImgDisplay2(QImage *qImg);
    void signalTest(int abc);

private:
    const PvDeviceInfo *lDeviceInfo;
    PvDevice *lDevice;
    PvStream *lStream;
    PvBuffer *lBuffers;

    PvGenBrowserWnd *lDeviceWnd;
    PvGenBrowserWnd *lCommunicationWnd;
    PvGenBrowserWnd *lStreamParametersWnd;

    PvSystem *lPvSystem;
    PvDeviceFinderWnd *lDeviceFinderWnd;

    PvPipeline *lPipeline;

    unsigned int imgStamp;


    const PvDeviceInfo* SelectDevice();
    PvDevice* ConnectToDevice();
    PvStream* OpenStream();
    void ShowGenWindow(PvGenBrowserWnd **aWnd, PvGenParameterArray *aParams);
    void ConfigureStream();
    void CreateStreamBuffers();


public:
    void imageDisplay();
    void deviceConnection();

};



#endif // BACKEND_H

