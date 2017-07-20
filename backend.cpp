

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <mainwindow.h>
#include "backend.h"
using namespace std;

irCamera::irCamera()
{

}
irCamera::irCamera(const irCamera& ir)
{
    lDeviceInfo = ir.lDeviceInfo;
    lDevice = ir.lDevice;
    lStream = ir.lStream;
    lBuffers = ir.lBuffers;
    lDeviceWnd = ir.lDeviceWnd;
    lCommunicationWnd = ir.lCommunicationWnd;
    lStreamParametersWnd = ir.lStreamParametersWnd;
    lPvSystem = ir.lPvSystem;
    lDeviceFinderWnd = ir.lDeviceFinderWnd;
    lPipeline = ir.lPipeline;
    imgStamp = ir.imgStamp;
}

/*
irCamera::~irCamera()
{
    cout << "Closing stream" << endl;
    lStream->Close();
    PvStream::Free(lStream);

    cout << "Disconnecting device" << endl;
    lDevice->Disconnect();
    PvDevice::Free(lDevice);

    cout << endl;
    cout << "<press a key to exit>" << endl;
    PvWaitForKeyPress();

    delete lPipeline;
    lPipeline = NULL;

#ifdef NOT_GUI
    delete lPvSystem;
    lPvSystem = NULL;
#else
    delete lDeviceFinderWnd;
    lDeviceFinderWnd = NULL;
#endif // PV_GUI_NOT_AVAILABLE
    PV_SAMPLE_TERMINATE();
}
*/
/*
void irCamera::signalImgDisplay1(QImage *qImg)
{

}

void irCamera::signalImgDisplay2(QImage *qImg)
{

}
*/

void irCamera::camConfiguration()
{
    cout<<"click the button"<<endl;
#ifdef NOT_GUI
    lPvSystem = new PvSystem;
    lDeviceInfo = SelectDevice();
#else
    lDeviceFinderWnd = new PvDeviceFinderWnd();
    lDeviceInfo = SelectDevice();
#endif // PV_GUI_NOT_AVAILABLE

    if (lDeviceInfo != NULL)
    {
        lDevice = ConnectToDevice();
        lStream = OpenStream();
    }
    ConfigureStream();
    CreateStreamBuffers();
    ShowGenWindow(&lDeviceWnd, lDevice->GetParameters());

    imgStamp = 0;
}

#ifdef NOT_GUI
const PvDeviceInfo* irCamera::SelectDevice()
{
    const PvDeviceInfo *lDeviceInfo = NULL;
    PvResult lResult;
    if (NULL != lPvSystem)
    {
        // Get the selected device information.
        lDeviceInfo = PvSelectDevice(*lPvSystem);
    }
    return lDeviceInfo;
}

#else
const PvDeviceInfo* irCamera::SelectDevice()
{
    const PvDeviceInfo *lDeviceInfo = NULL;
    PvResult lResult;
    if (NULL != lDeviceFinderWnd)
    {
        // Display list of GigE Vision and USB3 Vision devices
        lResult = lDeviceFinderWnd->ShowModal();
        if (!lResult.IsOK())
        {
            // User hit cancel
            cout << "No device selected." << endl;
            return NULL;
        }
        // Get the selected device information.
        lDeviceInfo = lDeviceFinderWnd->GetSelected();
    }
    return lDeviceInfo;
}
#endif

PvDevice* irCamera::ConnectToDevice()
{
    PvDevice *lDevice;
    PvResult lResult;
    // Connect to the GigE Vision or USB3 Vision device
    cout << "Connecting to " << lDeviceInfo->GetDisplayID().GetAscii() << "." << endl;
    lDevice = PvDevice::CreateAndConnect(lDeviceInfo, &lResult);
    if (lDevice == NULL)
    {
        cout << "Unable to connect to " << lDeviceInfo->GetDisplayID().GetAscii() << "." << endl;
    }
    return lDevice;
}


