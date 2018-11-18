#include "ftpgetwindow.h"

#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QGridLayout>
#include <QFileDialog>

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QFileInfo>
#include <QMetaEnum>
#include <QtNetwork/QNetworkAccessManager>

FtpgetWindow::FtpgetWindow(QWidget *parent)
    : QWidget(parent),
      fileDownloadSize(0),
      lastDownloadSize(0),
      file(Q_NULLPTR)
{
    downloadManager = new QNetworkAccessManager(this);
    connect(downloadManager, SIGNAL(finished(QNetworkReply*)),SLOT(downloadFinishReply(QNetworkReply*)));

    //初始化超时检查定时器，30秒查询一次
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), SLOT(timeOut()));

    urlLabel = new QLabel;
    urlLabel->setText(tr("Url:"));

    urlTextEdit = new QLineEdit;
    urlLabel->setBuddy(urlTextEdit);

    runningTaskButton = new QPushButton;
    runningTaskButton->setText("Run");
    connect(runningTaskButton, SIGNAL(clicked(bool)), SLOT(updateTaskRunningState()));

    dirLoactionLabel = new QLabel;
    dirLoactionLabel->setText(tr("Save Dir:"));

    dirTextEdit = new QLineEdit;
    dirTextEdit->setReadOnly(true);
    dirLoactionLabel->setBuddy(dirTextEdit);

    dirLocationButton = new QPushButton;
    dirLocationButton->setText("Select Save Dir");
    connect(dirLocationButton, SIGNAL(clicked(bool)), SLOT(updateSelectSaveDir()));

    runningTipLabel = new QLabel;
    runningTipLabel->setText(tr("Runing task:"));

    progressBar = new QProgressBar;
    runningTipLabel->setBuddy(progressBar);

    downlaodInfoLabel = new QLabel;
    downlaodInfoLabel->setText(tr("Download Info:"));

    downloadInfoTextEdit = new QTextEdit;
    downloadInfoTextEdit->setReadOnly(true);
    downlaodInfoLabel->setBuddy(downloadInfoTextEdit);

    mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 3);
    mainLayout->setColumnStretch(2, 1);
    mainLayout->setMargin(15);
    mainLayout->setColumnMinimumWidth(2, 15);

    mainLayout->addWidget(urlLabel, 0, 0);
    mainLayout->addWidget(urlTextEdit, 0, 1);
    mainLayout->addWidget(runningTaskButton, 0, 2);
    mainLayout->addWidget(dirLoactionLabel, 1, 0);
    mainLayout->addWidget(dirTextEdit, 1, 1);
    mainLayout->addWidget(dirLocationButton, 1, 2);
    mainLayout->addWidget(runningTipLabel, 2, 0, 1, 1);
    mainLayout->addWidget(progressBar, 2, 1, 1, 1);
    mainLayout->addWidget(downlaodInfoLabel, 3, 0, 1, 1);
    mainLayout->addWidget(downloadInfoTextEdit, 4, 0, 3, 3);
    setLayout(mainLayout);

    setFixedWidth(800);
    setWindowTitle(tr("FpGet Window"));
}

FtpgetWindow::~FtpgetWindow()
{
    if(file != Q_NULLPTR)
    {
        file->deleteLater();
        file = Q_NULLPTR;
    }
    //downloadManager的父对象是窗体，会自动进行析构
}

/**
 * @brief 进行下载超时判断，错误则发送超时信号
 */
void FtpgetWindow::timeOut()
{
    if(lastDownloadSize != fileDownloadSize)
        lastDownloadSize = fileDownloadSize;
    else
        emit downloadReply->error(QNetworkReply::TimeoutError); //下载超时，发送超时错误信号
}

/**
 * @brief 检查Url地址合法性
 * @return
 */
bool FtpgetWindow::checkUrl()
{
    url = QUrl(urlTextEdit->text());
    if(!url.isValid())
    {
        downloadInfoTextEdit->append("Error: Invalid URL");
        return false;
    }

    if(url.scheme() != "ftp")
    {
        downloadInfoTextEdit->append("Error: URL must start with 'ftp:'");
        return false;
    }

    if (url.path().isEmpty()) {
        downloadInfoTextEdit->append("Error: URL has no path");
        return false;
    }
    return true;
}

/**
 * @brief 检查文件下载地址
 * @return
 */
