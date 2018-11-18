#include <QFileDialog>
#include "mainwindow.h"

#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->uploadProgressBar->setValue(0);

    ftp = new FtpManager("ftp","password","39.105.110.80", 21);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString filenamewithpath;

    filenamewithpath = QFileDialog::getOpenFileName(0, "选择文件", QCoreApplication::applicationDirPath(), "所有文件(*)");

    ui->lineEdit->setText(filenamewithpath);
}

void MainWindow::on_pushButton_2_clicked()
{
    QString path, fileName;

    path = ui->lineEdit->text();
    fileName = "bin";

    ftp->uploadFile(path,fileName);  //上传测试文件
    reply = ftp->getRelay();  //获取

    connect(reply, SIGNAL(finished()), this, SLOT(OnFinishRelay()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(upLoadError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(uploadProgress(qint64 ,qint64)),this,SLOT(OnUploadProgress(qint64,qint64)));
}

/**

* @brief

* @date 2018/11/18

* @param

* @return void

*/
void MainWindow::OnUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    ui->uploadProgressBar->setValue(bytesSent*1.0/bytesTotal*100);
}

/**
* @brief 上传错误
* @date 2018/11/18
* @param
* @return void
*/

void MainWindow::upLoadError(QNetworkReply::NetworkError errorCode)
{

}

/**
* @brief 发送结果回调函数
* @date 2018/11/18
* @param none
* @return void
*/

void MainWindow::OnFinishRelay()
{
    if(reply->error() == QNetworkReply::NoError)
    {
        //成功
    }
    else
    {
        //错误
    }

    reply->deleteLater();

}