PvStream* irCamera::OpenStream()
{
    PvStream *lStream;
    PvResult lResult;
    // Open stream to the GigE Vision or USB3 Vision device
    cout << "Opening stream to device." << endl;
    lStream = PvStream::CreateAndOpen(lDeviceInfo->GetConnectionID(), &lResult);
    if (lStream == NULL)
    {
        cout << "Unable to stream from " << lDeviceInfo->GetDisplayID().GetAscii() << "." << endl;
    }
    return lStream;
}

void irCamera::ShowGenWindow(PvGenBrowserWnd **aWnd, PvGenParameterArray *aParams)
{
    // Create, assigne parameters, set title and show modeless
    (*aWnd) = new PvGenBrowserWnd;
//	(*aWnd)->SetTitle(PvString(aTitle));
    (*aWnd)->SetGenParameterArray(aParams);
    (*aWnd)->ShowModeless();
}

void irCamera::ConfigureStream()
{
    // If this is a GigE Vision device, configure GigE Vision specific streaming parameters
    PvDeviceGEV* lDeviceGEV = dynamic_cast<PvDeviceGEV *>(lDevice);
    if (lDeviceGEV != NULL)
    {
        PvStreamGEV *lStreamGEV = static_cast<PvStreamGEV *>(lStream);
        // Negotiate packet size
        lDeviceGEV->NegotiatePacketSize();
        // Configure device streaming destination
        lDeviceGEV->SetStreamDestination(lStreamGEV->GetLocalIPAddress(), lStreamGEV->GetLocalPort());
    }

}



void irCamera::CreateStreamBuffers()
{
    // Reading payload size from device
    uint32_t lSize = lDevice->GetPayloadSize();
    // Use BUFFER_COUNT or the maximum number of buffers, whichever is smaller
    uint32_t lBufferCount = (lStream->GetQueuedBufferMaximum() < BUFFER_COUNT) ?
        lStream->GetQueuedBufferMaximum() :
        BUFFER_COUNT;
    // Allocate buffers
    lBuffers = new PvBuffer[lBufferCount];
    for (uint32_t i = 0; i < lBufferCount; i++)
    {
        (lBuffers + i)->Alloc(static_cast<uint32_t>(lSize));
    }
    // Queue all buffers in the stream
    for (uint32_t i = 0; i < lBufferCount; i++)
    {
        lStream->QueueBuffer(lBuffers + i);
    }
}

