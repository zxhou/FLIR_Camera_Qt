#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QPushButton>
#include <Qlabel>


#include "backend.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    cv::Mat img1,img2;
    irCamera *irCam1,*irCam2;

signals:
    void signal_seconde_test(int a);

private:
    void open();
    void imgPlay();
    void imgDisplay1(QImage *qImg);
    void slotTest(int abbb);
    void slot_second_test(int a);
    QAction *openAction;
    QPushButton *deviceCon,*deviceCon2,*play1,*play2;

    QLabel *displayImg;
    QImage *image_qt;



};

#endif // MAINWINDOW_H
