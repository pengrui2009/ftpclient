#ifndef FTPGETWINDOW_H
#define FTPGETWINDOW_H

#include <QWidget>
#include <QUrl>
#include <QDir>
//#include <QNetworkReply>
#include <QtNetwork/QNetworkReply>

class QFile;
class QLabel;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QProgressBar;
class QGridLayout;

class QTimer;
class QNetworkAccessManager;

class FtpgetWindow : public QWidget
{
    Q_OBJECT

public:
    FtpgetWindow(QWidget *parent = 0);
    ~FtpgetWindow();

private slots:
    void timeOut();
    void updateSelectSaveDir();
    void updateTaskRunningState();
    void slotReadyRead();
    void readReplyError(QNetworkReply::NetworkError error);
    void downloadFinishReply(QNetworkReply* reply);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    bool checkUrl();
    bool checkSaveDir();
    bool createDownloadFile();
    void startDownloadFile();

private:
    qint64 fileDownloadSize;
    qint64 lastDownloadSize;
    QUrl url;
    QDir saveDir;
    QFile *file;
    QTimer *timer;
    QNetworkReply *downloadReply;
    QNetworkAccessManager *downloadManager;

    QLabel *urlLabel;
    QLabel *dirLoactionLabel;
    QLabel *downlaodInfoLabel;
    QLabel *runningTipLabel;
    QLineEdit *urlTextEdit;
    QLineEdit *dirTextEdit;
    QTextEdit *downloadInfoTextEdit;
    QPushButton *runningTaskButton;
    QPushButton *dirLocationButton;
    QProgressBar *progressBar;
    QGridLayout *mainLayout;
};

#endif // FTPGETWINDOW_H