void irCamera::imgAcquisition()//, PvDevice *aDevice2, PvStream *aStream2)
{
    // Get device parameters need to control streaming
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    // Map the GenICam AcquisitionStart and AcquisitionStop commands
    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>(lDeviceParams->Get("AcquisitionStart"));
    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>(lDeviceParams->Get("AcquisitionStop"));
    // Get stream parameters
    PvGenParameterArray *lStreamParams = lStream->GetParameters();

    // Map a few GenICam stream stats counters
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>(lStreamParams->Get("AcquisitionRate"));
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>(lStreamParams->Get("Bandwidth"));

    // Enable streaming and send the AcquisitionStart command
    std::cout << "Enabling streaming and sending AcquisitionStart command." << endl;
    lDevice->StreamEnable();
    lStart->Execute();

    char lDoodle[] = "|\\-|-/";
    int lDoodleIndex = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;
    // Acquire images until the user instructs us to stop.
    printf("\n<press a key to stop streaming>\n");//cout << endl << "<press a key to stop streaming>" << endl;
    while (!PvKbHit())
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;
        // Retrieve next buffer
        PvResult lResult = lStream->RetrieveBuffer(&lBuffer, &lOperationResult, 1000);

        if (lResult.IsOK())
        {
            if (lOperationResult.IsOK())
            {
                PvPayloadType lType;// , lType2;

                //
                // We now have a valid buffer. This is where you would typically process the buffer.
                // -----------------------------------------------------------------------------------------
                // ...
                lFrameRate->GetValue(lFrameRateVal);
                lBandwidth->GetValue(lBandwidthVal);
                // If the buffer contains an image, display width and height.
                uint32_t lWidth = 0, lHeight = 0;
                lType = lBuffer->GetPayloadType();
//				lType2 = lBuffer2->GetPayloadType();
                std::cout << fixed << setprecision(1);
                std::cout << lDoodle[lDoodleIndex];
                std::cout << " BlockID: " << uppercase << hex << setfill('0') << setw(16) << lBuffer->GetBlockID();
                if (lType == PvPayloadTypeImage)// && lType2 == PvPayloadTypeImage)
                {
                    // Get image specific buffer interface.
//                    PvImage *lImage = lBuffer->GetImage();
                    uint8_t *pSrc;
//					uint8_t *pSrc2;

                    pSrc = lBuffer->GetDataPointer();
//					pSrc2 = lBuffer2->GetDataPointer();

                    cv::Mat irImg(512, 640, CV_8U, pSrc);
//					cv::Mat irImg2(512, 640, CV_8U, pSrc2);

                    clock_t timems = clock();

                    std::string title0 = "irImg0 FPS:" + std::to_string(lFrameRateVal);
//					std::string title1 = "irImg1 FPS:" + std::to_string(lFrameRateVal);

                    std::string imgPath = "D:/Qt_workspace/fifth_test/camera1/" + std::to_string((double)timems);
                    imgPath = imgPath + ".png";


                    putText(irImg, title0, cv::Point(20, 30), CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0), 1, 2, false);

                    QImage *image_qt = new QImage((const unsigned char*)(irImg.data),irImg.cols,irImg.rows,QImage::Format_Grayscale8);
//                    if(!irImg.empty())
//                    {
//                       cout<<"return the QImage"<<endl;
//                       emit signalImgDisplay1(image_qt);
//                       emit signalTest(5);
//                    }

                    cv::imshow("ir1", irImg);
                    cv::waitKey(1);

                    cv::imwrite(imgPath,irImg);

                    // Read width, height.
                    lWidth = lBuffer->GetImage()->GetWidth();
                    lHeight = lBuffer->GetImage()->GetHeight();
                    std::cout << " W: " << dec << lWidth << " H: " << lHeight;
                }
                else {
                    std::cout << " (buffer does not contain image)";
                }
                std::cout << " " << lFrameRateVal << " FPS " << (lBandwidthVal / 1000000.0) << " Mb/s \r";
            }
            else
            {
                // Non OK operation result
                std::cout << lDoodle[lDoodleIndex] << " " << lOperationResult.GetCodeString().GetAscii() << "\r";
            }
            // Re-queue the buffer in the stream object.
            lStream->QueueBuffer(lBuffer);
        }
        else
        {
            // Retrieve buffer failure
            std::cout << lDoodle[lDoodleIndex] << " " << lResult.GetCodeString().GetAscii() << "\r";
        }
        ++lDoodleIndex %= 6;
    }
    PvGetChar(); // Flush key buffer for next stop.
    std::cout << endl << endl;
    // Tell the device to stop sending images.
    std::cout << "Sending AcquisitionStop command to the device" << endl;
    lStop->Execute();
    // Disable streaming on the device
    std::cout << "Disable streaming on the controller." << endl;
    lDevice->StreamDisable();

    // Abort all buffers from the stream and dequeue
    std::cout << "Aborting buffers still in stream" << endl;
    lStream->AbortQueuedBuffers();
    while (lStream->GetQueuedBufferCount() > 0)
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;
        lStream->RetrieveBuffer(&lBuffer, &lOperationResult);
    }
}


