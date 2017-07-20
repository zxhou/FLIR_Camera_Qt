
#include <QAction>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>

//#include <QString>



#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setWindowTitle(tr("Main Window"));//设置主窗口标题
    //创建一个openAction对象，在构造函数中传入一个图标，一个文本和this指针
    openAction = new QAction(QIcon(":/images/doc-open"),tr("&Open..."),this);
    openAction->setShortcuts(QKeySequence::Open);//设置openAction的快捷键
    openAction->setStatusTip(tr("Open an existing file"));//鼠标滑过这个openAction时状态栏会显示状态
    //由openAction的triggered信号触发mainWindow类的open()函数
    connect(openAction,&QAction::triggered,this,&MainWindow::open);

    QMenu *file = menuBar()->addMenu(tr("&File"));
    file->addAction(openAction);

    QToolBar *toolBar = addToolBar(tr("&File"));
    toolBar->addAction(openAction);

    statusBar();

    irCam1 = new irCamera;
    irCam2 = new irCamera;

    deviceCon = new QPushButton(QString("Device1 Selection"),this);
    deviceCon->setGeometry(10,70,120,40);
    QObject::connect(deviceCon,&QPushButton::clicked,irCam1,&irCamera::camConfiguration);

    deviceCon2 = new QPushButton(QString("Device2 Selection"),this);
    deviceCon2->setGeometry(10,150,120,40);
    QObject::connect(deviceCon2,&QPushButton::clicked,irCam2,&irCamera::camConfiguration);

//    deviceCon3 = new QPushButton(QString("Device3 selection"),this);
//    deviceCon3->setGeometry(10,230,120,40);
//    QObject::connect(deviceCon3,&QPushButton::clicked,&irCam3,&irCamera::camConfiguration);

    play1 = new QPushButton(QString("Play Camera1"),this);
    play1->setGeometry(10,230,120,40);

    QObject::connect(play1,&QPushButton::clicked,this,&MainWindow::imgPlay);

//    play2 = new QPushButton(QString("Play Camera2"),this);
//    play2->setGeometry(150,150,120,40);
//    QObject::connect(play2,&QPushButton::clicked,this,&MainWindow::imgPlay2);

    displayImg = new QLabel("image1",this);
    displayImg->setGeometry(300,70,500,500);

//    image_qt = new QImage("D:/robot.jpg");
    QObject::connect(irCam1,&irCamera::signalImgDisplay1,this,&MainWindow::imgDisplay1);
//    QObject::connect(&irCam1, SIGNAL(signalImgDisplay1(QImage*)), this, SLOT(imgDisplay1(QImage*)));
    QObject::connect(irCam1,&irCamera::signalTest,this,&MainWindow::slot_second_test);

//    QObject::connect(this,&MainWindow::signal_seconde_test,this,&MainWindow::slot_second_test);
//    emit signal_seconde_test(100);

}

MainWindow::~MainWindow()
{
    delete openAction;
}

void MainWindow::open()
{
    QMessageBox::information(this,tr("Information"),tr("Open"));
}

void MainWindow::imgPlay()
{
    boost::thread cameraThread1(boost::bind(&irCamera::imgAcquisition, *irCam1));
    boost::thread cameraThread2(boost::bind(&irCamera::imgAcquisition2, *irCam2));

}

void MainWindow::imgDisplay1(QImage *qImg)
{
    cout<<"display image process"<<endl;
    displayImg->setPixmap(QPixmap::fromImage(*qImg));
}

void MainWindow::slotTest(int abbb)
{
    cout<<"slot test is successed "<<abbb<<endl;
}

void MainWindow::slot_second_test(int a)
{
        cout<<"slot_second_test is succeeded"<<endl;
}