bool FtpgetWindow::checkSaveDir()
{
    QString dir = dirTextEdit->text();
    if(dir.isEmpty())
        dir = QDir::currentPath() + "/Download/";
    saveDir = QDir(dir);

    if(!saveDir.exists())
    {
        auto ok = saveDir.mkdir(dir);
        if(!ok) return false;
    }
    return true;
}

bool FtpgetWindow::createDownloadFile()
{
    auto localFileName = QFileInfo(url.path()).fileName();
    if (localFileName.isEmpty())
        localFileName = "ftpget.out";

    file = new QFile;
    file->setFileName(saveDir.absoluteFilePath(localFileName));
    if(!file->open(QIODevice::WriteOnly))
    {
        auto info = "Error: Cannot write file " + file->fileName()
                + ": " + file->errorString();
        downloadInfoTextEdit->append(info);
        return false;
    }
    return true;
}

/**
 * @brief 开始下载文件操作
 */
void FtpgetWindow::startDownloadFile()
{
    if(!createDownloadFile()) return;

    if(timer->isActive())
        timer->stop();
    fileDownloadSize = lastDownloadSize = 0; //重新设置定时器以及相关变量

    downloadInfoTextEdit->append("Download file: " + url.fileName());

    downloadReply = downloadManager->get(QNetworkRequest(url));

    //分块获取文件信息，并写入文件中
    connect(downloadReply, SIGNAL(readyRead()), SLOT(slotReadyRead()));

    //获取下载进度信息
    connect(downloadReply, SIGNAL(downloadProgress(qint64,qint64)),
            SLOT(downloadProgress(qint64,qint64)));

    //下载过程出错，进行报错处理（超时处理也是丢出超时信号，交由此槽函数进行处理）
    connect(downloadReply, SIGNAL(error(QNetworkReply::NetworkError)),
            SLOT(readReplyError(QNetworkReply::NetworkError)));

    timer->start(30 * 1000); //启动超时检查定时器，每30秒查询下载情况
}

void FtpgetWindow::updateSelectSaveDir()
{
    dirTextEdit->setText("");
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "C://",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(!dir.isEmpty())
        dirTextEdit->setText(dir);
}

void FtpgetWindow::updateTaskRunningState()
{
    if(!checkUrl() || !checkSaveDir())
        return;

    downloadInfoTextEdit->clear(); //清空信息栏

    runningTaskButton->setEnabled(false);
    dirLocationButton->setEnabled(false);
    startDownloadFile();
}

/**
 * @brief 文件下载完成的清尾操作
 * @param reply
 */
void FtpgetWindow::downloadFinishReply(QNetworkReply *reply)
{
    file->waitForBytesWritten(5 * 1000); //等待文件写入结束
    if(0 == file->size())
        //此处下载失败，不再进行重新下载操作
        downloadInfoTextEdit->append("Nothing be download.");
    else
        downloadInfoTextEdit->append("Download file success.");

    if(timer->isActive())
        timer->stop(); //停止超时计时器

    file->deleteLater();
    file = Q_NULLPTR;

    reply->deleteLater();
    reply = Q_NULLPTR;

    runningTaskButton->setEnabled(true);
    dirLocationButton->setEnabled(true);
}

void FtpgetWindow::slotReadyRead()
{
    file->write(downloadReply->readAll());
    fileDownloadSize = file->size(); //更新下载字节数
}

/**
 * @brief 下载异常，重新进行下载
 * @param error
 */
void FtpgetWindow::readReplyError(QNetworkReply::NetworkError error)
{
    auto metaEnum = QMetaEnum::fromType<QNetworkReply::NetworkError>();
    //PS:字符串转换为枚举值
    //Qt::Alignment alignment = (Qt::Alignment)metaEnum.keyToValue("Qt::AlignLeft");
    //alignment = (Qt::Alignment)metaEnum.keysToValue("Qt::AlignLeft | Qt::AlignVCenter");
    //枚举值转换为字符串
    auto errStr = metaEnum.valueToKey(error);
    downloadInfoTextEdit->append("Download file occur error: " + QString(errStr));

    file->deleteLater();
    file = Q_NULLPTR;

    downloadReply->deleteLater();
    downloadReply = Q_NULLPTR;

    startDownloadFile(); //重新尝试下载文件
}

void FtpgetWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(0 != bytesTotal)
    {
        progressBar->setMaximum(bytesTotal);
        progressBar->setValue(bytesReceived);
    }
}