void irCamera::imgAcquisition2()//, PvDevice *aDevice2, PvStream *aStream2)
{
    // Get device parameters need to control streaming
    PvGenParameterArray *lDeviceParams = lDevice->GetParameters();
    // Map the GenICam AcquisitionStart and AcquisitionStop commands
    PvGenCommand *lStart = dynamic_cast<PvGenCommand *>(lDeviceParams->Get("AcquisitionStart"));
    PvGenCommand *lStop = dynamic_cast<PvGenCommand *>(lDeviceParams->Get("AcquisitionStop"));
    // Get stream parameters
    PvGenParameterArray *lStreamParams = lStream->GetParameters();
    //	PvGenParameterArray *lStreamParams2 = aStream2->GetParameters();
    // Map a few GenICam stream stats counters
    PvGenFloat *lFrameRate = dynamic_cast<PvGenFloat *>(lStreamParams->Get("AcquisitionRate"));
    PvGenFloat *lBandwidth = dynamic_cast<PvGenFloat *>(lStreamParams->Get("Bandwidth"));

    // Enable streaming and send the AcquisitionStart command
    std::cout << "Enabling streaming and sending AcquisitionStart command." << endl;
    lDevice->StreamEnable();
    lStart->Execute();

    char lDoodle[] = "|\\-|-/";
    int lDoodleIndex = 0;
    double lFrameRateVal = 0.0;
    double lBandwidthVal = 0.0;
    // Acquire images until the user instructs us to stop.
    printf("\n<press a key to stop streaming>\n");//cout << endl << "<press a key to stop streaming>" << endl;
    while (!PvKbHit())
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;
        // Retrieve next buffer
        PvResult lResult = lStream->RetrieveBuffer(&lBuffer, &lOperationResult, 1000);

        if (lResult.IsOK())
        {
            if (lOperationResult.IsOK())
            {
                PvPayloadType lType;// , lType2;

                //
                // We now have a valid buffer. This is where you would typically process the buffer.
                // -----------------------------------------------------------------------------------------
                // ...
                lFrameRate->GetValue(lFrameRateVal);
                lBandwidth->GetValue(lBandwidthVal);
                // If the buffer contains an image, display width and height.
                uint32_t lWidth = 0, lHeight = 0;
                lType = lBuffer->GetPayloadType();
                std::cout << fixed << setprecision(1);
                std::cout << lDoodle[lDoodleIndex];
                std::cout << " BlockID: " << uppercase << hex << setfill('0') << setw(16) << lBuffer->GetBlockID();
                if (lType == PvPayloadTypeImage)// && lType2 == PvPayloadTypeImage)
                {
                    // Get image specific buffer interface.
                    uint8_t *pSrc;

                    pSrc = lBuffer->GetDataPointer();
                    cv::Mat irImg(512, 640, CV_8U, pSrc);

                    std::string title0 = "irImg2 FPS:" + std::to_string(lFrameRateVal);

                    clock_t timems = clock();

                    std::string imgPath = "D:/Qt_workspace/fifth_test/camera2/" + std::to_string((double)timems);
                    imgPath = imgPath + ".png";


                    putText(irImg, title0, cv::Point(20, 30), CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0), 1, 2, false);

                    cv::imshow("ir2", irImg);
                    cv::waitKey(1);
                    cv::imwrite(imgPath,irImg);



                    // Read width, height.
                    lWidth = lBuffer->GetImage()->GetWidth();
                    lHeight = lBuffer->GetImage()->GetHeight();
                    std::cout << " W: " << dec << lWidth << " H: " << lHeight;
                }
                else {
                    std::cout << " (buffer does not contain image)";
                }
                std::cout << " " << lFrameRateVal << " FPS " << (lBandwidthVal / 1000000.0) << " Mb/s \r";
            }
            else
            {
                // Non OK operation result
                std::cout << lDoodle[lDoodleIndex] << " " << lOperationResult.GetCodeString().GetAscii() << "\r";
            }
            // Re-queue the buffer in the stream object.
            lStream->QueueBuffer(lBuffer);
            //			aStream2->QueueBuffer(lBuffer2);
        }
        else
        {
            // Retrieve buffer failure
            std::cout << lDoodle[lDoodleIndex] << " " << lResult.GetCodeString().GetAscii() << "\r";
        }
        ++lDoodleIndex %= 6;
    }
    PvGetChar(); // Flush key buffer for next stop.
    std::cout << endl << endl;
    // Tell the device to stop sending images.
    std::cout << "Sending AcquisitionStop command to the device" << endl;
    lStop->Execute();
    // Disable streaming on the device
    std::cout << "Disable streaming on the controller." << endl;
    lDevice->StreamDisable();

    // Abort all buffers from the stream and dequeue
    std::cout << "Aborting buffers still in stream" << endl;
    lStream->AbortQueuedBuffers();
    while (lStream->GetQueuedBufferCount() > 0)
    {
        PvBuffer *lBuffer = NULL;
        PvResult lOperationResult;
        lStream->RetrieveBuffer(&lBuffer, &lOperationResult);
    }
}
